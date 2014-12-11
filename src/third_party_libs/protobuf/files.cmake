set(ProtobufSourcesDir "${PROJECT_SOURCE_DIR}/src/google/protobuf")
set(ProtobufLiteSources
    "${ProtobufSourcesDir}/extension_set.cc"
    "${ProtobufSourcesDir}/generated_message_util.cc"
    "${ProtobufSourcesDir}/io/coded_stream.cc"
    "${ProtobufSourcesDir}/io/zero_copy_stream.cc"
    "${ProtobufSourcesDir}/io/zero_copy_stream_impl_lite.cc"
    "${ProtobufSourcesDir}/message_lite.cc"
    "${ProtobufSourcesDir}/repeated_field.cc"
    "${ProtobufSourcesDir}/stubs/common.cc"
    "${ProtobufSourcesDir}/stubs/once.cc"
    "${ProtobufSourcesDir}/stubs/stringprintf.cc"
    "${ProtobufSourcesDir}/wire_format_lite.cc"
    )
if(UNIX AND NOT APPLE AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
  list(APPEND ProtobufLiteSources "${ProtobufSourcesDir}/stubs/atomicops_internals_x86_gcc.cc")
elseif(MSVC)
  list(APPEND ProtobufLiteSources "${ProtobufSourcesDir}/stubs/atomicops_internals_x86_msvc.cc")
endif()
set(ProtobufLiteHeaders
    "${ProtobufSourcesDir}/extension_set.h"
    "${ProtobufSourcesDir}/generated_message_util.h"
    "${ProtobufSourcesDir}/io/coded_stream.h"
    "${ProtobufSourcesDir}/io/coded_stream_inl.h"
    "${ProtobufSourcesDir}/io/zero_copy_stream.h"
    "${ProtobufSourcesDir}/io/zero_copy_stream_impl_lite.h"
    "${ProtobufSourcesDir}/message_lite.h"
    "${ProtobufSourcesDir}/repeated_field.h"
    "${ProtobufSourcesDir}/stubs/atomicops.h"
    "${ProtobufSourcesDir}/stubs/atomicops_internals_arm_gcc.h"
    "${ProtobufSourcesDir}/stubs/atomicops_internals_arm_qnx.h"
    "${ProtobufSourcesDir}/stubs/atomicops_internals_atomicword_compat.h"
    "${ProtobufSourcesDir}/stubs/atomicops_internals_macosx.h"
    "${ProtobufSourcesDir}/stubs/atomicops_internals_mips_gcc.h"
    "${ProtobufSourcesDir}/stubs/atomicops_internals_pnacl.h"
    "${ProtobufSourcesDir}/stubs/atomicops_internals_tsan.h"
    "${ProtobufSourcesDir}/stubs/atomicops_internals_x86_gcc.h"
    "${ProtobufSourcesDir}/stubs/atomicops_internals_x86_msvc.h"
    "${ProtobufSourcesDir}/stubs/common.h"
    "${ProtobufSourcesDir}/stubs/hash.h"
    "${ProtobufSourcesDir}/stubs/map-util.h"
    "${ProtobufSourcesDir}/stubs/once.h"
    "${ProtobufSourcesDir}/stubs/platform_macros.h"
    "${ProtobufSourcesDir}/stubs/stl_util.h"
    "${ProtobufSourcesDir}/stubs/stringprintf.h"
    "${ProtobufSourcesDir}/stubs/template_util.h"
    "${ProtobufSourcesDir}/stubs/type_traits.h"
    "${ProtobufSourcesDir}/wire_format_lite.h"
    "${ProtobufSourcesDir}/wire_format_lite_inl.h"
    )
set(ProtobufLiteAllFiles ${ProtobufLiteSources} ${ProtobufLiteHeaders})


set(ProtobufSources
    "${ProtobufSourcesDir}/compiler/importer.cc"
    "${ProtobufSourcesDir}/compiler/parser.cc"
    "${ProtobufSourcesDir}/descriptor.cc"
    "${ProtobufSourcesDir}/descriptor.pb.cc"
    "${ProtobufSourcesDir}/descriptor_database.cc"
    "${ProtobufSourcesDir}/dynamic_message.cc"
    "${ProtobufSourcesDir}/extension_set_heavy.cc"
    "${ProtobufSourcesDir}/generated_message_reflection.cc"
#    "${ProtobufSourcesDir}/io/gzip_stream.cc"  # Only useful if zlib is found and linked, otherwise generates "no symbols" linker warnings
    "${ProtobufSourcesDir}/io/printer.cc"
    "${ProtobufSourcesDir}/io/tokenizer.cc"
    "${ProtobufSourcesDir}/io/zero_copy_stream_impl.cc"
    "${ProtobufSourcesDir}/message.cc"
    "${ProtobufSourcesDir}/reflection_ops.cc"
    "${ProtobufSourcesDir}/service.cc"
    "${ProtobufSourcesDir}/stubs/structurally_valid.cc"
    "${ProtobufSourcesDir}/stubs/strutil.cc"
    "${ProtobufSourcesDir}/stubs/substitute.cc"
    "${ProtobufSourcesDir}/text_format.cc"
    "${ProtobufSourcesDir}/unknown_field_set.cc"
    "${ProtobufSourcesDir}/wire_format.cc"
#    ${ProtobufLiteSources}  # Rather than making libprotobuf compile all lite files, make it depend on libprotobuf_lite
    )
set(ProtobufHeaders
    "${ProtobufSourcesDir}/compiler/importer.h"
    "${ProtobufSourcesDir}/compiler/parser.h"
    "${ProtobufSourcesDir}/descriptor.h"
    "${ProtobufSourcesDir}/descriptor.pb.h"
    "${ProtobufSourcesDir}/descriptor_database.h"
    "${ProtobufSourcesDir}/dynamic_message.h"
    "${ProtobufSourcesDir}/generated_message_reflection.h"
#    "${ProtobufSourcesDir}/io/gzip_stream.h"  # Only useful if zlib is found and linked, otherwise generates "no symbols" linker warnings
    "${ProtobufSourcesDir}/io/printer.h"
    "${ProtobufSourcesDir}/io/tokenizer.h"
    "${ProtobufSourcesDir}/io/zero_copy_stream_impl.h"
    "${ProtobufSourcesDir}/message.h"
    "${ProtobufSourcesDir}/reflection_ops.h"
    "${ProtobufSourcesDir}/service.h"
    "${ProtobufSourcesDir}/stubs/strutil.h"
    "${ProtobufSourcesDir}/stubs/substitute.h"
    "${ProtobufSourcesDir}/text_format.h"
    "${ProtobufSourcesDir}/unknown_field_set.h"
    "${ProtobufSourcesDir}/wire_format.h"
#    ${ProtobufLiteHeaders}  # Rather than making libprotobuf compile all lite files, make it depend on libprotobuf_lite
    )
set(ProtobufAllFiles ${ProtobufSources} ${ProtobufHeaders})


set(ProtocSources
    "${ProtobufSourcesDir}/compiler/code_generator.cc"
    "${ProtobufSourcesDir}/compiler/command_line_interface.cc"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_enum.cc"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_enum_field.cc"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_extension.cc"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_field.cc"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_file.cc"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_generator.cc"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_helpers.cc"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_message.cc"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_message_field.cc"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_primitive_field.cc"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_service.cc"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_string_field.cc"
    "${ProtobufSourcesDir}/compiler/java/java_doc_comment.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_enum.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_enum_field.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_extension.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_field.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_file.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_generator.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_helpers.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_message.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_message_field.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_primitive_field.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_service.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_string_field.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/plugin.cc"
    "${ProtobufSourcesDir}/compiler/plugin.pb.cc"
   "${ProtobufSourcesDir}/compiler/python/python_generator.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/subprocess.cc"
    "${ProtobufSourcesDir}/compiler/zip_writer.cc"
    )
set(ProtocHeaders
    "${ProtobufSourcesDir}/compiler/code_generator.h"
    "${ProtobufSourcesDir}/compiler/command_line_interface.h"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_enum.h"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_enum_field.h"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_extension.h"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_field.h"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_file.h"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_generator.h"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_helpers.h"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_message.h"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_message_field.h"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_options.h"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_primitive_field.h"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_service.h"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_string_field.h"
    "${ProtobufSourcesDir}/compiler/java/java_doc_comment.h"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_enum.h"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_enum_field.h"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_extension.h"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_field.h"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_file.h"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_generator.h"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_helpers.h"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_message.h"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_message_field.h"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_primitive_field.h"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_service.h"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_string_field.h"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/plugin.h"
    "${ProtobufSourcesDir}/compiler/plugin.pb.h"
    "${ProtobufSourcesDir}/compiler/python/python_generator.h"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/subprocess.h"
    "${ProtobufSourcesDir}/compiler/zip_writer.h"
    )
set(ProtocAllFiles ${ProtocSources} ${ProtocHeaders})


set(ProtocMain
    "${ProtobufSourcesDir}/compiler/main.cc"
    )


set(LiteTestSourcesGenerated
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_import_lite.pb.cc"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_import_public_lite.pb.cc"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_lite.pb.cc"
    )
set(LiteTestHeadersGenerated
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_import_lite.pb.h"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_import_public_lite.pb.h"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_lite.pb.h"
    )
set(LiteTestProtoFiles
    "${ProtobufSourcesDir}/unittest_import_lite.proto"
    "${ProtobufSourcesDir}/unittest_import_public_lite.proto"
    "${ProtobufSourcesDir}/unittest_lite.proto"
    )
source_group("Proto Files" FILES ${LiteTestProtoFiles})
set(LiteTestSources
    "${ProtobufSourcesDir}/lite_unittest.cc"
    "${ProtobufSourcesDir}/test_util_lite.cc"
    ${LiteTestSourcesGenerated}
    )
set(LiteTestHeaders
    "${ProtobufSourcesDir}/test_util_lite.h"
    ${LiteTestHeadersGenerated}
    )
set(LiteTestAllFiles ${LiteTestSources} ${LiteTestHeaders} ${LiteTestProtoFiles})


set(TestPluginSources
    "${ProtobufSourcesDir}/compiler/mock_code_generator.cc"
    "${ProtobufSourcesDir}/compiler/test_plugin.cc"
    "${ProtobufSourcesDir}/testing/file.cc"
    )
set(TestPluginHeaders
    "${ProtobufSourcesDir}/compiler/mock_code_generator.h"
    "${ProtobufSourcesDir}/testing/file.h"
    )
set(TestPluginAllFiles ${TestPluginSources} ${TestPluginHeaders})


configure_file("${ProtobufSourcesDir}/testing/googletest.cc.in"
               "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/testing/googletest.cc" @ONLY)
set(TestsSourcesGenerated
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/compiler/cpp/cpp_test_bad_identifiers.pb.cc"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest.pb.cc"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_custom_options.pb.cc"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_embed_optimize_for.pb.cc"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_import.pb.cc"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_import_public.pb.cc"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_lite_imports_nonlite.pb.cc"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_mset.pb.cc"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_no_generic_services.pb.cc"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_optimize_for.pb.cc"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/compiler/command_line_interface_unittest.cc"  # This is configured by CMake, not protoc
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/testing/googletest.cc"  # This is configured by CMake, not protoc
    )
set(TestsHeadersGenerated
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/compiler/cpp/cpp_test_bad_identifiers.pb.h"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest.pb.h"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_custom_options.pb.h"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_embed_optimize_for.pb.h"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_import.pb.h"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_import_public.pb.h"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_lite_imports_nonlite.pb.h"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_mset.pb.h"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_no_generic_services.pb.h"
    "${CMAKE_BINARY_DIR}/GeneratedProtoFiles/google/protobuf/unittest_optimize_for.pb.h"
    )
set(TestsProtoFiles
    "${ProtobufSourcesDir}/compiler/cpp/cpp_test_bad_identifiers.proto"
    "${ProtobufSourcesDir}/unittest.proto"
    "${ProtobufSourcesDir}/unittest_custom_options.proto"
    "${ProtobufSourcesDir}/unittest_embed_optimize_for.proto"
    "${ProtobufSourcesDir}/unittest_import.proto"
    "${ProtobufSourcesDir}/unittest_import_public.proto"
    "${ProtobufSourcesDir}/unittest_lite_imports_nonlite.proto"
    "${ProtobufSourcesDir}/unittest_mset.proto"
    "${ProtobufSourcesDir}/unittest_no_generic_services.proto"
    "${ProtobufSourcesDir}/unittest_optimize_for.proto"
    )
source_group("Proto Files" FILES ${TestsProtoFiles})
set(TestsSources
#    "${ProtobufSourcesDir}/compiler/command_line_interface_unittest.cc"  # This needs the path to protobuf_test_plugin, so is configured by CMake and included in ${TestsSourcesGenerated}
    "${ProtobufSourcesDir}/compiler/cpp/cpp_bootstrap_unittest.cc"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_plugin_unittest.cc"
    "${ProtobufSourcesDir}/compiler/cpp/cpp_unittest.cc"
    "${ProtobufSourcesDir}/compiler/importer_unittest.cc"
    "${ProtobufSourcesDir}/compiler/java/java_doc_comment_unittest.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/java/java_plugin_unittest.cc"  # Should exclude?
    "${ProtobufSourcesDir}/compiler/mock_code_generator.cc"
    "${ProtobufSourcesDir}/compiler/parser_unittest.cc"
    "${ProtobufSourcesDir}/compiler/python/python_plugin_unittest.cc"  # Should exclude?
    "${ProtobufSourcesDir}/descriptor_database_unittest.cc"
    "${ProtobufSourcesDir}/descriptor_unittest.cc"
    "${ProtobufSourcesDir}/dynamic_message_unittest.cc"
    "${ProtobufSourcesDir}/extension_set_unittest.cc"
    "${ProtobufSourcesDir}/generated_message_reflection_unittest.cc"
    "${ProtobufSourcesDir}/io/coded_stream_unittest.cc"
    "${ProtobufSourcesDir}/io/printer_unittest.cc"
    "${ProtobufSourcesDir}/io/tokenizer_unittest.cc"
    "${ProtobufSourcesDir}/io/zero_copy_stream_unittest.cc"
    "${ProtobufSourcesDir}/message_unittest.cc"
    "${ProtobufSourcesDir}/reflection_ops_unittest.cc"
    "${ProtobufSourcesDir}/repeated_field_reflection_unittest.cc"
    "${ProtobufSourcesDir}/repeated_field_unittest.cc"
    "${ProtobufSourcesDir}/stubs/common_unittest.cc"
    "${ProtobufSourcesDir}/stubs/once_unittest.cc"
    "${ProtobufSourcesDir}/stubs/stringprintf_unittest.cc"
    "${ProtobufSourcesDir}/stubs/structurally_valid_unittest.cc"
    "${ProtobufSourcesDir}/stubs/strutil_unittest.cc"
    "${ProtobufSourcesDir}/stubs/template_util_unittest.cc"
    "${ProtobufSourcesDir}/stubs/type_traits_unittest.cc"
    "${ProtobufSourcesDir}/testing/file.cc"
#    "${ProtobufSourcesDir}/testing/googletest.cc"  # This needs the source dir, so is configured by CMake and included in ${TestsSourcesGenerated}
    "${ProtobufSourcesDir}/test_util.cc"
    "${ProtobufSourcesDir}/text_format_unittest.cc"
    "${ProtobufSourcesDir}/unknown_field_set_unittest.cc"
    "${ProtobufSourcesDir}/wire_format_unittest.cc"
    ${TestsSourcesGenerated}
    )
set(TestsHeaders
    "${ProtobufSourcesDir}/compiler/mock_code_generator.h"
    "${ProtobufSourcesDir}/testing/file.h"
    "${ProtobufSourcesDir}/testing/googletest.h"
    "${ProtobufSourcesDir}/test_util.h"
    ${TestsHeadersGenerated}
    )
set(TestsAllFiles ${TestsSources} ${TestsHeaders} ${TestsProtoFiles})


set_source_files_properties(${LiteTestSourcesGenerated} ${LiteTestHeadersGenerated} ${TestsSourcesGenerated} ${TestsHeadersGenerated} PROPERTIES GENERATED TRUE)
