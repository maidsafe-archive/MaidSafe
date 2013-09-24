set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(Protobuf)

include(../../../cmake_modules/standard_flags.cmake)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4005 /wd4018 /wd4100 /wd4125 /wd4127 /wd4244 /wd4267 /wd4389 /wd4510 /wd4610 /wd4701 /wd4702 /wd4703 /wd4800")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
endif()

include_directories(${PROJECT_SOURCE_DIR}/src)
add_definitions(-D_CONSOLE)

file(GLOB_RECURSE LIB_PROTBUF_SOURCES "${PROJECT_SOURCE_DIR}/src/google/protobuf/*.cc")
file(GLOB_RECURSE LIB_PROTBUF_HEADERS "${PROJECT_SOURCE_DIR}/src/google/protobuf/*.h")
set(PROTOBUF_DIR ${LIB_PROTBUF_SOURCES} ${LIB_PROTBUF_HEADERS})

file(GLOB_RECURSE LIB_PROTOC_SOURCES "${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/*.cc")
file(GLOB_RECURSE LIB_PROTOC_HEADERS "${PROJECT_SOURCE_DIR}/src/google/protobuf/compiler/*.h")
set(PROTOC_DIR ${LIB_PROTOC_SOURCES} ${LIB_PROTOC_HEADERS})

file(GLOB_RECURSE PROTOC_TESTS_SOURCES "${PROJECT_SOURCE_DIR}/src/google/protobuf/tests/*.cc")
file(GLOB_RECURSE PROTOC_TESTS_HEADERS "${PROJECT_SOURCE_DIR}/src/google/protobuf/tests/*.h")
set(TESTS ${PROTOC_TESTS_SOURCES} ${PROTOC_TESTS_HEADERS})

list(REMOVE_ITEM PROTOBUF_DIR ${PROTOC_DIR} ${TESTS})

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