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
#  Module used to build Callback File System (CBFS) libs.                                          #
#                                                                                                  #
#  Simply do:                                                                                      #
#      cmake -P <path to this script>                                                              #
#                                                                                                  #
#  If the script fails, see                                                                        #
#      https://github.com/maidsafe/MaidSafe/wiki/Installing-Prerequisites-and-Optional-Components  #
#  for manual instructions.                                                                        #
#                                                                                                  #
#==================================================================================================#


set(RegistryEntries "[HKEY_CURRENT_USER\\Software\\Eldos\\CallbackFS;InstallPath]")
execute_process(COMMAND wmic useraccount get name,sid OUTPUT_VARIABLE OutVar OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX MATCHALL "S-1-5[-0-9]+" UsersSecurityIDs "${OutVar}")
foreach(UsersSecurityID ${UsersSecurityIDs})
  list(APPEND RegistryEntries "[HKEY_USERS\\${UsersSecurityID}\\Software\\Eldos\\CallbackFS;InstallPath]")
endforeach()

unset(SolutionFile)
unset(ProjectFile)
foreach(RegistryEntry ${RegistryEntries})
  get_filename_component(CbfsRootDir "${RegistryEntry}" ABSOLUTE CACHE)
  find_file(SolutionFile NAMES CbFS_VS2013.sln PATHS ${CbfsRootDir}/SourceCode/CBFS/CPP NO_DEFAULT_PATH)
  find_file(ProjectFile NAMES CbFS_VS2013.vcxproj PATHS ${CbfsRootDir}/SourceCode/CBFS/CPP NO_DEFAULT_PATH)
  if(SolutionFile AND ProjectFile)
    break()
  else()
    unset(CbfsRootDir CACHE)
    unset(SolutionFile CACHE)
    unset(ProjectFile CACHE)
  endif()
endforeach()

if(SolutionFile AND ProjectFile)
  message("Found CBFS at ${CbfsRootDir}")
else()
  message(FATAL_ERROR "Failed to find .sln and/or .vcxproj file.")
endif()

execute_process(COMMAND wmic os get osarchitecture OUTPUT_VARIABLE OutVar)
string(REGEX MATCH "64-bit" x64 "${OutVar}")
set(ExcludeFromBuild Itanium Dynamic)
if(NOT x64)
  list(APPEND ExcludeFromBuild x64 X64)
endif()

file(STRINGS "${SolutionFile}" SolutionContents)
file(STRINGS "${ProjectFile}" ProjectContents)

foreach(Line ${SolutionContents})
  unset(Found)
  foreach(ExcludeString ${ExcludeFromBuild})
    string(REGEX MATCH "${ExcludeString}" Found "${Line}")
    if(Found)
      break()
    endif()
  endforeach()
  if(NOT Found)
    set(SolutionClean "${SolutionClean}${Line}\n")
  endif()
endforeach()
file(WRITE ${CbfsRootDir}/SourceCode/CBFS/CPP/CbFS_VS2013.sln "${SolutionClean}")

foreach(Line ${ProjectContents})
  unset(FoundClosingTag)
  if(ClosingTag)
    set(UseCurrentLine OFF)
    string(REGEX MATCH "${ClosingTag}" FoundClosingTag "${Line}")
    if(FoundClosingTag)
      unset(ClosingTag)
    endif()
  else()
    set(UseCurrentLine ON)
    unset(Found)
    foreach(ExcludeString ${ExcludeFromBuild})
      string(REGEX MATCH "${ExcludeString}" Found "${Line}")
      if(Found)
        break()
      endif()
    endforeach()
    if(Found)
      set(UseCurrentLine OFF)
      string(REGEX REPLACE "[ ]*<([^ >]+).*" "\\1" Tag "${Line}")
      set(ClosingTag "</${Tag}>")
      string(REGEX MATCH "${ClosingTag}|/>" FoundClosingTag "${Line}")
      if(FoundClosingTag)
        unset(ClosingTag)
      endif()
    endif()
  endif()
  if(UseCurrentLine)
    if(CurrentPlatformType STREQUAL x64)
      if(CurrentMode STREQUAL Debug)
        string(REGEX REPLACE "\\.\\\\Debug([\\/])" ".\\\\x64\\\\Debug\\1" Line "${Line}")
      elseif(CurrentMode STREQUAL Release)
        string(REGEX REPLACE "\\.\\\\Release([\\/])" ".\\\\x64\\\\Release\\1" Line "${Line}")
      endif()
    endif()

    if(FoundItemDefinitionGroup)
      if(FoundClCompile)
        string(REGEX MATCH "DebugInformationFormat" FoundDebugInformationFormat "${Line}")
        string(REGEX MATCH "/ClCompile" FoundCompileClosingTag "${Line}")
        if(FoundDebugInformationFormat)
          string(REGEX REPLACE "([ ]+)<DebugInformationFormat>.*" "\\1<DebugInformationFormat>${DebugInfoFormat}</DebugInformationFormat>" Line "${Line}")
          unset(FoundItemDefinitionGroup)
          unset(FoundClCompile)
          unset(FoundDebugInformationFormat)
          unset(FoundCompileClosingTag)
        elseif(FoundCompileClosingTag)
          set(ProjectClean "${ProjectClean}      <DebugInformationFormat>${DebugInfoFormat}</DebugInformationFormat>\n")
          unset(FoundItemDefinitionGroup)
          unset(FoundClCompile)
          unset(FoundDebugInformationFormat)
          unset(FoundCompileClosingTag)
        endif()
      else()
        string(REGEX MATCH "<ClCompile>" FoundClCompile "${Line}")
      endif()
    else()
      string(REGEX MATCH "<ItemDefinitionGroup" FoundItemDefinitionGroup "${Line}")
      if(FoundItemDefinitionGroup)
        unset(CurrentMode)
        string(REGEX MATCH "Debug" CurrentMode "${Line}")
        if(CurrentMode)
          set(DebugInfoFormat "ProgramDatabase")
        else()
          set(CurrentMode Release)
          set(DebugInfoFormat "None")
        endif()
        unset(CurrentPlatformType)
        string(REGEX MATCH "x64" CurrentPlatformType "${Line}")
      endif()
    endif()

    string(REGEX REPLACE "([ ]+<RuntimeLibrary>MultiThreaded(Debug)?)</RuntimeLibrary>" "\\1DLL</RuntimeLibrary>" Line "${Line}")
    set(ProjectClean "${ProjectClean}${Line}\n")
  endif()
