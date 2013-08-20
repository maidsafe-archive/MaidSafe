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

#ifndef MAIDSAFE_SUREFILE_SUREFILE_IMPL_H_
#define MAIDSAFE_SUREFILE_SUREFILE_IMPL_H_

#include "boost/filesystem/path.hpp"

#include "maidsafe/data_store/surefile_store.h"
#include "maidsafe/passport/detail/secure_string.h"
#include "maidsafe/lifestuff/lifestuff.h"

//#include "maidsafe/lifestuff/detail/client_maid.h"
//#include "maidsafe/lifestuff/detail/client_mpid.h"

namespace maidsafe {
namespace surefile {

class SureFileImpl {
 public:
  typedef passport::detail::Password Password;
  typedef passport::Maid Maid;
  typedef data_store::SureFileStore SureFileStore;

  explicit SureFileImpl(const lifestuff::Slots& slots);
  ~SureFileImpl();

  void InsertUserInput(uint32_t position, const std::string& characters, lifestuff::InputField input_field);
  void RemoveUserInput(uint32_t position, uint32_t length, lifestuff::InputField input_field);
  void ClearUserInput(lifestuff::InputField input_field);
  bool ConfirmUserInput(lifestuff::InputField input_field);

  void CreateUser();
  void LogIn();
  void LogOut();

  void ChangePassword();

  bool logged_in() const;

  boost::filesystem::path mount_path();

 private:
  void FinaliseUserInput();
  void ResetInput();
  void ResetConfirmationInput();

  bool logged_in_;
  std::unique_ptr<Password> password_, confirmation_password_, current_password_;
  std::unique_ptr<SureFileStore> storage_;
  std::unique_ptr<Maid> maid_;
  std::unique_ptr<Identity> unique_user_id_;
};

}  // namespace surefile
}  // namespace maidsafe

#endif  // MAIDSAFE_SUREFILE_SUREFILE_IMPL_H_
