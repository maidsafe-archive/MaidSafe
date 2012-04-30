set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project(Protobuf)
cmake_minimum_required(VERSION 2.6)

if(WIN32)
  if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 /wd4244 /wd4267 /wd4018 /wd4355 /wd4800 /wd4251 /wd4996 /wd4146 /wd4305)
  ENDIF(  )
else()
  add_definitions(-Wno-deprecated)
  add_definitions(-DHAVE_PTHREAD)
endif()

include_directories(${PROJECT_SOURCE_DIR}/src)
add_definitions(-D_CONSOLE -std=c++11 -Wambiguous-member-template -Wbind-to-temporary-copy -Weverything )

# library
FILE(GLOB_RECURSE LIB_PROTBUF_SOURCES "${PROJECT_SOURCE_DIR}/src/google/protobuf/*.cc")
FILE(GLOB_RECURSE LIB_PROTBUF_HEADERS "${PROJECT_SOURCE_DIR}/src/google/protobuf/*.h")
SET(PROTOBUF_DIR ${LIB_PROTBUF_SOURCES} ${LIB_PROTBUF_HEADERS})
# compiler
FILE(GLOB_RECURSE LIB_PROTOC_SOURCES "${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/*.cc")
FILE(GLOB_RECURSE LIB_PROTOC_HEADERS "${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/*.h")
SET(PROTOC_DIR ${LIB_PROTOC_SOURCES} ${LIB_PROTOC_HEADERS})
#tests
FILE(GLOB_RECURSE PROTOC_TESTS_SOURCES "${PROJECT_SOURCE_DIR}/src/google/protobuf/tests/*.cc")
FILE(GLOB_RECURSE PROTOC_TESTS_HEADERS "${PROJECT_SOURCE_DIR}/src/google/protobuf/tests/*.h")
SET(TESTS ${PROTOC_TESTS_SOURCES} ${PROTOC_TESTS_HEADERS})

LIST(REMOVE_ITEM PROTOBUF_DIR ${PROTOC_DIR} ${TESTS})

add_library("libprotobuf" ${PROTOBUF_DIR})
add_executable(protoc ${PROTOC_DIR})

if( UNIX )
  target_link_libraries(protoc -lpthread )
endif()

target_link_libraries(protoc libprotobuf)