endforeach()
file(WRITE ${CbfsRootDir}/SourceCode/CBFS/CPP/CbFS_VS2013.vcxproj "${ProjectClean}")

if(x64)
  message("Building 64-bit Debug version")
  execute_process(COMMAND msbuild /M:7 /P:Configuration=Debug,Platform=x64 CbFS_VS2013.sln
                  WORKING_DIRECTORY "${CbfsRootDir}/SourceCode/CBFS/CPP"
                  RESULT_VARIABLE ResVar OUTPUT_VARIABLE OutVar)
  if(NOT ResVar EQUAL 0)
    message(FATAL_ERROR "Failed to run command msbuild /M:7 /P:Configuration=Debug,Platform=x64 CbFS_VS2013.sln from within \"${CbfsRootDir}/SourceCode/CBFS/CPP\"\n\n${OutVar}") 
  endif()
  
  message("Building 64-bit Release version")
  execute_process(COMMAND msbuild /M:7 /P:Configuration=Release,Platform=x64 CbFS_VS2013.sln
                  WORKING_DIRECTORY "${CbfsRootDir}/SourceCode/CBFS/CPP"
                  RESULT_VARIABLE ResVar OUTPUT_VARIABLE OutVar)
  if(NOT ResVar EQUAL 0)
    message(FATAL_ERROR "Failed to run command msbuild /M:7 /P:Configuration=Release,Platform=x64 CbFS_VS2013.sln from within \"${CbfsRootDir}/SourceCode/CBFS/CPP\"\n\n${OutVar}")
  endif()
endif()

message("Building 32-bit Debug version")
execute_process(COMMAND msbuild /M:7 /P:Configuration=Debug,Platform=Win32 CbFS_VS2013.sln
                WORKING_DIRECTORY "${CbfsRootDir}/SourceCode/CBFS/CPP"
                RESULT_VARIABLE ResVar OUTPUT_VARIABLE OutVar)
if(NOT ResVar EQUAL 0)
  message(FATAL_ERROR "Failed to run command msbuild /M:7 /P:Configuration=Debug,Platform=Win32 CbFS_VS2013.sln from within \"${CbfsRootDir}/SourceCode/CBFS/CPP\"\n\n${OutVar}") 
endif()

message("Building 32-bit Release version")
execute_process(COMMAND msbuild /M:7 /P:Configuration=Release,Platform=Win32 CbFS_VS2013.sln
                WORKING_DIRECTORY "${CbfsRootDir}/SourceCode/CBFS/CPP"
                RESULT_VARIABLE ResVar OUTPUT_VARIABLE OutVar)
if(NOT ResVar EQUAL 0)
  message(FATAL_ERROR "Failed to run command msbuild /M:7 /P:Configuration=Release,Platform=Win32 CbFS_VS2013.sln from within \"${CbfsRootDir}/SourceCode/CBFS/CPP\"\n\n${OutVar}")
endif()

message("Now do the following:\n  * configure the MaidSafe super project (run cmake on it)\n  * build the 'cbfs_driver_installer' target\n  * run cbfs_driver_installer with '-i' to install these new drivers")
