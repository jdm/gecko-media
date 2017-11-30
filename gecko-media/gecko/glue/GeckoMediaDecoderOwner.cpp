/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "GeckoMediaDecoderOwner.h"
#include "mozilla/AbstractThread.h"
#include "VideoFrameContainer.h"
#include "ImageContainer.h"

namespace mozilla {

GeckoMediaDecoderOwner::GeckoMediaDecoderOwner(PlayerCallbackObject aCallback)
  : mCallback(aCallback)
{
}

GeckoMediaDecoderOwner::GeckoMediaDecoderOwner()
{

}

void
GeckoMediaDecoderOwner::DownloadProgressed()
{
}

void
GeckoMediaDecoderOwner::DispatchAsyncEvent(const nsAString& aName)
{
  nsAutoCString dst;
  CopyUTF16toUTF8(aName, dst);
  if (dst.EqualsLiteral("durationchange")) {
    if (mCallback.mContext && mCallback.mDurationChanged) {
      (*mCallback.mDurationChanged)(mCallback.mContext,
                                    mDecoder->GetDuration());
      return;
    }
  }
  if (mCallback.mContext && mCallback.mAsyncEvent) {
    (*mCallback.mAsyncEvent)(mCallback.mContext, dst.get());
  }
};

void
GeckoMediaDecoderOwner::UpdateReadyState()
{
}

void
GeckoMediaDecoderOwner::FireTimeUpdate(bool aPeriodic)
{
  if (mCallback.mContext && mCallback.mTimeUpdate) {
    double time = mDecoder->GetCurrentTime();
    (*mCallback.mTimeUpdate)(mCallback.mContext, time);
  }
}

bool
GeckoMediaDecoderOwner::GetPaused()
{
  return mDecoder && mDecoder->IsOwnerPaused();
}

void
GeckoMediaDecoderOwner::MetadataLoaded(const MediaInfo* aInfo,
                                       UniquePtr<const MetadataTags> aTags)
{
  // FIXME: serialize aInfo and aTags somehow to callback.
  if (mCallback.mContext && mCallback.mMetadataLoaded) {
    GeckoMediaMetadata metadata = { 0, 0, 0 };
    if (aInfo->HasVideo()) {
      metadata.mVideoWidth = aInfo->mVideo.mDisplay.width;
      metadata.mVideoHeight = aInfo->mVideo.mDisplay.height;
    }
    metadata.mDuration = mDecoder->GetDuration();
    (*mCallback.mMetadataLoaded)(mCallback.mContext, metadata);
  }
}

void
GeckoMediaDecoderOwner::FirstFrameLoaded()
{
  if (mCallback.mContext && mCallback.mLoadedData) {
    (*mCallback.mLoadedData)(mCallback.mContext);
  }
}

void
GeckoMediaDecoderOwner::NetworkError(const MediaResult& aError)
{
}

void
GeckoMediaDecoderOwner::DecodeError(const MediaResult& aError)
{
  mHasError = true;
  if (mCallback.mContext && mCallback.mDecodeError) {
    (*mCallback.mDecodeError)(mCallback.mContext);
  }
}

void
GeckoMediaDecoderOwner::DecodeWarning(const MediaResult& aError)
{
}

bool
GeckoMediaDecoderOwner::HasError() const
{
  return mHasError;
}

void
GeckoMediaDecoderOwner::LoadAborted()
{
}

void
GeckoMediaDecoderOwner::PlaybackEnded()
{
  if (mCallback.mContext && mCallback.mPlaybackEnded) {
    (*mCallback.mPlaybackEnded)(mCallback.mContext);
  }
}

void
GeckoMediaDecoderOwner::SeekStarted()
{
  if (mCallback.mContext && mCallback.mSeekStarted) {
    (*mCallback.mSeekStarted)(mCallback.mContext);
  }
}

void
GeckoMediaDecoderOwner::SeekCompleted()
{
  if (mCallback.mContext && mCallback.mSeekCompleted) {
    (*mCallback.mSeekCompleted)(mCallback.mContext);
  }
}

void
GeckoMediaDecoderOwner::DownloadSuspended()
{
}

void
GeckoMediaDecoderOwner::NotifySuspendedByCache(bool aSuspendedByCache)
{
}

void
GeckoMediaDecoderOwner::NotifyDecoderPrincipalChanged()
{
}

void
GeckoMediaDecoderOwner::SetAudibleState(bool aAudible)
{
}

void
GeckoMediaDecoderOwner::NotifyXPCOMShutdown()
{
}

void
GeckoMediaDecoderOwner::DispatchEncrypted(const nsTArray<uint8_t>& aInitData,
                                          const nsAString& aInitDataType)
{
}

void
GeckoMediaDecoderOwner::ConstructMediaTracks(const MediaInfo* aInfo)
{
}

void
GeckoMediaDecoderOwner::RemoveMediaTracks()
{
}

void
GeckoMediaDecoderOwner::AsyncResolveSeekDOMPromiseIfExists()
{
}

void
GeckoMediaDecoderOwner::AsyncRejectSeekDOMPromiseIfExists()
{
}

void
GeckoMediaDecoderOwner::NotifyWaitingForKey()
{
}

AbstractThread*
GeckoMediaDecoderOwner::AbstractMainThread() const
{
  return AbstractThread::MainThread();
}

dom::HTMLMediaElement*
GeckoMediaDecoderOwner::GetMediaElement()
{
  return nullptr;
}

VideoFrameContainer*
GeckoMediaDecoderOwner::GetVideoFrameContainer()
{
  RefPtr<layers::ImageContainer> container =
    new layers::ImageContainer(this);
  mVideoFrameContainer =
    new VideoFrameContainer(this, container.forget());
  return mVideoFrameContainer;
}

already_AddRefed<GMPCrashHelper>
GeckoMediaDecoderOwner::CreateGMPCrashHelper()
{
  return nullptr;
}

void
GeckoMediaDecoderOwner::Invalidate(bool aImageSizeChanged,
                                   Maybe<nsIntSize>& aNewIntrinsicSize,
                                   bool aForceInvalidate)
{
}

void
GeckoMediaDecoderOwner::PrincipalHandleChangedForVideoFrameContainer(
  VideoFrameContainer* aContainer,
  const PrincipalHandle& aNewPrincipalHandle)
{
}

void
GeckoMediaDecoderOwner::SetDecoder(GeckoMediaDecoder* aDecoder)
{
  mDecoder = aDecoder;
}

void
GeckoMediaDecoderOwner::UpdateCurrentImages(nsTArray<GeckoPlanarYCbCrImage> aImages)
{
  if (mCallback.mContext && mCallback.mUpdateCurrentImages) {
    (*mCallback.mUpdateCurrentImages)(mCallback.mContext, aImages.Length(), aImages.Elements());
  }
}

void
GeckoMediaDecoderOwner::NotifyBuffered() const
{
  if (mCallback.mContext && mCallback.mNotifyBuffered) {
    auto buffered = mDecoder->GetBuffered();
    size_t size = buffered.Length();
    GeckoMediaTimeInterval* ranges = new GeckoMediaTimeInterval[size];
    size_t i = 0;
    for (auto interval : buffered) {
      ranges[i].mStart = interval.mStart.ToSeconds();
      ranges[i].mEnd = interval.mEnd.ToSeconds();
      i++;
    }
    (*mCallback.mNotifyBuffered)(mCallback.mContext, size, ranges);
    delete [] ranges;
  }
}

void
GeckoMediaDecoderOwner::NotifySeekable() const
{
  if (mCallback.mContext && mCallback.mNotifySeekable) {
    auto seekable = mDecoder->GetSeekable();
    size_t size = seekable.Length();
    GeckoMediaTimeInterval* ranges = new GeckoMediaTimeInterval[size];
    size_t i = 0;
    for (auto interval : seekable) {
      ranges[i].mStart = interval.mStart.ToSeconds();
      ranges[i].mEnd = interval.mEnd.ToSeconds();
      i++;
    }
    (*mCallback.mNotifySeekable)(mCallback.mContext, size, ranges);
    delete [] ranges;
  }
}

void
GeckoMediaDecoderOwner::Shutdown()
{
  if (mVideoFrameContainer) {
    // The ImageContainer keeps a list of the images that it sends out to Rust,
    // so that if we shutdown, we can deallocate and neuter the images
    // proactively. If we don't do this, we can end up with crashes if Rust
    // code on another thread tries to use images after we've shutdown.
    auto imageContainer = mVideoFrameContainer->GetImageContainer();
    if (imageContainer) {
      imageContainer->DeallocateExportedImages();
    }
    mVideoFrameContainer->ForgetElement();
    mVideoFrameContainer = nullptr;
  }
  mDecoder = nullptr;
  if (mCallback.mContext && mCallback.mFree) {
    (*mCallback.mFree)(mCallback.mContext);
  }
  mCallback = {0};
}

} // namespace mozilla
