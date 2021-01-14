cmake_minimum_required(VERSION 2.8)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#set(cmdline "cat config_build.py | grep project_name_linux | cut -f2 -d= | sed -r 's/( )*\"//g'")

#=============JUST ALL PROJECT CONFIGURATION IS HERE================================
message(STATUS "INFO>> lw_index_datastructs start of cmake configuration")
set(LW_PROJECT_NAME "lw_index_datastructs")
set(LW_EXTRA_INCLUDE_FOLDERS "./extra/gtest-1.8.0/include")
set(LW_INCLUDE_UNITTESTS "1")
#=============JUST ALL PROJECT CONFIGURATION IS HERE================================

project(${LW_PROJECT_NAME})

#=============LIST OF CONFIGURABLE OPTIONS WITH cmake -D================================
option(LW_VERBOSE_BUILD "Verbose output during create projects" ON)
option(LW_USE_MSVC_LEAK_DETECTOR "Use MSVC leak detection mechanism" OFF)
option(LW_USE_STATIC_CRT "Use static CRT version" OFF)
option(LW_USE_AXV2_SIMD "Use some SIMD extension to leverage on vectorized instructions" ON)
option(LW_USE_EXTRA_CHECK_ASSERTS_IN_RUNTIME "Use extra asserts. Can be usefull for debuging. But can be turn off for release" ON)

#=============APPEND ALL CMAKE SCRIPTS===================================================
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake)
include(ArchConfiguration)
include(HelpFunctions)
include(ToolChainConfiguration)

#=============GLOBAL CMAKE VARIABLE=====================================================
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
messageNormal("PROJECT_ROOT: " ${PROJECT_ROOT})
messageNormal("BUILD TYPE: " ${CMAKE_BUILD_TYPE})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LW_DEBUG_BUILD 1)
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LW_DEBUG_BUILD 0)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(LW_RELEASE_BUILD 1)
else(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(LW_RELEASE_BUILD 0)
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

messageVerbose("================Found sources start===========")
file(GLOB_RECURSE original_src "src/*.cpp")
foreach(f ${original_src})
    messageVerbose(${f})
endforeach()
messageVerbose("================Found sources end=============")

messageVerbose("================Found headers start===========")
file(GLOB_RECURSE original_headers "headers_internal/*.h" "headers_internal/*.hpp" "headers_public/*.h" "headers_public/*.hpp")
messageVerbose("================Found headers end=============")

if (original_src)
    createSourceGrouping(${original_src})
endif(original_src)

if (original_headers)
    createHeadersGrouping(${original_headers})
endif(original_headers)

#==============EXTRA CHECK TO USE THIS PROJECT ONLY AS LIBRARY===========================================
if (${LW_FORCE_USE_LW_LINAL_PRIMITIVES_AS_LIBRARY})
    messageVerbose(" Test and tools forced to be removed ")
    set(LW_INCLUDE_UNITTESTS "0")
    set(LW_INCLUDE_TOOLS "0")
endif() # CMake 2.6.0 the ELSE() and ENDIF() constructs can be empty
#==============EXTRA CHECK TO USE THIS PROJECT ONLY AS LIBRARY===========================================

#============= ALL INCLUDES FOR THIS PROJECT START=========================================================
if (${LW_FORCE_USE_LW_LINAL_PRIMITIVES_AS_LIBRARY})
  #target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_ROOT}/../)
  include_directories(${PROJECT_ROOT}/../)
else()
  messageVerbose("================Extra include folders start===========")
  #target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_ROOT}/../)
  include_directories(${PROJECT_ROOT}/../)
  foreach(folder ${LW_EXTRA_INCLUDE_FOLDERS})
    # https://stackoverflow.com/questions/13703647/how-to-properly-add-include-directories-with-cmake/45636872#45636872
    set(abs_fodler_path "${PROJECT_ROOT}/${folder}")
    messageVerbose("+ " ${abs_fodler_path})
    #target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${abs_fodler_path})
    include_directories(SYSTEM ${abs_fodler_path})
  endforeach()
  messageVerbose("================Extra include folders end===========")
endif()
#============= ALL INCLUDES FOR THIS PROJECT END =========================================================

#============= BUILD TARGETS START=========================================================
add_library(${PROJECT_NAME} STATIC ${original_src} ${original_headers})

if (${LW_INCLUDE_UNITTESTS})
    add_subdirectory(./extra/gtest-1.8.0)
    add_subdirectory(tests)
endif (${LW_INCLUDE_UNITTESTS})
#============= BUILD TARGETS END===========================================================

configureCompileFlags(${PROJECT_NAME})
addDefinition(LW_DEBUG_BUILD)
addDefinition(LW_RELEASE_BUILD)
addDefinition(LW_USE_MSVC_LEAK_DETECTOR)
addDefinition(LW_USE_STATIC_CRT)
addDefinition(LW_USE_AXV2_SIMD)
addDefinition(LW_USE_EXTRA_CHECK_ASSERTS_IN_RUNTIME)

#=======================================================================================
printInfo()
#=======================================================================================
