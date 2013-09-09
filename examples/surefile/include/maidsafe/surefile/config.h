/*  Copyright 2013 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.novinet.com/license

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

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
// Inform UI of deletion of a directory at the root of the drive.
typedef std::function<void(const std::string& /*service_alias*/)> OnServiceRemovedFunction;
// Inform UI of rename of a directory at the root of the drive.
typedef std::function<void(const std::string& /*old_service_alias*/,
                           const std::string& /*new_service_alias*/)> OnServiceRenamedFunction;

// Slots are used to provide useful information back to the client application.
struct Slots {
  ConfigurationErrorFunction configuration_error;
  OnServiceAddedFunction on_service_added;
  OnServiceRemovedFunction on_service_removed;
  OnServiceRenamedFunction on_service_renamed;
};

const char kCharRegex[] = ".*";

}  // namespace surefile
}  // namespace maidsafe

#endif  // MAIDSAFE_SUREFILE_CONFIG_H_
