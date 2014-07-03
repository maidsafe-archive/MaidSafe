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
# 
# # Protoc files
# set(ProtobufSourcesDir "${PROJECT_SOURCE_DIR}/src/google/protobuf")
# file(GLOB_RECURSE ProtocSources "${ProtobufSourcesDir}/compiler/*.cc")
# file(GLOB_RECURSE ProtocTestSources "${ProtobufSourcesDir}/compiler/*test*.cc" "${ProtobufSourcesDir}/compiler/*mock*.cc")
# list(REMOVE_ITEM ProtocSources ${ProtocTestSources})
# 
# file(GLOB_RECURSE ProtocHeaders "${ProtobufSourcesDir}/compiler/*.h")
# file(GLOB_RECURSE ProtocTestHeaders "${ProtobufSourcesDir}/compiler/*test*.h" "${ProtobufSourcesDir}/compiler/*mock*.h")
# list(REMOVE_ITEM ProtocHeaders ${ProtocTestHeaders})
# 
# set(ProtocAllFiles ${ProtocSources} ${ProtocHeaders})
# set(ProtocTestAllFiles ${ProtocTestSources} ${ProtocTestHeaders})
# 
# # Protobuf files
# file(GLOB_RECURSE ProtobufSources "${ProtobufSourcesDir}/*.cc")
# file(GLOB_RECURSE ProtobufTestSources "${ProtobufSourcesDir}/*test*.cc" "${ProtobufSourcesDir}/*mock*.cc" "${ProtobufSourcesDir}/tests/*.cc" "${ProtobufSourcesDir}/testing/*.cc")
# # Only useful if zlib is found and linked, otherwise generates "no symbols" linker warnings
# list(REMOVE_ITEM ProtobufSources ${ProtobufTestSources} ${ProtocSources} ${ProtocTestSources} "${ProtobufSourcesDir}/io/gzip_stream.cc")
# if(NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
#   list(REMOVE_ITEM ProtobufSources "${ProtobufSourcesDir}/stubs/atomicops_internals_x86_gcc.cc")
# endif()
# if(NOT MSVC)
#   list(REMOVE_ITEM ProtobufSources "${ProtobufSourcesDir}/stubs/atomicops_internals_x86_msvc.cc")
# endif()
# list(REMOVE_ITEM ProtobufTestSources ${ProtocTestSources})
# 
# file(GLOB_RECURSE ProtobufHeaders "${ProtobufSourcesDir}/*.h")
# file(GLOB_RECURSE ProtobufTestHeaders "${ProtobufSourcesDir}/*test*.h" "${ProtobufSourcesDir}/*mock*.h" "${ProtobufSourcesDir}/tests/*.h" "${ProtobufSourcesDir}/testing/*.h")
# list(REMOVE_ITEM ProtobufHeaders ${ProtobufTestHeaders} ${ProtocHeaders} ${ProtocTestHeaders} "${ProtobufSourcesDir}/io/gzip_stream.h")
# list(REMOVE_ITEM ProtobufTestHeaders ${ProtocTestHeaders})
# 
# set(ProtobufAllFiles ${ProtobufSources} ${ProtobufHeaders})
# set(ProtobufTestAllFiles ${ProtobufTestSources} ${ProtobufTestHeaders})
# 
# foreach(ProtobufSource ${ProtobufSources})
#   message("ProtobufSource: ${ProtobufSource}")
# endforeach()
# message("")
# foreach(ProtobufHeader ${ProtobufHeaders})
#   message("ProtobufHeader: ${ProtobufHeader}")
# endforeach()
# message("")
# foreach(ProtocSource ${ProtocSources})
#   message("ProtocSource: ${ProtocSource}")
# endforeach()
# message("")
# foreach(ProtocHeader ${ProtocHeaders})
#   message("ProtocHeader: ${ProtocHeader}")
# endforeach()
# message("")
# 
# foreach(ProtobufTestSource ${ProtobufTestSources})
#   message("ProtobufTestSource: ${ProtobufTestSource}")
# endforeach()
# message("")
# foreach(ProtobufTestHeader ${ProtobufTestHeaders})
#   message("ProtobufTestHeader: ${ProtobufTestHeader}")
# endforeach()
# message("")
# foreach(ProtocTestSource ${ProtocTestSources})
#   message("ProtocTestSource: ${ProtocTestSource}")
# endforeach()
# message("")
# foreach(ProtocTestHeader ${ProtocTestHeaders})
#   message("ProtocTestHeader: ${ProtocTestHeader}")
# endforeach()
# message("")



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
)
target_compile_options(protobuf_lite
  PUBLIC
    $<$<BOOL:${MSVC}>:
        $<$<CONFIG:Release>:/O2 /GL /MD>
        $<$<CONFIG:Debug>:/Od /MDd /Zi>
        $<$<CONFIG:MinSizeRel>:/MD>
        $<$<CONFIG:RelWithDebInfo>:/O2 /GL /MD /Zi>
    >
    $<$<BOOL:${UNIX}>:-std=c++11 ${LibCXX}>
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

add_executable(protobuf_lite_tests ${LiteTestAllFiles})
target_compile_options(protobuf_lite_tests PRIVATE $<$<BOOL:${MSVC}>:/W3 /wd4018 /wd4244 /wd4267 /wd4800 /wd4996> $<$<BOOL:${UNIX}>:-w>)
target_link_libraries(protobuf_lite_tests protobuf_lite)

add_executable(protobuf_test_plugin ${TestPluginAllFiles})
target_compile_options(protobuf_test_plugin PRIVATE $<$<BOOL:${MSVC}>:/W3 /wd4018 /wd4244 /wd4267 /wd4800 /wd4996> $<$<BOOL:${UNIX}>:-w>)
target_link_libraries(protobuf_test_plugin proto_compiler)

add_executable(protobuf_tests ${TestsAllFiles})
target_compile_options(protobuf_tests PRIVATE $<$<BOOL:${MSVC}>:/W3 /wd4018 /wd4244 /wd4267 /wd4800 /wd4996> $<$<BOOL:${UNIX}>:-w>)
target_link_libraries(protobuf_tests proto_compiler)

set_target_properties(protobuf_lite protobuf proto_compiler protoc protobuf_lite_tests protobuf_test_plugin protobuf_tests
                      PROPERTIES FOLDER "Third Party/Google Protocol Buffers")

set(AllStaticLibsForCurrentProject protobuf_lite protobuf proto_compiler)
set(AllExesForCurrentProject protoc protobuf_lite_tests protobuf_test_plugin protobuf_tests)
include(../../../cmake_modules/standard_flags.cmake)


#==================================================================================================#
# Package                                                                                          #
#==================================================================================================#
install(TARGETS protobuf_lite protobuf COMPONENT Development CONFIGURATIONS Debug Release ARCHIVE DESTINATION lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/ COMPONENT Development DESTINATION include/maidsafe/third_party_libs)

install(TARGETS protoc COMPONENT Tools CONFIGURATIONS Debug RUNTIME DESTINATION bin/debug)
install(TARGETS protoc COMPONENT Tools CONFIGURATIONS Release RUNTIME DESTINATION bin)
