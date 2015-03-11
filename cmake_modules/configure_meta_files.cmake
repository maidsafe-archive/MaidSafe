#==================================================================================================#
#                                                                                                  #
#  Copyright 2013 MaidSafe.net limited                                                             #
#                                                                                                  #
#  This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,        #
#  version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which    #
#  licence you accepted on initial access to the Software (the "Licences").                        #
#                                                                                                  #
#  By contributing code to the MaidSafe Software, or to this project generally, you agree to be    #
#  bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root        #
#  directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available   #
#  at: http://www.maidsafe.net/licenses                                                            #
#                                                                                                  #
#  Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed    #
#  under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF   #
#  ANY KIND, either express or implied.                                                            #
#                                                                                                  #
#  See the Licences for the specific language governing permissions and limitations relating to    #
#  use of the MaidSafe Software.                                                                   #
#                                                                                                  #
#==================================================================================================#


file(GLOB_RECURSE MetaFiles "${CMAKE_CURRENT_BINARY_DIR}/*.message_types.meta")
set(HaveCacheable FALSE)
foreach(MetaFile ${MetaFiles})
  # Get file path relative to ${CMAKE_CURRENT_BINARY_DIR}/copied_message_types
  string(REPLACE "${CMAKE_CURRENT_BINARY_DIR}/copied_message_types/" "" FileName "${MetaFile}")

  # Construct warning string
  set(DevWarning "// Modify in <sub-project>/cmake/${FileName} if required, not here.")
  string(LENGTH "${DevWarning}" DevWarningLength)
  if(DevWarningLength GREATER "100")
    set(DevWarning "${DevWarning}     // NOLINT\n")
  else()
    set(DevWarning "${DevWarning}\n")
  endif()

  # Read contents
  file(STRINGS "${MetaFile}" AllTypes)

  set(LineNumber 0)
  foreach(MessageType ${AllTypes})
    math(EXPR LineNumber ${LineNumber}+1)
    # Strip any comment (everything after a '#') and check we've still got some content left
    string(REGEX REPLACE "([^#]*)(#.*)" "\\1" MessageType "${MessageType}")

    if(MessageType)
      # Get the Action type
      string(REGEX REPLACE ".*[Aa][Cc][Tt][Ii][Oo][Nn]:([^ ]*).*" "\\1" Action "${MessageType}")

      # Get the Source Persona and routing Sender types
      string(REGEX REPLACE ".*[Ss][Oo][Uu][Rr][Cc][Ee]:([^ ]*).*" "\\1" Source "${MessageType}")
      string(REGEX MATCH "[^:]+" SourcePersona "${Source}")
      string(REGEX REPLACE "[^:]+:(.*)" "\\1" RoutingSenderType "${Source}")

      # Get the Destination Persona and routing Receiver types
      string(REGEX REPLACE ".*[Dd][Ee][Ss][Tt][Ii][Nn][Aa][Tt][Ii][Oo][Nn]:([^ ]*).*" "\\1"
             Destination "${MessageType}")
      string(REGEX MATCH "[^:]+" DestinationPersona "${Destination}")
      string(REGEX REPLACE "[^:]+:(.*)" "\\1" RoutingReceiverType "${Destination}")

      # Get the Contents field and check the type is 'struct' or 'class'
      string(REGEX REPLACE ".*[Cc][Oo][Nn][Tt][Ee][Nn][Tt][Ss]:([^ ]*).*" "\\1"
             Contents "${MessageType}")
      string(REGEX MATCHALL "[^:]+" ContentsList "${Contents}")
      list(GET ContentsList 0 ClassType)
      if(NOT ClassType STREQUAL "struct" AND NOT ClassType STREQUAL "class")
        set(ErrorMsg "\nError on line ${LineNumber} of <sub-project>/cmake/${FileName}\nContents ")
        set(ErrorMsg "${ErrorMsg}tag is '${ClassType}', but must be either 'struct' or 'class'.")
        message(FATAL_ERROR "${ErrorMsg}")
      endif()
      # Get the Contents class name and namespace.  Construct the fully-qualified class name
      list(GET ContentsList -1 ClassName)
      list(REVERSE ContentsList)
      list(REMOVE_AT ContentsList 0 -1)
      set(Fwd "${ClassType} ${ClassName};")
      set(QualifiedClassName ${ClassName})
      foreach(Namespace ${ContentsList})
        set(Fwd "namespace ${Namespace} { ${Fwd} }")
        set(QualifiedClassName "${Namespace}::${QualifiedClassName}")
      endforeach()

      # Get the Cacheable field if it exists
      string(REGEX MATCH "[Cc][Aa][Cc][Hh][Ee][Aa][Bb][Ll][Ee]" IsCacheable "${MessageType}")

      # Write the forward declaration if required
      list(APPEND AllClasses ${QualifiedClassName})
      list(LENGTH AllClasses AllClassesLengthBefore)
      list(REMOVE_DUPLICATES AllClasses)
      list(LENGTH AllClasses AllClassesLengthAfter)
      if(AllClassesLengthBefore EQUAL AllClassesLengthAfter)
        string(LENGTH "${Fwd}" FwdLength)
        if(FwdLength GREATER "100")
          set(Fwd "${Fwd}  // NOLINT")
        endif()
        list(APPEND Fwds "${Fwd}\n")
      endif()

      # Add Destination Persona and message details to global lists
      list(APPEND DestinationPersonas ${DestinationPersona})
      list(APPEND ${DestinationPersona}ServiceMessages "${Action}:${SourcePersona}")

      # Add Source and Destination Persona and message details to global lists related to caching
      if(IsCacheable)
        list(APPEND CacheableMessages "${Action}:${SourcePersona}:${DestinationPersona}")
        set(HaveCacheable TRUE)
      endif()

      # Write the typedef
      set(Typedef "// Auto-generated typedef.\n")
      list(APPEND Typedef "${DevWarning}")
      list(APPEND Typedef "typedef maidsafe::nfs::MessageWrapper<\n")
      list(APPEND Typedef "    maidsafe::nfs::MessageAction::k${Action},\n")
      list(APPEND Typedef "    maidsafe::nfs::SourcePersona<maidsafe::nfs::Persona::k${SourcePersona}>,\n")
      list(APPEND Typedef "    maidsafe::routing::${RoutingSenderType}Source,\n")
      list(APPEND Typedef "    maidsafe::nfs::DestinationPersona<maidsafe::nfs::Persona::k${DestinationPersona}>,\n")
      list(APPEND Typedef "    maidsafe::routing::${RoutingReceiverType}Id,\n")
      list(APPEND Typedef "    ${QualifiedClassName}>\n")
      list(APPEND Typedef "        ${Action}From${SourcePersona}To${DestinationPersona};\n\n")

      list(APPEND Typedefs "${Typedef}")
    endif()
  endforeach()
