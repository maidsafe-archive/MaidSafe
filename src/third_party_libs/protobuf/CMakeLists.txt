set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(Protobuf)

cmake_minimum_required(VERSION 2.8.10 FATAL_ERROR)

include(../../../cmake_modules/standard_flags.cmake)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX- /W0")
#    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 /wd4244 /wd4267 /wd4018 /wd4355 /wd4800 /wd4251 /wd4146 /wd4305)
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
endif()

include_directories(${PROJECT_SOURCE_DIR}/src)
add_definitions(-D_CONSOLE)

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

add_library(protobuf ${PROTOBUF_DIR})
add_executable(protoc ${PROTOC_DIR})
set_target_properties(protobuf protoc PROPERTIES FOLDER "Third Party/Google Protocol Buffers")

if(MSVC)
  set_target_properties(protobuf PROPERTIES
                        STATIC_LIBRARY_FLAGS_RELEASE "/LTCG"
                        STATIC_LIBRARY_FLAGS_RELWITHDEBINFO "/LTCG")
  set_target_properties(protoc PROPERTIES
                        LINK_FLAGS_RELEASE "/OPT:REF /OPT:ICF /LTCG"
                        LINK_FLAGS_RELWITHDEBINFO "/OPT:REF /OPT:ICF /LTCG /DEBUG"
                        LINK_FLAGS_MINSIZEREL "/LTCG")
endif()

target_link_libraries(protobuf ${JustThread_LIBRARIES})
target_link_libraries(protoc protobuf)
if(UNIX)
  target_link_libraries(protoc -lpthread)
endif()
