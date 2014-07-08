#==================================================================================================#
#                                                                                                  #
#  Copyright (c) 2012 MaidSafe.net limited                                                         #
#                                                                                                  #
#  The following source code is property of MaidSafe.net limited and is not meant for external     #
#  use.  The use of this code is governed by the license file licence.txt found in the root        #
#  directory of this project and also on www.maidsafe.net.                                         #
#                                                                                                  #
#  You are not free to copy, amend or otherwise use this source code without the explicit written  #
#  permission of the board of directors of MaidSafe.net.                                           #
#                                                                                                  #
#==================================================================================================#


project(sqlite)

include(${CMAKE_SOURCE_DIR}/cmake_modules/standard_setup.cmake)

add_library(sqlite STATIC ${PROJECT_SOURCE_DIR}/include/sqlite3.h ${PROJECT_SOURCE_DIR}/src/sqlite3.c)
#target_compile_options(sqlite PRIVATE $<$<BOOL:${MSVC}>:/wd4244>)
ms_target_include_system_dirs(sqlite PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(sqlite PRIVATE SQLITE_OMIT_LOAD_EXTENSION)

if(MSVC)
  target_link_libraries(sqlite ${JustThread_LIBRARIES})
else()
  target_link_libraries(sqlite ${JustThread_LIBRARIES} -lpthread)
endif()

set(AllStaticLibsForCurrentProject sqlite)

ms_add_executable(sqlite_test "." ${PROJECT_SOURCE_DIR}/src/sqlite_test.cc)
ms_add_executable(speedtest1 "." ${PROJECT_SOURCE_DIR}/src/speedtest1.cc)
ms_add_executable(wordcount "." ${PROJECT_SOURCE_DIR}/src/wordcount.c)

target_link_libraries(sqlite_test sqlite maidsafe_test ${BoostFilesystemLibs})
target_link_libraries(speedtest1 sqlite maidsafe_test ${BoostFilesystemLibs})
target_link_libraries(wordcount sqlite maidsafe_test ${BoostFilesystemLibs})

if(MSVC)
  set_target_properties(sqlite sqlite_test speedtest1 wordcount PROPERTIES FOLDER "Third Party/SQLite")
  set(AllExesForCurrentProject sqlite_test speedtest1 wordcount)
else()
  ms_add_executable(crashtest1 "." ${PROJECT_SOURCE_DIR}/src/crashtest1.c)
  ms_add_executable(threadtest1 "." ${PROJECT_SOURCE_DIR}/src/threadtest1.c)
  ms_add_executable(threadtest2 "." ${PROJECT_SOURCE_DIR}/src/threadtest2.c)
  ms_add_executable(threadtest3 "." ${PROJECT_SOURCE_DIR}/src/threadtest3.c)

  target_link_libraries(crashtest1 sqlite maidsafe_test ${BoostFilesystemLibs})
  target_link_libraries(threadtest1 sqlite maidsafe_test ${BoostFilesystemLibs})
  target_link_libraries(threadtest2 sqlite maidsafe_test ${BoostFilesystemLibs})
  target_link_libraries(threadtest3 sqlite maidsafe_test ${BoostFilesystemLibs})

  target_include_directories(threadtest3 ${PROJECT_SOURCE_DIR}/src/)

  set_target_properties(sqlite sqlite_test crashtest1 speedtest1 threadtest1 threadtest2 threadtest3 wordcount PROPERTIES FOLDER "Third Party/SQLite")
  set(AllExesForCurrentProject sqlite_test speedtest1 crashtest1 speedtest1 threadtest1 threadtest2 threadtest3 wordcount)
endif()

add_definitions(-DSQLITE_ENABLE_RTREE)

include(../../../cmake_modules/standard_flags.cmake)

install(TARGETS sqlite COMPONENT Development CONFIGURATIONS Debug Release ARCHIVE DESTINATION lib)
install(FILES ${PROJECT_SOURCE_DIR}/include/sqlite.h COMPONENT Development DESTINATION include/maidsafe/third_party_libs/sqlite)
