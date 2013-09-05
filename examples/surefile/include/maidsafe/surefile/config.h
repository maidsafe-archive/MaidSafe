/* Copyright 2013 MaidSafe.net limited

This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or later,
and The General Public License (GPL), version 3. By contributing code to this project You agree to
the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in the root directory
of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available at:

http://www.novinet.com/license

Unless required by applicable law or agreed to in writing, software distributed under the License is
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing permissions and limitations under the
License.
*/

#ifndef MAIDSAFE_SUREFILE_CONFIG_H_
#define MAIDSAFE_SUREFILE_CONFIG_H_

#include <memory>
#include <functional>
#include <string>

namespace maidsafe {
namespace surefile {

// Type passed to input functions in SureFile class to determine which variable(s) to process.
enum InputField { kPassword, kConfirmationPassword };

// Config file parsing error.
typedef std::function<void()> ConfigurationErrorFunction;
// Inform UI of request for the addition of a new directory at the root of the drive.
typedef std::function<void()> OnServiceAddedFunction;

// Slots are used to provide useful information back to the client application.
struct Slots {
  ConfigurationErrorFunction configuration_error;
  OnServiceAddedFunction on_service_added;
};

const char kCharRegex[] = ".*";

}  // namespace surefile
}  // namespace maidsafe

#endif  // MAIDSAFE_SUREFILE_CONFIG_H_
