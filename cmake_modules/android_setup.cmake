#==================================================================================================#
#                                                                                                  #
#  Copyright 2014 MaidSafe.net limited                                                             #
#                                                                                                  #
#  This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,        #
#  version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which    #
#  licence you accepted on initial access to the Software (the "Licences").                        #
#                                                                                                  #
#  By contributing code to the MaidSafe Software, or to this project generally, you agree to be    #
#  bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root        #
#  directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available   #
#  at: http://www.maidsafe.net/licenses                                                            #
#                                                                                                  #
#  Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed    #
#  under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF   #
#  ANY KIND, either express or implied.                                                            #
#                                                                                                  #
#  See the Licences for the specific language governing permissions and limitations relating to    #
#  use of the MaidSafe Software.                                                                   #
#                                                                                                  #
#==================================================================================================#
#                                                                                                  #
#  Module used to create android setup for each project                                            #
#                                                                                                  #
#==================================================================================================#

# Disable android builds for any platform but linux
if(MSVC OR APPLE)
  message(FATAL_ERROR "Android builds are currently not supported in this platform")
endif()

if(NOT ANDROID_NDK_TOOLCHAIN_ROOT)
  set(ErrorMessage "\n\nCan't find ANDROID_NDK_TOOLCHAIN_ROOT. ")
  set(ErrorMessage "${ErrorMessage}To set it, run:\n")
  set(ErrorMessage "${ErrorMessage}    cmake . -DANDROID_NDK_TOOLCHAIN_ROOT=\"<path to ndk toolchain>\"\n\n")
  message(FATAL_ERROR "${ErrorMessage}")
endif()

function(get_android_api_version_from_file File ApiLevel)
  if(EXISTS "${File}")
    file(STRINGS "${File}" AndroidVersionLine REGEX "#define __ANDROID_API__")
    string(REGEX MATCH "[0-9]+" MatchedVersion "${AndroidVersionLine}")
    if(MatchedVersion)
      set(${ApiLevel} ${MatchedVersion} PARENT_SCOPE)
    else()
      unset(${ApiLevel} PARENT_SCOPE)
    endif()
  endif()
endfunction()

set(CMAKE_C_COMPILER "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/arm-linux-androideabi-gcc")
set(CMAKE_CXX_COMPILER "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/arm-linux-androideabi-g++")

macro(ms_android_setup_flags)
  get_android_api_version_from_file("${ANDROID_NDK_TOOLCHAIN_ROOT}/sysroot/usr/include/android/api-level.h" AndroidApiLevel)
  string(REGEX REPLACE "([0-9]\\.[0-9]).*" "\\1" AndroidGccVersion "${CMAKE_CXX_COMPILER_VERSION}")

  set(ExeLinkerFlags "-L${ANDROID_NDK_TOOLCHAIN_ROOT}/arm-linux-androideabi/lib/armv7-a -lstdc++")

  if(NOT AndroidApiLevel LESS 16)
    set(PlatformFlags "-fPIE")
    set(ExeLinkerFlags "${ExeLinkerFlags} -pie -rdynamic")
  endif()

  set(PLATFORM_PREFIX "${ANDROID_NDK_TOOLCHAIN_ROOT}/sysroot")
  set(PLATFORM_FLAGS "${PlatformFlags} -fPIC -Wno-psabi --sysroot=${PLATFORM_PREFIX}")
  set(CMAKE_C_FLAGS "${PLATFORM_FLAGS} -march=armv7-a -mfloat-abi=softfp -mfpu=neon")
  set(CMAKE_CXX_FLAGS "${PLATFORM_FLAGS} -march=armv7-a -mfloat-abi=softfp -mfpu=neon")
  set(CMAKE_EXE_LINKER_FLAGS "${ExeLinkerFlags}")
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--fix-cortex-a8")
endmacro()
