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

#ifndef MAIDSAFE_SUREFILE_SUREFILE_H_
#define MAIDSAFE_SUREFILE_SUREFILE_H_

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <string>

#ifdef __MSVC__
#  pragma warning(push, 1)
#endif
# include "boost/spirit/include/qi.hpp"
#ifdef __MSVC__
#  pragma warning(pop)
#endif

#include "boost/filesystem/path.hpp"

#include "maidsafe/common/crypto.h"
#include "maidsafe/common/types.h"
#include "maidsafe/data_store/sure_file_store.h"
#include "maidsafe/passport/detail/secure_string.h"
#include "maidsafe/drive/drive_api.h"
#include "maidsafe/drive/drive.h"

#include "maidsafe/surefile/error.h"
#include "maidsafe/surefile/config.h"

namespace maidsafe {
namespace surefile {

namespace test { class SureFileTest; }

class SureFile {
public:
  typedef passport::detail::Password Password;
  typedef data_store::SureFileStore SureFileStore;
  typedef drive::VirtualDrive<SureFileStore>::value_type Drive;
  typedef std::map<std::string, std::string> ServiceMap;
  typedef std::pair<std::string, std::string> ServicePair;

  // SureFile constructor, refer to discussion in LifeStuff.h for Slots. Throws
  // CommonErrors::uninitialised if any 'slots' member has not been initialised.
  explicit SureFile(Slots slots);
  ~SureFile();

  // Creates and/or inserts a string of 'characters' at position 'position' in the input type,
  // password, confirmation password etc., determined by 'input_field', see lifestuff.h for the
  // definition of InputField. Implicitly accepts Unicode characters converted to std::string.
  void InsertInput(uint32_t position, const std::string& characters, InputField input_field);
  // Removes the sequence of characters starting at position 'position' and ending at position
  // 'position' + 'length' from the input type determined by 'input_field'.
  void RemoveInput(uint32_t position, uint32_t length, InputField input_field);

  bool CanCreateUser() const;
  void CreateUser();
  // Mounts virtual drive and initialises services if any. Throws on exception.
  void Login();

  void AddService(const std::string& storage_path, const std::string& service_alias);
  bool RemoveService(const std::string& service_alias);

  // Returns whether user is logged in or not.
  bool logged_in() const;
  // Root path of mounted virtual drive or empty if unmounted.
  std::string mount_path() const;
  ServiceMap service_pairs() const;

  friend class test::SureFileTest;
 private:
  template<typename Iterator>
  struct Grammer : boost::spirit::qi::grammar<Iterator, ServiceMap()> {
    Grammer();

    boost::spirit::qi::rule<Iterator, ServiceMap()> start;
    boost::spirit::qi::rule<Iterator, ServicePair()> pair;
    boost::spirit::qi::rule<Iterator, std::string()> key, value;
  };

  Slots CheckSlots(Slots slots) const;

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
  ServiceMap ReadConfigFile() const;
  void WriteConfigFile(const ServiceMap& service_pairs) const;
  void AddConfigEntry(const std::string& storage_path, const std::string& service_alias) const;

  void OnServiceAdded() const;
  void OnServiceRemoved(const std::string& service_alias) const;
  void OnServiceRenamed(const std::string& old_service_alias, const std::string& new_service_alias) const;

  void PutIds(const boost::filesystem::path& storage_path,
              const Identity& drive_root_id,
              const Identity& service_root_id) const;
  void DeleteIds(const boost::filesystem::path& storage_path) const;
  std::pair<Identity, Identity> GetIds(const boost::filesystem::path& storage_path) const;

  void CheckValid(const std::string& storage_path, const std::string& service_alias) const;
  void CheckDuplicate(const std::string& storage_path, const std::string& service_alias) const;
  bool ValidateContent(const std::string& content) const;

  NonEmptyString Serialise(const Identity& drive_root_id, const Identity& service_root_id) const;
  std::pair<Identity, Identity> Parse(const NonEmptyString& serialised_credentials) const;

  crypto::SecurePassword SecurePassword() const;
  crypto::AES256Key SecureKey(const crypto::SecurePassword& secure_password) const;
  crypto::AES256InitialisationVector SecureIv(const crypto::SecurePassword& secure_password) const;
  std::string EncryptSureFile() const;

  Slots slots_;
  bool logged_in_;
  std::unique_ptr<Password> password_, confirmation_password_;
  boost::filesystem::path mount_path_;
  Identity drive_root_id_;
  std::unique_ptr<Drive> drive_; 
  std::map<std::string, std::pair<Identity, Identity>> pending_service_additions_;
  mutable std::mutex mutex_;
  std::thread mount_thread_;
  bool mount_status_;
  static const std::string kSureFile;
};

template<typename Iterator>
SureFile::Grammer<Iterator>::Grammer()
    : Grammer::base_type(start) {
  start = *pair;
  pair  =  key >> '>' >> value;
  key   = +(boost::spirit::qi::char_ - '>');
  value = +(boost::spirit::qi::char_ - ':') >> ':';
}

}  // namespace surefile
}  // namespace maidsafe

#endif  // MAIDSAFE_SUREFILE_SUREFILE_H_