endforeach()

list(REMOVE_DUPLICATES DestinationPersonas)
string(REPLACE "\n;" "\n" Typedefs "${Typedefs}")
string(REPLACE "\n;" "\n" Fwds "${Fwds}")


set(Variants "template<typename T>\n")
list(APPEND Variants "inline bool GetVariant(const maidsafe::nfs::TypeErasedMessageWrapper& message, T& variant);\n\n")
foreach(DestinationPersona ${DestinationPersonas})
  unset(Actions)
  foreach(MessageType ${${DestinationPersona}ServiceMessages})
    string(REGEX MATCH "[^:]+" Action "${MessageType}")
    list(APPEND Actions ${Action})
    unset(${Action}Sources)
    unset(AllMessageTypedefs)
  endforeach()
  list(REMOVE_DUPLICATES Actions)
  foreach(MessageType ${${DestinationPersona}ServiceMessages})
    string(REGEX MATCH "[^:]+" Action "${MessageType}")
    string(REGEX REPLACE "[^:]+:(.*)" "\\1" SourcePersona "${MessageType}")
    list(APPEND ${Action}Sources ${SourcePersona})
    list(APPEND AllMessageTypedefs ${Action}From${SourcePersona}To${DestinationPersona})
  endforeach()

  string(REGEX REPLACE ";" ",\n    " AllMessageTypedefs "${AllMessageTypedefs}")
  set(Variant "//==================================================================================================\n")
  list(APPEND Variant "// ${DestinationPersona}\n")
  list(APPEND Variant "//==================================================================================================\n\n")

  list(APPEND Variant "// Auto-generated typedef.\n")
  list(APPEND Variant "// Modify in <sub-project>/cmake/*.message_types.meta if required, not here.\n")
  list(APPEND Variant "typedef boost::variant<\n    ${AllMessageTypedefs}> ${DestinationPersona}ServiceMessages;\n\n")

  list(APPEND Variant "// Auto-generated function.\n")
  list(APPEND Variant "// Modify in <sub-project>/cmake/*.message_types.meta if required, not here.\n")
  list(APPEND Variant "template<>\n")
  list(APPEND Variant "inline bool GetVariant<${DestinationPersona}ServiceMessages>(\n")
  list(APPEND Variant "    const maidsafe::nfs::TypeErasedMessageWrapper& message,\n")
  list(APPEND Variant "    ${Spaces}${DestinationPersona}ServiceMessages& variant) {\n")
  list(APPEND Variant "  auto action(std::get<0>(message));\n")
  list(APPEND Variant "  auto source_persona(std::get<1>(message).data);\n")
  list(APPEND Variant "  auto destination_persona(std::get<2>(message).data);\n")
  list(APPEND Variant "  auto message_id(std::get<3>(message).data);\n")
  list(APPEND Variant "  LOG(kVerbose) << \"GetVariant<${DestinationPersona}ServiceMessages>: \" << source_persona << \" \" << action\n")
  list(APPEND Variant "                << \" to \" << destination_persona << \", id: \" << message_id;\n")
  list(APPEND Variant "  if (destination_persona != maidsafe::nfs::Persona::k${DestinationPersona}) {\n")
  list(APPEND Variant "    LOG(kError) << \"Wrong destination persona type: \" << destination_persona;\n")
  list(APPEND Variant "    BOOST_THROW_EXCEPTION(maidsafe::MakeError(maidsafe::CommonErrors::invalid_parameter));\n")
  list(APPEND Variant "  }\n")
  list(APPEND Variant "  switch (action) {\n")
  foreach(Action ${Actions})
    list(APPEND Variant "    case maidsafe::nfs::MessageAction::k${Action}:\n")
    list(APPEND Variant "      switch (source_persona) {\n")
    foreach(SourcePersona ${${Action}Sources})
      list(APPEND Variant "        case maidsafe::nfs::Persona::k${SourcePersona}:\n")
      list(APPEND Variant "          variant = ${DestinationPersona}ServiceMessages(\n")
      list(APPEND Variant "              ${Action}From${SourcePersona}To${DestinationPersona}(message));\n")
      list(APPEND Variant "          return true;\n")
    endforeach()
    list(APPEND Variant "        default:\n")
    list(APPEND Variant "          break;\n")
    list(APPEND Variant "      }\n")
    list(APPEND Variant "      break;\n")
  endforeach()
  list(APPEND Variant "    default:\n")
  list(APPEND Variant "      break;\n")
  list(APPEND Variant "  }\n")
  list(APPEND Variant "  return false;\n")
  list(APPEND Variant "}\n\n\n")

  list(APPEND Variants "${Variant}")
