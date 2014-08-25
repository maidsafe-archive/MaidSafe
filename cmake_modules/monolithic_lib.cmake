
function(GET_DEPEND_OS_LIBS target result)
  SET(deps ${${target}_LIB_DEPENDS})
  IF(deps)
   FOREACH(lib ${deps})
    IF(NOT lib MATCHES "general" AND NOT lib MATCHES "debug" AND NOT lib MATCHES "optimized")
      GET_TARGET_PROPERTY(lib_location ${lib} LOCATION)
      IF(NOT lib_location)
        SET(ret ${ret} ${lib})
      ENDIF()
    ENDIF()
   ENDFOREACH()
  ENDIF()
  SET(${result} ${ret} PARENT_SCOPE)
endfunction()

set(DevLibDepends maidsafe_common
                  maidsafe_passport
                  maidsafe_rudp
                  maidsafe_routing
                  maidsafe_encrypt
                  maidsafe_api
                  maidsafe_nfs_core
                  maidsafe_nfs_client
                  ${AllBoostLibs}
                  cryptopp
                  protobuf_lite
                  protobuf
                  sqlite)
list(REMOVE_ITEM DevLibDepends BoostGraphParallel BoostMath BoostMpi BoostRegex BoostSerialization BoostTest)
foreach(Libb ${DevLibDepends})
  message("lib - ${Libb}")
endforeach()

SET(SOURCE_FILE ${CMAKE_CURRENT_BINARY_DIR}/maidsafe_depends.cc)
ADD_LIBRARY(maidsafe STATIC ${SOURCE_FILE})
target_include_directories(maidsafe PUBLIC "${CMAKE_SOURCE_DIR}/src/common/include/maidsafe")
SET(OSLIBS)
FOREACH(LIB ${DevLibDepends})
  GET_TARGET_PROPERTY(LIB_LOCATION ${LIB} LOCATION_RELEASE)
  GET_TARGET_PROPERTY(LIB_TYPE ${LIB} TYPE)
  IF(NOT LIB_LOCATION)
     LIST(APPEND OSLIBS ${LIB})
  ELSE()
    IF(LIB_TYPE STREQUAL "STATIC_LIBRARY")
      SET(STATIC_LIBS ${STATIC_LIBS} ${LIB_LOCATION})
      ADD_DEPENDENCIES(maidsafe ${LIB})
      GET_DEPEND_OS_LIBS(${LIB} LIB_OSLIBS)
      LIST(APPEND OSLIBS ${LIB_OSLIBS})
    ELSE()
      LIST(APPEND OSLIBS ${LIB})
    ENDIF()
  ENDIF()
ENDFOREACH()
IF(OSLIBS)
  LIST(REMOVE_DUPLICATES OSLIBS)
  TARGET_LINK_LIBRARIES(maidsafe PUBLIC ${OSLIBS})
ENDIF()

ADD_CUSTOM_COMMAND(
  OUTPUT  ${SOURCE_FILE}
  COMMAND ${CMAKE_COMMAND}  -E touch ${SOURCE_FILE}
  DEPENDS ${STATIC_LIBS})

IF(MSVC)
  SET(LINKER_EXTRA_FLAGS "")
  FOREACH(LIB ${STATIC_LIBS})
    SET(LINKER_EXTRA_FLAGS "${LINKER_EXTRA_FLAGS} ${LIB}")
  ENDFOREACH()
  SET_TARGET_PROPERTIES(maidsafe PROPERTIES STATIC_LIBRARY_FLAGS
    "${LINKER_EXTRA_FLAGS}")
ELSE()
  GET_TARGET_PROPERTY(TARGET_LOCATION maidsafe LOCATION)
  IF(APPLE)
    ADD_CUSTOM_COMMAND(TARGET maidsafe POST_BUILD
      COMMAND rm ${TARGET_LOCATION}
      COMMAND /usr/bin/libtool -static -o ${TARGET_LOCATION}
      ${STATIC_LIBS}
    )
  ELSE()
    ADD_CUSTOM_COMMAND(TARGET maidsafe POST_BUILD
      COMMAND rm ${TARGET_LOCATION}
      COMMAND ${CMAKE_COMMAND}
      -DSUPER_PROJECT_BINARY_DIR="${CMAKE_BINARY_DIR}"
      -DSUPER_PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
      -DTARGET_LOCATION="${TARGET_LOCATION}"
      -DSTATIC_LIBS="${STATIC_LIBS}"
      -DLIB="${LIB}"
      -DCMAKE_AR="${CMAKE_AR}"
      -DConfig=$<CONFIGURATION>
      -P "${CMAKE_SOURCE_DIR}/cmake_modules/make_unix_static_lib.cmake"
      -V
    )
  ENDIF()
ENDIF()


