set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(Protobuf)

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
target_include_directories(protobuf SYSTEM PUBLIC ${PROJECT_SOURCE_DIR}/src)

add_executable(protoc ${PROTOC_DIR})
set_target_properties(protobuf protoc PROPERTIES FOLDER "Third Party/Google Protocol Buffers")

set(AllStaticLibsForCurrentProject protobuf)
set(AllExesForCurrentProject protoc)
include(../../../cmake_modules/standard_flags.cmake)

target_compile_definitions(protobuf
  PUBLIC
    $<$<AND:$<BOOL:${JUST_THREAD_DEADLOCK_CHECK}>,$<CONFIG:Debug>>:_JUST_THREAD_DEADLOCK_CHECK>
    $<$<BOOL:${UNIX}>:HAVE_PTHREAD>
)

target_compile_options(protobuf
  PUBLIC
    $<$<BOOL:${MSVC}>:
        $<$<CONFIG:Release>:/O2 /GL /MD>
        $<$<CONFIG:Debug>:/Od /MDd /Zi>
        $<$<CONFIG:MinSizeRel>:/MD>
        $<$<CONFIG:RelWithDebInfo>:/O2 /GL /MD /Zi>
    >
    $<$<BOOL:${UNIX}>:-std=c++11 ${LibCpp}>
  PRIVATE
    $<$<BOOL:${MSVC}>:/W3 /wd4244 /wd4267 /wd4800 /wd4996>
    $<$<BOOL:${UNIX}>:-w>
)

target_compile_definitions(protoc
  PUBLIC
    $<$<AND:$<BOOL:${JUST_THREAD_DEADLOCK_CHECK}>,$<CONFIG:Debug>>:_JUST_THREAD_DEADLOCK_CHECK>
)

target_compile_options(protoc
  PRIVATE
    $<$<BOOL:${MSVC}>:/W3 /wd4244 /wd4267 /wd4800 /wd4996>
    $<$<BOOL:${UNIX}>:-w>
)

target_link_libraries(protobuf ${JustThread_LIBRARIES})
target_link_libraries(protoc protobuf)
if(UNIX)
  target_link_libraries(protoc -lpthread)
endif()
