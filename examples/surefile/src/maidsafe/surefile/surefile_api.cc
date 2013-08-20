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

#include "maidsafe/surefile/surefile_api.h"

#include "maidsafe/surefile/surefile_impl.h"

namespace maidsafe {
namespace surefile {

  
SureFile::SureFile(const lifestuff::Slots& slots)
  : surefile_impl_(new SureFileImpl(slots)) {}

SureFile::~SureFile() {}

void SureFile::InsertUserInput(uint32_t position, const std::string& characters, lifestuff::InputField input_field) {
  return surefile_impl_->InsertUserInput(position, characters, input_field);
}

void SureFile::RemoveUserInput(uint32_t position, uint32_t length, lifestuff::InputField input_field) {
  return surefile_impl_->RemoveUserInput(position, length, input_field);
}

void SureFile::ClearUserInput(lifestuff::InputField input_field) {
  return surefile_impl_->ClearUserInput(input_field);
}

bool SureFile::ConfirmUserInput(lifestuff::InputField input_field) {
  return surefile_impl_->ConfirmUserInput(input_field);
}

void SureFile::CreateUser() {
  return surefile_impl_->CreateUser();
}

void SureFile::LogIn() {
  return surefile_impl_->LogIn();
}

void SureFile::ChangePassword() {
  return surefile_impl_->ChangePassword();
}

bool SureFile::logged_in() const {
  return surefile_impl_->logged_in();
}

std::string SureFile::mount_path() {
  return surefile_impl_->mount_path().string();
}

}  // namespace surefile
}  // namespace maidsafe
