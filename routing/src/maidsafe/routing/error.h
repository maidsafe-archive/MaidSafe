/*******************************************************************************
 *  Copyright 2012 maidsafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 ******************************************************************************/

#ifndef MAIDSAFE_ROUTING_ERROR_H_
#define MAIDSAFE_ROUTING_ERROR_H_

#include <system_error>
#include "maidsafe/common/platform_config.h"
namespace maidsafe {

namespace routing {

namespace error {

enum error_codes {
  // General
  Ok = 10,
  timed_out = 100,
  // RoutingTable
  own_id_not_includable = 200,
  failed_to_insert_new_contact = 300,
  failed_to_find_contact = 400,
  failed_to_set_publicKey = 500,
  failed_to_update_rank = 600,
  failed_to_set_preferred_endpoint = 700,
  // Node
  no_online_bootstrap_contacts = 800,
  invalid_bootstrap_contacts = 900,
  not_joined = 1000,
  cannot_write_config = 1100
};

enum error_conditions {
  routing_table_error = 100,
  node_error = 200,
  file_error = 300,
  network_error = 400
};

std::error_code make_error_code(error_codes e);
std::error_condition make_error_condition(error_conditions e);

} // namespace error // until gcc supports enum classes we need this!!

class error_category_routing : public std::error_category {
 public:
    virtual std::string message(int ev) const;
#ifdef MAIDSAFE_WIN32
    virtual const char* name() const;  // msvc 11 does not support noexcept
    virtual std::error_condition default_error_condition(int ev) const;
    virtual bool equivalent(const std::error_code& code, int condition) const;
#else
    virtual const char* name() const noexcept (true);  // gcc > 4.7 requires noexcept
    virtual std::error_condition default_error_condition(int ev) const noexcept (true);
    virtual bool equivalent(const std::error_code& code, int condition) const noexcept (true);
#endif
};


const std::error_category &error_category();

}  // namespace routing

} // namespace maidsafe

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Weffc++"
#endif
namespace std
{
  template <>
  struct std::is_error_code_enum<maidsafe::routing::error::error_codes>
    : public true_type {};

  template <>
  struct std::is_error_condition_enum<maidsafe::routing::error::error_conditions>
      : public true_type {};
}
#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif

#endif  // MAIDSAFE_ROUTING_ERROR_H_

