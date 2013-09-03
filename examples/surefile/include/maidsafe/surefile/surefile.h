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

#ifdef __MSVC__
#  pragma warning(push, 1)
#endif
# include <boost/spirit/include/qi.hpp>
#ifdef __MSVC__
#  pragma warning(pop)
#endif

#include "boost/filesystem/path.hpp"

#include "maidsafe/data_store/sure_file_store.h"
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

#include "maidsafe/surefile/error.h"

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
  typedef drive::detail::FuseDriveInUserSpace<Storage> Drive;
};
#endif

class SureFile {
public:
  typedef passport::detail::Password Password;
  typedef data_store::SureFileStore SureFileStore;
  typedef SureFileDrive<SureFileStore>::Drive Drive;
  typedef std::map<std::string, std::string> ServiceMap;
  typedef std::pair<std::string, std::string> ServicePair;
  typedef lifestuff::ConfigurationErrorFunction ConfigurationErrorFunction;

  // SureFile constructor, refer to discussion in LifeStuff.h for Slots. Throws
  // CommonErrors::uninitialised if any 'slots' member has not been initialised.
  explicit SureFile(lifestuff::Slots slots);
  ~SureFile();

  // Creates and/or inserts a string of 'characters' at position 'position' in the input type,
  // password, confirmation password etc., determined by 'input_field', see lifestuff.h for the
  // definition of InputField. Implicitly accepts Unicode characters converted to std::string.
  void InsertInput(uint32_t position, const std::string& characters, lifestuff::InputField input_field);
  // Removes the sequence of characters starting at position 'position' and ending at position
  // 'position' + 'length' from the input type determined by 'input_field'.
  void RemoveInput(uint32_t position, uint32_t length, lifestuff::InputField input_field);

  bool CanCreateUser();
  void CreateUser();
  // Mounts virtual drive and initialises services if any. Throws on exception.
  void Login();

  void AddService(const std::string& storage_path, const std::string& service_alias);
  void AddServiceFailed(const std::string& service_alias);

  // Returns whether user is logged in or not.
  bool logged_in() const;
  // Root path of mounted virtual drive or empty if unmounted.
  std::string mount_path();

  friend class test::SureFileTest;
 private:
  template<typename Iterator>
  struct grammer : boost::spirit::qi::grammar<Iterator, ServiceMap()> {
    grammer();

    boost::spirit::qi::rule<Iterator, ServiceMap()> start;
    boost::spirit::qi::rule<Iterator, ServicePair()> pair;
    boost::spirit::qi::rule<Iterator, std::string()> key, value;
  };

  lifestuff::Slots CheckSlots(lifestuff::Slots slots);

  void InitialiseService(const std::string& storage_path,
                         const std::string& service_alias,
                         const Identity& service_root_id);

  void FinaliseInput(bool login);
  void ClearInput();
  void ConfirmInput();
  void ResetPassword();
  void ResetConfirmationPassword();

  void MountDrive(const Identity& drive_root_id);
  void UnmountDrive();
#ifdef MAIDSAFE_WIN32
  std::string GetMountPath() const;
#endif
  ServiceMap ReadConfigFile();
  void WriteConfigFile(const ServiceMap& service_pairs);
  void AddConfigEntry(const std::string& storage_path, const std::string& service_alias);

  void OnServiceAdded(const std::string& service_alias,
                      const Identity& drive_root_id,  
                      const Identity& service_root_id);  
  void OnServiceRemoved(const std::string& service_alias);
  void OnServiceRenamed(const std::string& old_service_alias,
                        const std::string& new_service_alias);

  void PutIds(const boost::filesystem::path& storage_path,
              const Identity& drive_root_id,
              const Identity& service_root_id);
  void DeleteIds(const boost::filesystem::path& storage_path);
  std::pair<Identity, Identity> GetIds(const boost::filesystem::path& storage_path);

  void CheckValid(const std::string& storage_path, const std::string& service_alias);
  void ValidateContent(const std::string& content);

  NonEmptyString Serialise(const Identity& drive_root_id, const Identity& service_root_id);
  std::pair<Identity, Identity> Parse(const NonEmptyString& serialised_credentials);

  crypto::SecurePassword SecurePassword();
  crypto::AES256Key SecureKey(const crypto::SecurePassword& secure_password);
  crypto::AES256InitialisationVector SecureIv(const crypto::SecurePassword& secure_password);
  std::string EncryptSureFile();

  lifestuff::Slots slots_;
  bool logged_in_;
  std::unique_ptr<Password> password_, confirmation_password_;
  boost::filesystem::path mount_path_;
  std::unique_ptr<Drive> drive_;
  std::map<std::string, std::pair<Identity, Identity>> pending_service_additions_;
  mutable std::mutex mutex_;
  std::thread mount_thread_;
  bool mount_status_;
  static const std::string kSureFile;
  static const boost::filesystem::path kUserAppPath;
};

template<typename Iterator>
SureFile::grammer<Iterator>::grammer()
    : grammer::base_type(start) {
  start = *pair;
  pair  =  key >> '>' >> value;
  key   = +(boost::spirit::qi::char_ - '>');
  value = +(boost::spirit::qi::char_ - ':') >> ':';
}

}  // namespace surefile
}  // namespace maidsafe

#endif  // MAIDSAFE_SUREFILE_SUREFILE_H_
