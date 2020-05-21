// Copyright 2017 Google LLC
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

#ifndef SHAKA_EMBEDDED_MEDIA_CLEARKEY_IMPLEMENTATION_FACTORY_H_
#define SHAKA_EMBEDDED_MEDIA_CLEARKEY_IMPLEMENTATION_FACTORY_H_

#include <memory>
#include <string>
#include <vector>

#include "shaka/eme/implementation_factory.h"

namespace shaka {
namespace eme {

class ClearKeyImplementationFactory final : public ImplementationFactory {
 public:
  static constexpr const char* kKeySystemName = "org.w3.clearkey";

  ClearKeyImplementationFactory() {}

  bool SupportsSessionType(MediaKeySessionType type) const override;

  bool SupportsInitDataType(MediaKeyInitDataType type) const override;

  bool SupportsAudioRobustness(const std::string& robustness) const override;

  bool SupportsVideoRobustness(const std::string& robustness) const override;

  MediaKeysRequirement DistinctiveIdentifier() const override;

  MediaKeysRequirement PersistentState() const override;

  std::shared_ptr<Implementation> CreateImplementation(
      ImplementationHelper* helper,
      MediaKeysRequirement distinctive_identifier,
      MediaKeysRequirement persistent_state,
      const std::vector<std::string>& audio_robustness,
      const std::vector<std::string>& video_robustness) override;
};

}  // namespace eme
}  // namespace shaka

#endif  // SHAKA_EMBEDDED_EME_CLEARKEY_IMPLEMENTATION_FACTORY_H_
