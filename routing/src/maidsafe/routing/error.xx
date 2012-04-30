#include "maidsafe/routing/error.h"
// ************************************

namespace maidsafe {

namespace routing {

namespace error {

std::error_code make_error_code(error_codes e) {
  return std::error_code(static_cast<int>(e), error_category());
}

std::error_condition make_error_condition(error_conditions e) {
  return std::error_condition(static_cast<int>(e), error_category());
}


} // namespace error // until gcc supports strongly typed enums we need this!!

#ifdef MAIDSAFE_WIN32
const char* error_category_routing::name() const {
#else
const char* error_category_routing::name() const noexcept (true){
#endif 
  return "routing";
}

std::string error_category_routing::message(int ev) const {
  switch (ev)
  {
  case error::timed_out :
    return "Timed out";
  case error::own_id_not_includable :
    return "Own node not includable";
  case error::failed_to_insert_new_contact :
    return "Cannot insert new contact";
  case error::failed_to_find_contact :
    return "Cannot find contact";
  case error::failed_to_set_publicKey :
    return "cannot set public key";
  case error::failed_to_update_rank :
    return "cannot update rank";
  case error::failed_to_set_preferred_endpoint :
    return "Cannot set desired endpoint";
  case error::no_online_bootstrap_contacts :
    return "No contactable bootstrap contacts";
  case error::invalid_bootstrap_contacts :
    return "Invalid bootstrap contacts";
  case error::not_joined :
    return "Not joined";
  case error::cannot_write_config:
    return "bad config file";
  default :
    return "Unknown routing error";
  }
}
#ifdef MAIDSAFE_WIN32
std::error_condition error_category_routing::default_error_condition(int ev) const {
#else
std::error_condition error_category_routing::default_error_condition(int ev) const noexcept (true) {
#endif
  switch (ev)
    {
      case error::timed_out:
      case error::not_joined:
        return error::network_error;
      case error::failed_to_find_contact:
      case error::failed_to_insert_new_contact:
      case error::failed_to_set_publicKey:
      case error::failed_to_update_rank:
        return error::routing_table_error;
      case error::own_id_not_includable:
      case error::invalid_bootstrap_contacts:
        return error::node_error;
      case error::cannot_write_config:
        return error::file_error;
      default:
        return std::error_condition(ev, *this);
    }
}
// TODO(dirvine) we can implement this later on I think if we want to catch and
// collate all system errors as per this example
// bool error_category_routing::equivalent(
//     const std::error_code& code,
//     int condition) const {
//   switch (condition) {
//     case error::network_error:
//     return code == std::errc::not_enough_memory
//       || code == std::errc::resource_unavailable_try_again
//       || code == std::errc::too_many_files_open
//       || code == std::errc::too_many_files_open_in_system;
//     case error::file_error:
//     return code == std::errc::no_such_file_or_directory;
//   default:
//     return false;
//   }
// }

const std::error_category& error_category() {
  static error_category_routing instance;
  return instance;
}


}  // namespace routing

} // namespace maidsafe