endforeach()


# Handle cacheable messages
macro(get_action_source_and_destination MessageTypeIn)
  string(REGEX MATCH "[^:]+" Action "${MessageTypeIn}")
  string(REGEX REPLACE "[^:]+:([^:]+):.*" "\\1" SourcePersona "${MessageTypeIn}")
  string(REGEX REPLACE "[^:]+:[^:]+:(.*)" "\\1" DestinationPersona "${MessageTypeIn}")
endmacro()

if(HaveCacheable)
  list(APPEND Variants "//==================================================================================================\n")
  list(APPEND Variants "// CacheHandler\n")
  list(APPEND Variants "//==================================================================================================\n\n")

  unset(Actions)
  foreach(MessageType ${CacheableMessages})
    get_action_source_and_destination("${MessageType}")
    list(APPEND Actions ${Action})
    unset(${Action}Sources)
    unset(AllMessageTypedefs)
  endforeach()
  list(REMOVE_DUPLICATES Actions)

  foreach(MessageType ${CacheableMessages})
    get_action_source_and_destination("${MessageType}")
     list(APPEND ${Action}Sources ${SourcePersona})
  endforeach()
  foreach(Action ${Actions})
    list(REMOVE_DUPLICATES ${Action}Sources)
    foreach(SourcePersona ${${Action}Sources})
      unset(${Action}${SourcePersona})
    endforeach()
  endforeach()

  foreach(MessageType ${CacheableMessages})
    get_action_source_and_destination("${MessageType}")
    list(APPEND ${Action}${SourcePersona} ${DestinationPersona})
    list(APPEND AllMessageTypedefs ${Action}From${SourcePersona}To${DestinationPersona})
  endforeach()
  list(REMOVE_DUPLICATES AllMessageTypedefs)
  foreach(Action ${Actions})
    foreach(SourcePersona ${${Action}Sources})
      list(REMOVE_DUPLICATES ${Action}${SourcePersona})
    endforeach()
  endforeach()

  string(REGEX REPLACE ";" ",\n    " AllMessageTypedefs "${AllMessageTypedefs}")

  set(Variant "// Auto-generated typedef.\n")
  list(APPEND Variant "// Modify in <sub-project>/cmake/*.message_types.meta if required, not here.\n")
  list(APPEND Variant "typedef boost::variant<\n    ${AllMessageTypedefs}> CacheableMessages;\n\n")
   list(APPEND Variant "// Auto-generated function.\n")
  list(APPEND Variant "// Modify in <sub-project>/cmake/*.message_types.meta if required, not here.\n")
  list(APPEND Variant "template<>\n")
  list(APPEND Variant "inline bool GetVariant<CacheableMessages>(\n")
  list(APPEND Variant "    const maidsafe::nfs::TypeErasedMessageWrapper& message,\n")
  list(APPEND Variant "    ${Spaces}CacheableMessages& cacheable_variant) {\n")
  list(APPEND Variant "  auto action(std::get<0>(message));\n")
  list(APPEND Variant "  auto source_persona(std::get<1>(message).data);\n")
  list(APPEND Variant "  auto destination_persona(std::get<2>(message).data);\n")
  list(APPEND Variant "  switch (action) {\n")
  foreach(Action ${Actions})
    list(APPEND Variant "    case maidsafe::nfs::MessageAction::k${Action}:\n")
    list(APPEND Variant "      switch (source_persona) {\n")
    foreach(SourcePersona ${${Action}Sources})
      list(APPEND Variant "        case maidsafe::nfs::Persona::k${SourcePersona}:\n")
      list(APPEND Variant "          switch (destination_persona) {\n")
      foreach(DestinationPersona ${${Action}${SourcePersona}})
        list(APPEND Variant "            case maidsafe::nfs::Persona::k${DestinationPersona}:\n")
        list(APPEND Variant "              cacheable_variant = CacheableMessages(\n")
        list(APPEND Variant "                  ${Action}From${SourcePersona}To${DestinationPersona}(message));\n")
        list(APPEND Variant "              return true;\n")
      endforeach()
      list(APPEND Variant "            default:\n")
      list(APPEND Variant "              break;\n")
      list(APPEND Variant "          }\n")
      list(APPEND Variant "          break;\n")
    endforeach()
    list(APPEND Variant "        default:\n")
    list(APPEND Variant "          break;\n")
    list(APPEND Variant "      }\n")
    list(APPEND Variant "      break;\n")
  endforeach()
  list(APPEND Variant "    default:\n")
  list(APPEND Variant "      break;\n")
  list(APPEND Variant "  }\n")
  list(APPEND Variant "  return false;\n")
  list(APPEND Variant "}\n\n")

  list(APPEND Variants "${Variant}")
endif()

string(REPLACE "\n;" "\n" Variants "${Variants}")
configure_file(${InputFile} ${OutputFile} @ONLY)
