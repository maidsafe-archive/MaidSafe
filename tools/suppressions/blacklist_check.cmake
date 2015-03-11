file(MD5 "${BlacklistFile}" Hash)
if(NOT Hash STREQUAL BlacklistFileHash)
  message(FATAL_ERROR "\nPlease rerun CMake since ${BlacklistFile} has been modified.\n\n")
endif()
