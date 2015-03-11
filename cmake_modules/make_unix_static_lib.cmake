set(TEMP_DIR ${CMAKE_BINARY_DIR}/merge_libs_temp_dir)
make_directory(${TEMP_DIR})
string (REPLACE " " ";" STATIC_LIBS "${STATIC_LIBS}")

foreach(LIB ${STATIC_LIBS})
get_filename_component(NAME_NO_EXT ${LIB} NAME_WE)
set(TEMP_SUBDIR ${TEMP_DIR}/${NAME_NO_EXT})
make_directory(${TEMP_SUBDIR})
  execute_process(
      COMMAND ${CMAKE_AR} -t ${LIB}
      OUTPUT_VARIABLE LIB_OBJS
      )
  string(REGEX REPLACE "\n" ";" LIB_OBJ_LIST "${LIB_OBJS}")
  string(REGEX REPLACE ";$" "" LIB_OBJ_LIST "${LIB_OBJ_LIST}")

  list(LENGTH LIB_OBJ_LIST LENGTH_WITH_DUPS)
set(LIB_OBJ_LIST_NO_DUPS ${LIB_OBJ_LIST})
  list(REMOVE_DUPLICATES LIB_OBJ_LIST_NO_DUPS)
list(LENGTH LIB_OBJ_LIST_NO_DUPS LENGTH_WITHOUT_DUPS)

if(LENGTH_WITH_DUPS EQUAL LENGTH_WITHOUT_DUPS)
  execute_process(
      COMMAND ${CMAKE_AR} -x ${LIB}
      WORKING_DIRECTORY ${TEMP_SUBDIR}
      )
  else()
  list(SORT LIB_OBJ_LIST)
  set(SAME_OBJ_COUNT 1)
  set(LAST_OBJ_NAME)
foreach(OBJ ${LIB_OBJ_LIST})
  if(OBJ STREQUAL LAST_OBJ_NAME)
get_filename_component(OBJ_NO_EXT ${OBJ} NAME_WE)
  file(RENAME "${TEMP_SUBDIR}/${OBJ}" "${TEMP_SUBDIR}/${OBJ_NO_EXT}.${SAME_OBJ_COUNT}.o")
  math(EXPR SAME_OBJ_COUNT "${SAME_OBJ_COUNT}+1")
  else()
  set(SAME_OBJ_COUNT 1)
endif()
  set(LAST_OBJ_NAME "${OBJ}")
  execute_process(
      COMMAND ${CMAKE_AR} -xN ${SAME_OBJ_COUNT} ${LIB} ${OBJ}
      WORKING_DIRECTORY ${TEMP_SUBDIR}
      )
  endforeach()
endif()

  file(GLOB_RECURSE LIB_OBJECTS "${TEMP_SUBDIR}/*.o")
set(OBJECTS ${OBJECTS} ${LIB_OBJECTS})
endforeach()

# Use relative paths, makes command line shorter.
get_filename_component(ABS_TEMP_DIR ${TEMP_DIR} ABSOLUTE)
foreach(OBJ ${OBJECTS})
file(RELATIVE_PATH OBJ ${ABS_TEMP_DIR} ${OBJ})
file(TO_NATIVE_PATH ${OBJ} OBJ)
set(ALL_OBJECTS ${ALL_OBJECTS} ${OBJ})
endforeach()

file(TO_NATIVE_PATH ${TARGET_LOCATION} ${TARGET_LOCATION})
# Now pack the objects into library with ar.
  execute_process(
      COMMAND ${CMAKE_AR} rcs ${TARGET_LOCATION} ${ALL_OBJECTS}
      WORKING_DIRECTORY ${TEMP_DIR}
      )

# Cleanup
file(REMOVE_RECURSE ${TEMP_DIR})
