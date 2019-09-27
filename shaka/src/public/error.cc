// Copyright 2016 Google LLC
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

#include "shaka/error.h"

#include "src/util/macros.h"

namespace shaka {

class Error::Impl {};

Error::Error(const std::string& message)
    : message(message), severity(0), category(0), code(0) {}

Error::Error(int severity, int category, int code, const std::string& message)
    : message(message), severity(severity), category(category), code(code) {}

Error::Error(const Error& other)
    : message(other.message),
      severity(other.severity),
      category(other.category),
      code(other.code) {}

Error::Error(Error&&) = default;

Error::~Error() {}

Error& Error::operator=(const Error& other) {
  message = other.message;
  severity = other.severity;
  category = other.category;
  code = other.code;

  return *this;
}

Error& Error::operator=(Error&&) = default;

}  // namespace shaka
