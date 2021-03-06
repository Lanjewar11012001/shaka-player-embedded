// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "shaka/media/default_media_player.h"

#include <vector>

#include "shaka/media/text_track.h"
#include "src/debug/mutex.h"
#ifdef OS_IOS
#  include "src/media/ios/av_media_player.h"
#endif
#include "src/media/mse_media_player.h"

namespace shaka {
namespace media {

class DefaultMediaPlayer::Impl {
 public:
  Impl(ClientList* clients, VideoRenderer* video_renderer,
       AudioRenderer* audio_renderer)
      : mutex("DefaultMediaPlayer"),
#ifdef OS_IOS
        av_player(clients),
        playing_src_(false),
#endif
        mse_player(clients, video_renderer, audio_renderer) {
  }

  Mutex mutex;
#ifdef OS_IOS
  ios::AvMediaPlayer av_player;
  bool playing_src_;
#endif
  MseMediaPlayer mse_player;
  std::vector<std::shared_ptr<TextTrack>> text_tracks_;
};

DefaultMediaPlayer::DefaultMediaPlayer(VideoRenderer* video_renderer,
                                       AudioRenderer* audio_renderer)
    : impl_(new Impl(GetClientList(), video_renderer, audio_renderer)) {}
DefaultMediaPlayer::~DefaultMediaPlayer() {}

void DefaultMediaPlayer::SetDecoders(Decoder* video_decoder,
                                     Decoder* audio_decoder) {
  impl_->mse_player.SetDecoders(video_decoder, audio_decoder);
}

const void* DefaultMediaPlayer::GetIosView() {
#ifdef OS_IOS
  return impl_->av_player.GetIosView();
#else
  return nullptr;
#endif
}

const void* DefaultMediaPlayer::GetAvPlayer() {
#ifdef OS_IOS
  return impl_->av_player.GetAvPlayer();
#else
  return nullptr;
#endif
}

MediaCapabilitiesInfo DefaultMediaPlayer::DecodingInfo(
    const MediaDecodingConfiguration& config) const {
  if (config.type == MediaDecodingType::File) {
#ifdef OS_IOS
    return impl_->av_player.DecodingInfo(config);
#else
    return MediaCapabilitiesInfo();
#endif
  } else {
    return impl_->mse_player.DecodingInfo(config);
  }
}

std::vector<std::shared_ptr<MediaTrack>> DefaultMediaPlayer::AudioTracks() {
  std::unique_lock<Mutex> lock(impl_->mutex);
#ifdef OS_IOS
  if (impl_->playing_src_)
    return impl_->av_player.AudioTracks();
#endif
  return {};
}

std::vector<std::shared_ptr<const MediaTrack>> DefaultMediaPlayer::AudioTracks()
    const {
  std::unique_lock<Mutex> lock(impl_->mutex);
#ifdef OS_IOS
  if (impl_->playing_src_) {
    return const_cast<const ios::AvMediaPlayer&>(impl_->av_player)
        .AudioTracks();
  }
#endif
  return {};
}

std::vector<std::shared_ptr<MediaTrack>> DefaultMediaPlayer::VideoTracks() {
  std::unique_lock<Mutex> lock(impl_->mutex);
#ifdef OS_IOS
  if (impl_->playing_src_)
    return impl_->av_player.VideoTracks();
#endif
  return {};
}

std::vector<std::shared_ptr<const MediaTrack>> DefaultMediaPlayer::VideoTracks()
    const {
  std::unique_lock<Mutex> lock(impl_->mutex);
#ifdef OS_IOS
  if (impl_->playing_src_) {
    return const_cast<const ios::AvMediaPlayer&>(impl_->av_player)
        .VideoTracks();
  }
#endif
  return {};
}

std::vector<std::shared_ptr<TextTrack>> DefaultMediaPlayer::TextTracks() {
  std::unique_lock<Mutex> lock(impl_->mutex);
#ifdef OS_IOS
  if (impl_->playing_src_)
    return impl_->av_player.TextTracks();
#endif
  return impl_->text_tracks_;
}

std::vector<std::shared_ptr<const TextTrack>> DefaultMediaPlayer::TextTracks()
    const {
  std::unique_lock<Mutex> lock(impl_->mutex);
#ifdef OS_IOS
  if (impl_->playing_src_) {
    return const_cast<const ios::AvMediaPlayer&>(impl_->av_player).TextTracks();
  }
#endif
  return {impl_->text_tracks_.begin(), impl_->text_tracks_.end()};
}

std::shared_ptr<TextTrack> DefaultMediaPlayer::AddTextTrack(
    TextTrackKind kind, const std::string& label, const std::string& language) {
  std::shared_ptr<TextTrack> ret;
  {
    std::unique_lock<Mutex> lock(impl_->mutex);
    ret.reset(new TextTrack(kind, label, language, ""));
    impl_->text_tracks_.emplace_back(ret);
  }
  GetClientList()->OnAddTextTrack(ret);
  return ret;
}

void DefaultMediaPlayer::Detach() {
  ProxyMediaPlayer::Detach();
#ifdef OS_IOS
  impl_->playing_src_ = true;
#endif
}


MediaPlayer* DefaultMediaPlayer::CreateMse() {
  if (!impl_->mse_player.AttachMse())
    return nullptr;
  return &impl_->mse_player;
}
MediaPlayer* DefaultMediaPlayer::CreateSource(const std::string& src) {
#ifdef OS_IOS
  if (!impl_->av_player.AttachSource(src))
    return nullptr;
  impl_->playing_src_ = true;
  return &impl_->av_player;
#else
  return nullptr;
#endif
}

}  // namespace media
}  // namespace shaka
