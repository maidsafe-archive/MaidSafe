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

#ifndef MAIDSAFE_SUREFILE_SUREFILE_API_H_
#define MAIDSAFE_SUREFILE_SUREFILE_API_H_

#include <memory>

#include "maidsafe/lifestuff/lifestuff.h"

namespace maidsafe {
namespace surefile {

class SureFile {
 public:
  // SureFile constructor, refer to discussion in LifeStuff.h for Slots. Throws
  // CommonErrors::uninitialised if any 'slots' member has not been initialised.
  explicit SureFile(const lifestuff::Slots& slots);
  ~SureFile();

  // Creates and/or inserts a string of 'characters' at position 'position' in the input type,
  // password, confirmation password etc., determined by 'input_field', see lifestuff.h for the
  // definition of InputField. Implicitly accepts Unicode characters converted to std::string.
  void InsertUserInput(uint32_t position, const std::string& characters, lifestuff::InputField input_field);
  // Removes the sequence of characters starting at position 'position' and ending at position
  // 'position' + 'length' from the input type determined by 'input_field'.
  void RemoveUserInput(uint32_t position, uint32_t length, lifestuff::InputField input_field);
  // Clears the currently inserted characters from the input type determined by 'input_field'.
  void ClearUserInput(lifestuff::InputField input_field);
  // Compares input types, dependent on 'input_field' value, for equality.
  bool ConfirmUserInput(lifestuff::InputField input_field);

  // Creates new user credentials, derived from password, that are subsequently retrieved during
  // login. If an exception is thrown during the call, attempts cleanup then rethrows the exception.
  void CreateUser();
  // Recovers session details subject to validation from input password. If an exception is thrown
  // during the call, attempts cleanup then rethrows the exception.
  void LogIn();

  // The following method can be used to change a user's credentials.
  void ChangePassword();

  // Returns whether user is logged in or not.
  bool logged_in() const;
  // Root path of mounted virtual drive or empty if unmounted.
  std::string mount_path();

 private:
  std::unique_ptr<SureFileImpl> surefile_impl_;
};

}  // namespace surefile
}  // namespace maidsafe

#endif  // MAIDSAFE_SUREFILE_SUREFILE_API_H_
