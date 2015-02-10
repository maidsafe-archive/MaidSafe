project(breakpad-client)
cmake_minimum_required(VERSION 2.8.4)

set(CMAKE_DEBUG_POSTFIX _d)

set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(src)

set(SOURCES)

if(UNIX)
  add_definitions(-Wall)
  set(SOURCES
    src/client/minidump_file_writer.cc
    src/common/convert_UTF.c
    src/common/string_conversion.cc
  )
  if(APPLE)
    include_directories(/Developer/Headers/FlatCarbon)
    find_library(COCOA_LIBRARY COCOA)
    mark_as_advanced(COCOA_LIBRARY)
    set(EXTRA_LIBS crypto ${COCOA_LIBRARY})
    set(SOURCES ${SOURCES}
	src/client/mac/handler/breakpad_nlist_64.cc
	src/client/mac/handler/dynamic_images.cc
	src/client/mac/handler/exception_handler.cc
	src/client/mac/handler/minidump_generator.cc
	src/client/mac/handler/protected_memory_allocator.cc
	src/common/mac/dump_syms.mm
#	src/common/mac/dwarf/bytereader.cc
#	src/common/mac/dwarf/dwarf2reader.cc
#	src/common/mac/dwarf/functioninfo.cc
	src/common/mac/file_id.cc
	src/common/mac/macho_id.cc
	src/common/mac/macho_utilities.cc
	src/common/mac/macho_walker.cc
	src/common/mac/string_utilities.cc
)
  else()
    set(SOURCES ${SOURCES}
      src/client/linux/handler/exception_handler.cc
      #src/client/linux/handler/linux_thread.cc
      #src/client/linux/handler/minidump_generator.cc
      src/client/linux/crash_generation/crash_generation_client.cc #ex1
      src/client/linux/minidump_writer/minidump_writer.cc #ex2
      src/client/linux/minidump_writer/linux_dumper.cc #ex3
      src/common/linux/dump_symbols.cc
      src/common/linux/file_id.cc
      src/common/linux/guid_creator.cc
      src/common/linux/http_upload.cc
      src/common/md5.c
      src/common/dwarf_cfi_to_module.cc
      src/common/dwarf_cu_to_module.cc
      src/common/dwarf_line_to_module.cc
      src/common/dwarf/bytereader.cc
      src/common/dwarf/dwarf2reader.cc
      src/common/module.cc
      src/common/language.cc
      src/common/linux/elf_symbols_to_module.cc
      src/common/stabs_to_module.cc
    )
  endif()
elseif(WIN32)
  set(SOURCES
    src/common/windows/guid_string.cc
    src/client/windows/crash_generation/crash_generation_client.cc
    src/client/windows/crash_generation/client_info.cc
    src/client/windows/crash_generation/minidump_generator.cc
    src/client/windows/handler/exception_handler.cc
  )
  #ADD_DEFINITIONS(/Zc:wchar_t-)
  add_definitions(/D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "_UNICODE" /D "UNICODE" /EHsc)
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /Ot /Oy /D \"NDEBUG\" /MT /Gy /Zi")
  if(CMAKE_CL_64)
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Ot /Oy /D \"_DEBUG\" /D \"DEBUG\" /MTd /c /Zi /TP")
  else()
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Ot /Oy /D \"_DEBUG\" /D \"DEBUG\" /MTd /c /ZI /TP")
  endif()
endif()

add_library(breakpad STATIC ${SOURCES})

if(UNIX AND NOT APPLE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
    set_target_properties(breakpad PROPERTIES COMPILE_FLAGS "-fPIC -stdlib=libc++ -std=c++11")
  else()
    set_target_properties(breakpad PROPERTIES COMPILE_FLAGS "-fPIC -std=c++11")
  endif()
endif()

target_link_libraries(breakpad ${EXTRA_LIBS})
