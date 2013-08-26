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

#ifndef MAIDSAFE_SUREFILE_SUREFILE_H_
#define MAIDSAFE_SUREFILE_SUREFILE_H_

#include <memory>
#include <map>
#include <string>

#include <boost/spirit/include/qi.hpp>
#include "boost/filesystem/path.hpp"

#include "maidsafe/data_store/surefile_store.h"
#include "maidsafe/passport/detail/secure_string.h"
#ifdef WIN32
#  ifdef HAVE_CBFS
#    include "maidsafe/drive/win_drive.h"
#  else
#    include "maidsafe/drive/dummy_win_drive.h"
#  endif
#else
#  include "maidsafe/drive/unix_drive.h"
#endif
#include "maidsafe/lifestuff/lifestuff.h"


namespace maidsafe {
namespace surefile {

namespace test { class SureFileTest; }

#ifdef WIN32
#  ifdef HAVE_CBFS
template<typename Storage>
struct SureFileDrive {
  typedef drive::detail::CbfsDriveInUserSpace<data_store::SureFileStore> Drive;
};
#  else
typedef drive::DummyWinDriveInUserSpace Drive;
#  endif
#else
template<typename Storage>
struct SureFileDrive {
  typedef drive::FuseDriveInUserSpace<Storage> Drive;
};
#endif

class SureFile {
public:
  typedef passport::detail::Password Password;
  typedef data_store::SureFileStore SureFileStore;
  typedef SureFileDrive<SureFileStore>::Drive Drive;
  typedef std::map<std::string, std::string> Map;
  typedef std::pair<std::string, std::string> Pair;
  typedef lifestuff::ConfigurationErrorFunction ConfigurationErrorFunction;

  // SureFile constructor, refer to discussion in LifeStuff.h for Slots. Throws
  // CommonErrors::uninitialised if any 'slots' member has not been initialised.
  explicit SureFile(const lifestuff::Slots& slots);
  ~SureFile();

  // Creates and/or inserts a string of 'characters' at position 'position' in the input type,
  // password, confirmation password etc., determined by 'input_field', see lifestuff.h for the
  // definition of InputField. Implicitly accepts Unicode characters converted to std::string.
  void InsertInput(uint32_t position, const std::string& characters, lifestuff::InputField input_field);
  // Removes the sequence of characters starting at position 'position' and ending at position
  // 'position' + 'length' from the input type determined by 'input_field'.
  void RemoveInput(uint32_t position, uint32_t length, lifestuff::InputField input_field);
  // Clears the currently inserted characters from the input type determined by 'input_field'.
  void ClearInput(lifestuff::InputField input_field);
  // Compares input types, dependent on 'input_field' value, for equality.
  bool ConfirmInput(lifestuff::InputField input_field);

  // Mounts virtual drive and initialises services if any. Throws on exception.
  void LogIn();
  // Unmounts virtual drive.
  void LogOut();

  void AddService(const std::string& storage_path, const std::string& service_alias);
  void AddServiceFailed(const std::string& service_alias);

  // Returns whether user is logged in or not.
  bool logged_in() const;
  // Root path of mounted virtual drive or empty if unmounted.
  std::string mount_path();

  friend class test::SureFileTest;
 private:
  template<typename Iterator, typename Skipper>
  struct grammer : boost::spirit::qi::grammar<Iterator, Map(), Skipper> {    
    grammer();

    boost::spirit::qi::rule<Iterator, Map(), Skipper> start;
    boost::spirit::qi::rule<Iterator, Pair()> pair;
    boost::spirit::qi::rule<Iterator, std::string()> key, value;
  };

  const lifestuff::Slots& CheckSlots(const lifestuff::Slots& slots);

  void InitialiseService(const std::string& storage_path,
                         const std::string& service_alias,
                         const Identity& service_root_id);

  void FinaliseInput();
  void ResetInput();
  void ResetConfirmationInput();

  void MountDrive(const Identity& drive_root_id);
  void UnmountDrive();

  Map ReadConfigFile();
  void WriteConfigFile(const Map& root_pairs);
  void AddConfigEntry(const std::string& storage_path, const std::string& service_alias);

  void OnServiceAdded(const std::string& service_alias,
                      const Identity& drive_root_id,  
                      const Identity& service_root_id);  
  void OnServiceRemoved(const std::string& service_alias);
  
  void PutCredentials(const boost::filesystem::path& storage_path,
                      const crypto::AES256Key& key,
                      const crypto::AES256InitialisationVector& iv,
                      const Identity& drive_root_id,
                      const Identity& service_root_id);
  void DeleteCredentials(const boost::filesystem::path& storage_path);
  std::pair<Identity, Identity> GetCredentials(const boost::filesystem::path& storage_path,
                                               const crypto::AES256Key& key,
                                               const crypto::AES256InitialisationVector& iv);

  void CheckValid(const std::string& storage_path, const std::string& service_alias);

  NonEmptyString Serialise(const Identity& drive_root_id, const Identity& service_root_id);
  std::pair<Identity, Identity> Parse(const NonEmptyString& serialised_credentials);

  crypto::SecurePassword CreateSecurePassword();
  crypto::AES256Key SecureKey(const crypto::SecurePassword& secure_password);
  crypto::AES256InitialisationVector SecureIv(const crypto::SecurePassword& secure_password);

  const lifestuff::Slots& slots_;
  bool logged_in_;
  std::unique_ptr<Password> password_, confirmation_password_, current_password_;
  boost::filesystem::path mount_path_;
  std::unique_ptr<Drive> drive_;
  std::map<std::string, std::pair<Identity, Identity>> pending_service_additions_;
  std::thread mount_thread_;
  static const boost::filesystem::path kConfigFilePath;
  static const boost::filesystem::path kCredentialsFilename;
};

template<typename Iterator, typename Skipper>
SureFile::grammer<Iterator, Skipper>::grammer()
    : grammer::base_type(start) {
  start = *pair;
  pair  =  key >> '>' >> value;
  key   = +(boost::spirit::qi::char_ - '>');
  value = +(boost::spirit::qi::char_ - ':') >> ':';
}

}  // namespace surefile
}  // namespace maidsafe

#endif  // MAIDSAFE_SUREFILE_SUREFILE_H_
