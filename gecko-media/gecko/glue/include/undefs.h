
#undef MOZ_GECKO_PROFILER
#undef MOZ_MEMORY
#undef MOZ_GLUE_IN_PROGRAM

// This depends on the media/audioipc "prototype".
// TODO: import media/audioipc?
#undef MOZ_CUBEB_REMOTING

#undef MOZ_WIDGET_ANDROID
#undef MOZ_AV1

#undef MOZ_WEBRTC

#if defined(ANDROID)
// valloc isn't declared in any header of the generated ndk-r15c arm (version 21) toolchain.
#undef HAVE_VALLOC
#endif

#define ReentrancyGuard GeckoMedia_ReentrancyGuard
#define TimeStamp GeckoMedia_TimeStamp
#define HashBytes GeckoMedia_HashBytes
#define BaseTimeDurationPlatformUtils GeckoMedia_BaseTimeDurationPlatformUtils
#define Unused GeckoMedia_Unused
#define detail GeckoMedia_detail

#define moz_malloc_size_of GeckoMedia_moz_malloc_size_of
#define moz_malloc_usable_size GeckoMedia_moz_malloc_usable_size
#define moz_posix_memalign GeckoMedia_moz_posix_memalign
#define moz_xcalloc GeckoMedia_moz_xcalloc
#define moz_xmalloc GeckoMedia_moz_xmalloc
#define moz_xmemalign GeckoMedia_moz_xmemalign
#define moz_xposix_memalign GeckoMedia_moz_xposix_memalign
#define moz_xrealloc GeckoMedia_moz_xrealloc
#define moz_xstrdup GeckoMedia_moz_xstrdup
#define moz_xstrndup GeckoMedia_moz_xstrndup
#define moz_xvalloc GeckoMedia_moz_xvalloc
#define mozalloc_abort GeckoMedia_mozalloc_abort
#define mozalloc_handle_oom GeckoMedia_mozalloc_handle_oom
#define mozalloc_set_oom_abort_handler GeckoMedia_mozalloc_set_oom_abort_handler

#define MacIOSurface GeckoMedia_MacIOSurface
#define MacIOSurfaceLib GeckoMedia_MacIOSurfaceLib
#define GetContextType GeckoMedia_GetContextType

#define mozPoisonValueInit GeckoMedia_mozPoisonValueInit
#define gMozillaPoisonSize GeckoMedia_gMozillaPoisonSize
#define gMozillaPoisonBase GeckoMedia_gMozillaPoisonBase
#define gMozillaPoisonValue GeckoMedia_gMozillaPoisonValue
