/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Copied from WebRender's boilerplate.rs.
extern crate gleam;
extern crate glutin;
extern crate time;
extern crate webrender;

use gleam::gl;
use webrender::api::*;

struct Notifier {
    window_proxy: glutin::WindowProxy,
}

impl Notifier {
    fn new(window_proxy: glutin::WindowProxy) -> Notifier {
        Notifier {
            window_proxy,
        }
    }
}

impl RenderNotifier for Notifier {
    fn clone(&self) -> Box<RenderNotifier> {
        Box::new(Notifier {
            window_proxy: self.window_proxy.clone(),
        })
    }

    fn wake_up(&self) {
        #[cfg(not(target_os = "android"))] self.window_proxy.wakeup_event_loop();
    }

    fn new_document_ready(&self, _: DocumentId, _scrolled: bool, _composite_needed: bool) {
        self.wake_up();
    }
}

pub trait HandyDandyRectBuilder {
    fn to(&self, x2: i32, y2: i32) -> LayoutRect;
    fn by(&self, w: i32, h: i32) -> LayoutRect;
}
// Allows doing `(x, y).to(x2, y2)` or `(x, y).by(width, height)` with i32
// values to build a f32 LayoutRect
impl HandyDandyRectBuilder for (i32, i32) {
    fn to(&self, x2: i32, y2: i32) -> LayoutRect {
        LayoutRect::new(
            LayoutPoint::new(self.0 as f32, self.1 as f32),
            LayoutSize::new((x2 - self.0) as f32, (y2 - self.1) as f32),
        )
    }

    fn by(&self, w: i32, h: i32) -> LayoutRect {
        LayoutRect::new(
            LayoutPoint::new(self.0 as f32, self.1 as f32),
            LayoutSize::new(w as f32, h as f32),
        )
    }
}

pub trait Example {
    fn render(
        &mut self,
        api: &RenderApi,
        builder: &mut DisplayListBuilder,
        resources: &mut ResourceUpdates,
        layout_size: LayoutSize,
        pipeline_id: PipelineId,
        document_id: DocumentId,
    );
    fn on_event(&mut self, event: glutin::Event, api: &RenderApi, document_id: DocumentId) -> bool;
    fn get_external_image_handler(&mut self) -> Option<Box<webrender::ExternalImageHandler>> {
        None
    }
    fn get_output_image_handler(&mut self, _gl: &gl::Gl) -> Option<Box<webrender::OutputImageHandler>> {
        None
    }
    fn init(&mut self, _window_proxy: glutin::WindowProxy) {}
    fn needs_repaint(&mut self) -> bool {
        true
    }
    fn should_close_window(&mut self) -> bool {
        false
    }
    fn video_dimensions(&mut self) -> Option<(i32, i32)> {
        None
    }
}

pub fn main_wrapper(example: &mut Example, options: Option<webrender::RendererOptions>) {
    let res_path = None;

    let window = glutin::WindowBuilder::new()
        .with_title("Gecko Media Player")
        .with_multitouch()
        .with_vsync()
        .with_gl(glutin::GlRequest::GlThenGles {
            opengl_version: (3, 2),
            opengles_version: (3, 0),
        })
        .build()
        .unwrap();

    window.hide();
    unsafe {
        window.make_current().ok();
    }

    let gl = match gl::GlType::default() {
        gl::GlType::Gl => unsafe { gl::GlFns::load_with(|symbol| window.get_proc_address(symbol) as *const _) },
        gl::GlType::Gles => unsafe { gl::GlesFns::load_with(|symbol| window.get_proc_address(symbol) as *const _) },
    };

    println!("OpenGL version {}", gl.get_string(gl::VERSION));
    println!("Shader resource path: {:?}", res_path);

    let (width, height) = window.get_inner_size_pixels().unwrap();

    let opts = webrender::RendererOptions {
        resource_override_path: res_path,
        debug: true,
        precache_shaders: true,
        device_pixel_ratio: window.hidpi_factor(),
        ..options.unwrap_or(webrender::RendererOptions::default())
    };

    let size = DeviceUintSize::new(width, height);
    let notifier = Box::new(Notifier::new(window.create_window_proxy()));
    let (mut renderer, sender) = webrender::Renderer::new(gl.clone(), notifier, opts).unwrap();
    let api = sender.create_api();
    let document_id = api.add_document(size, 0);

    example.init(window.create_window_proxy());

    if let Some(external_image_handler) = example.get_external_image_handler() {
        renderer.set_external_image_handler(external_image_handler);
    }
    if let Some(output_image_handler) = example.get_output_image_handler(&*gl) {
        renderer.set_output_image_handler(output_image_handler);
    }

    let epoch = Epoch(0);
    let root_background_color = ColorF::new(0.0, 0.0, 0.0, 1.0);

    let pipeline_id = PipelineId(0, 0);
    api.set_root_pipeline(document_id, pipeline_id);

    let mut counter = 0;
    let start_time = time::precise_time_s();

    if let Some(dimensions) = example.video_dimensions() {
        let (video_width, video_height) = dimensions;
        window.set_inner_size(video_width as u32, video_height as u32);
    }
    window.show();

    let mut window_size = window.get_inner_size_pixels().unwrap();
    'outer: for event in window.wait_events() {
        let mut events = Vec::new();
        events.push(event);

        if example.should_close_window() {
            break 'outer;
        }

        for event in window.poll_events() {
            events.push(event);
        }

        for event in events {
            match event {
                glutin::Event::Closed |
                glutin::Event::KeyboardInput(_, _, Some(glutin::VirtualKeyCode::Escape)) => break 'outer,
                _ => if example.on_event(event, &api, document_id) {},
            }
        }

        // We need to inform WebRender of window resizes, otherwise it will
        // clip its painting to what it thinks is the window size.
        // On MacOS, Glutin doesn't dispatch a Resized event to our handler
        // above to notify us that the view port has changed. So we need to
        // check on every frame whether the window size has changed.
        let mut resized = false;
        let current_window_size = window.get_inner_size_pixels().unwrap();
        if current_window_size != window_size {
            resized = true;
            window_size = current_window_size;
            let (width, height) = window_size;
            api.set_window_parameters(
                document_id,
                DeviceUintSize::new(width, height),
                DeviceUintRect::new(DeviceUintPoint::new(0, 0),
                                    DeviceUintSize::new(width, height)),
                window.hidpi_factor(),
            );
        }

        if !example.needs_repaint() && !resized {
            continue;
        }

        let (width, height) = window.get_inner_size().unwrap();
        let layout_size = LayoutSize::new(width as f32, height as f32);
        let mut builder = DisplayListBuilder::new(pipeline_id, layout_size);
        let mut resources = ResourceUpdates::new();

        example.render(
            &api,
            &mut builder,
            &mut resources,
            layout_size,
            pipeline_id,
            document_id,
        );
        api.set_display_list(
            document_id,
            epoch,
            Some(root_background_color),
            LayoutSize::new(width as f32, height as f32),
            builder.finalize(),
            true,
            resources,
        );
        api.generate_frame(document_id, None);
        renderer.update();
        let (width, height) = window.get_inner_size_pixels().unwrap();
        renderer.render(DeviceUintSize::new(width, height)).unwrap();
        window.swap_buffers().ok();
        counter += 1;
    }

    renderer.deinit();
    let duration = time::precise_time_s() - start_time;
    let fps = (counter as f64) / duration;
    println!(
        "Rendered {} frames in {} seconds; {} fps",
        counter,
        duration,
        fps
    )
}
