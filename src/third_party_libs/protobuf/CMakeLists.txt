set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
# protoc segfaults if compiled with ubsan
set(NO_UBSAN TRUE)
# ned 2014-03-21: The above line works for protobuf, but has no effect on protoc which
#                 still gains the -fsanitize=undefined. I've wasted enough time on this
#                 so I'm pressing ahead with the below hack
if(HAVE_UNDEFINED_BEHAVIOR_SANITIZER)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-sanitize=undefined")
endif()

set(CMAKE_DEBUG_POSTFIX -d)
set(CMAKE_RELWITHDEBINFO_POSTFIX -rwdi)
set(CMAKE_MINSIZEREL_POSTFIX -msr)
set(CMAKE_RELEASENOINLINE_POSTFIX -rni)

project(Protobuf)

include(files.cmake)


#==================================================================================================#
# Define libraries and protoc executable                                                           #
#==================================================================================================#
add_library(protobuf_lite STATIC ${ProtobufLiteAllFiles})
ms_target_include_system_dirs(protobuf_lite PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_compile_definitions(protobuf_lite
  PUBLIC
    HAVE_HASH_MAP=1 HAVE_HASH_SET=1
    HASH_NAMESPACE=std
    HASH_MAP_H=<unordered_map> HASH_SET_H=<unordered_set>
    HASH_MAP_CLASS=unordered_map HASH_SET_CLASS=unordered_set
    HAVE_ZLIB=0
    $<$<AND:$<BOOL:${JUST_THREAD_DEADLOCK_CHECK}>,$<CONFIG:Debug>>:_JUST_THREAD_DEADLOCK_CHECK>
    $<$<BOOL:${MSVC}>:NOMINMAX>
  PRIVATE
    $<$<BOOL:${UNIX}>:HAVE_PTHREAD>
    $<$<STREQUAL:"${CMAKE_BUILD_TYPE}","Tsan">:THREAD_SANITIZER>
)
target_compile_options(protobuf_lite
  PUBLIC
    $<$<BOOL:${MSVC}>:
        $<$<CONFIG:Release>:/O2 /GL /MD>
        $<$<CONFIG:Debug>:/Od /MDd /Zi>
        $<$<CONFIG:MinSizeRel>:/MD>
        $<$<CONFIG:RelWithDebInfo>:/O2 /GL /MD /Zi>
    >
    $<$<BOOL:${UNIX}>:-std=c++11 -fPIC ${LibCXX}>
  PRIVATE
    $<$<BOOL:${MSVC}>:/W3 /wd4018 /wd4244 /wd4267 /wd4800 /wd4996>
    $<$<BOOL:${UNIX}>:-w>
)
target_link_libraries(protobuf_lite ${JustThread_LIBRARIES})
if(TARGET check_sanitizer_blacklist)
  add_dependencies(protobuf_lite check_sanitizer_blacklist)
endif()

add_library(protobuf STATIC ${ProtobufAllFiles})
target_compile_options(protobuf PRIVATE $<$<BOOL:${MSVC}>:/W3 /wd4018 /wd4244 /wd4267 /wd4800 /wd4996> $<$<BOOL:${UNIX}>:-w>)
target_link_libraries(protobuf protobuf_lite)

add_library(proto_compiler ${ProtocAllFiles})
target_include_directories(proto_compiler PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_compile_options(proto_compiler PRIVATE $<$<BOOL:${MSVC}>:/W3 /wd4018 /wd4146 /wd4244 /wd4267 /wd4800 /wd4996> $<$<BOOL:${UNIX}>:-w>)
target_link_libraries(proto_compiler protobuf)

add_executable(protoc ${ProtocMain})
target_compile_options(protoc PRIVATE $<$<BOOL:${MSVC}>:/W3 /wd4018 /wd4244 /wd4267 /wd4800 /wd4996> $<$<BOOL:${UNIX}>:-w>)
target_link_libraries(protoc proto_compiler protobuf_lite)
if(UNIX)
  target_link_libraries(protoc -lpthread)
endif()


#==================================================================================================#
# Define test executables                                                                          #
#==================================================================================================#
set(GeneratedProtoRootDir ${CMAKE_BINARY_DIR}/GeneratedProtoFiles)
add_executable(protobuf_lite_tests ${LiteTestAllFiles})
target_include_directories(protobuf_lite_tests PRIVATE ${GeneratedProtoRootDir})
target_compile_options(protobuf_lite_tests PRIVATE $<$<BOOL:${MSVC}>:/W3 /wd4018 /wd4244 /wd4267 /wd4800 /wd4996> $<$<BOOL:${UNIX}>:-w>)
target_link_libraries(protobuf_lite_tests protobuf_lite)

add_executable(protobuf_test_plugin ${TestPluginAllFiles})
target_compile_options(protobuf_test_plugin PRIVATE $<$<BOOL:${MSVC}>:/W3 /wd4018 /wd4100 /wd4127 /wd4244 /wd4245 /wd4267 /wd4800 /wd4996> $<$<BOOL:${UNIX}>:-w>)
target_link_libraries(protobuf_test_plugin proto_compiler gtest)
add_custom_command(TARGET protobuf_test_plugin PRE_BUILD
                   COMMAND ${CMAKE_COMMAND}
                   -DINPUT_FILE="${ProtobufSourcesDir}/compiler/command_line_interface_unittest.cc.in"
                   -DOUTPUT_FILE="${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/compiler/command_line_interface_unittest.cc"
                   -DPLUGIN_PATH="$<TARGET_FILE:protobuf_test_plugin>" -P "${PROJECT_SOURCE_DIR}/configure_cli_unittest.cmake")

add_executable(protobuf_tests ${TestsAllFiles})
target_include_directories(protobuf_tests PRIVATE ${GeneratedProtoRootDir} ${BoostSourceDir})
target_compile_options(protobuf_tests PRIVATE $<$<BOOL:${MSVC}>:/W3 /wd4018 /wd4100 /wd4125 /wd4127 /wd4146 /wd4244 /wd4267 /wd4305 /wd4307 /wd4800 /wd4996> $<$<BOOL:${UNIX}>:-w>)
target_compile_definitions(protobuf_tests PRIVATE $<$<BOOL:${MSVC}>:BOOST_ALL_NO_LIB>)
target_link_libraries(protobuf_tests proto_compiler gtest_main gtest_no_maidsafe_log ${BoostFilesystemLibs} ${BoostSystemLibs})
add_dependencies(protobuf_tests protobuf_test_plugin)

set_target_properties(protobuf_lite protobuf proto_compiler protoc protobuf_lite_tests protobuf_test_plugin protobuf_tests
                      PROPERTIES FOLDER "Third Party/Google Protocol Buffers")

set(AllStaticLibsForCurrentProject protobuf_lite protobuf proto_compiler)
set(AllExesForCurrentProject protoc protobuf_lite_tests protobuf_test_plugin protobuf_tests)
include(../../../cmake_modules/standard_flags.cmake)


#==================================================================================================#
# Set up protoc custom commands for tests                                                          #
#==================================================================================================#
set(ProtocArgs "--proto_path=${PROJECT_SOURCE_DIR}/src" "--cpp_out=${GeneratedProtoRootDir}")
if(MSVC)
  list(APPEND ProtocArgs "--error_format=msvs")
else()
  list(APPEND ProtocArgs "--error_format=gcc")
endif()

# Add custom command to generate CC and header files
file(MAKE_DIRECTORY ${GeneratedProtoRootDir})
foreach(ProtoFile ${LiteTestProtoFiles} ${TestsProtoFiles})
  get_filename_component(ProtoFileNameWe ${ProtoFile} NAME_WE)
  string(REPLACE "${PROJECT_SOURCE_DIR}/src" "${GeneratedProtoRootDir}" GeneratedFile "${ProtoFile}")
  string(REPLACE ".proto" ".pb.cc" GeneratedSource "${GeneratedFile}")
  string(REPLACE ".proto" ".pb.h" GeneratedHeader "${GeneratedFile}")
  add_custom_command(OUTPUT ${GeneratedSource} ${GeneratedHeader}
                     COMMAND $<TARGET_FILE:protoc> ${ProtocArgs} ${ProtoFile}
                     DEPENDS protoc ${ProtoFile}
                     COMMENT "Generated files from ${ProtoFileNameWe}.proto"
                     VERBATIM)
endforeach()


#==================================================================================================#
# Package                                                                                          #
#==================================================================================================#
install(TARGETS protobuf_lite protobuf COMPONENT Development CONFIGURATIONS Debug Release ARCHIVE DESTINATION lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/ COMPONENT Development DESTINATION include/maidsafe/third_party_libs)

install(TARGETS protoc COMPONENT Tools CONFIGURATIONS Debug RUNTIME DESTINATION bin/debug)
install(TARGETS protoc COMPONENT Tools CONFIGURATIONS Release RUNTIME DESTINATION bin)
