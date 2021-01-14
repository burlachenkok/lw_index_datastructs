#============================================================================================
# Message wrapper for verbose message
#============================================================================================
function(messageVerbose)
  if(LW_VERBOSE_BUILD)
    message(STATUS "project-" ${PROJECT_NAME} ": " ${ARGV})
  endif(LW_VERBOSE_BUILD)
endfunction(messageVerbose)

#============================================================================================
# Message wrapper for normal message
#============================================================================================
function(messageNormal)
  message(STATUS "project-" ${PROJECT_NAME} ": " ${ARGV})
endfunction(messageNormal)

#===========================================================================================
# Print info about system
#===========================================================================================
function(printInfo)
    messageNormal("System: " ${CMAKE_SYSTEM_NAME} " " ${CMAKE_SYSTEM_VERSION})
    messageNormal("Processor of host system: " ${CMAKE_HOST_SYSTEM_PROCESSOR})
    messageNormal("Processor of target system: " ${CMAKE_SYSTEM_PROCESSOR})
    messageNormal("CMake generator: " ${CMAKE_GENERATOR})
    messageNormal("Binary tree: " ${PROJECT_BINARY_DIR})
    messageNormal("Source tree: " ${PROJECT_SOURCE_DIR})
    messageNormal("Project root: " ${PROJECT_ROOT})
    messageNormal("Build type: " ${CMAKE_BUILD_TYPE})
    messageNormal("Toolchain file: " ${CMAKE_TOOLCHAIN_FILE})
    messageNormal("Project name: " ${PROJECT_NAME})
    messageNormal("Project Lw name: " ${LW_PROJECT_NAME})
endfunction(printInfo)

#===========================================================================================
# Add extra definition for C preprocessor 
#===========================================================================================
function(addDefinition option_name)
    if (${option_name})
        target_compile_definitions(${PROJECT_NAME} PRIVATE ${option_name}=1)
    else(${option_name})
        target_compile_definitions(${PROJECT_NAME} PRIVATE ${option_name}=0)
    endif(${option_name})
endfunction(addDefinition)

#============================================================================================
# Create source and Headers files grouping in Visual Studio
#============================================================================================
function(createSourceGrouping)
    if(MSVC)
        foreach(f ${ARGV})
            file(RELATIVE_PATH SRCGR ${PROJECT_ROOT} ${f})
            set(SRCGR "Sources/${SRCGR}")
            string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})
            string(REPLACE / \\ SRCGR ${SRCGR})
            source_group("${SRCGR}" FILES ${f})
        endforeach()
    endif(MSVC)
endfunction(createSourceGrouping)

#============================================================================================
# Create Headers files grouping in Visual Studio
#============================================================================================
function(createHeadersGrouping)
    if(MSVC)
        foreach(f ${ARGV})
            file(RELATIVE_PATH SRCGR ${PROJECT_ROOT} ${f})
            set(SRCGR "Headers/${SRCGR}")
            string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})
            string(REPLACE / \\ SRCGR ${SRCGR})
            source_group("${SRCGR}" FILES ${f})
        endforeach()
    endif(MSVC)
endfunction(createHeadersGrouping)

#============================================================================================
# Rather specific compiler flags
# Right now configure CRT for Visual Studio builds
#============================================================================================
macro(configureCompileFlags projectName)
    if(MSVC)
         # Select crt version for windows builds based on generator expression syntax: 
         # https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html
         # http://stackoverflow.com/questions/34490294/what-does-configdebugrelease-mean-in-cmake
         # http://stackoverflow.com/questions/10199904/how-can-i-set-specific-compiler-flags-for-for-a-specific-target-in-a-specific-bu
         if (LW_USE_STATIC_CRT)
             target_compile_options(${projectName} PRIVATE "/MT$<$<CONFIG:Debug>:d>")
         else(LW_USE_STATIC_CRT)
             target_compile_options(${projectName} PRIVATE "/MD$<$<CONFIG:Debug>:d>")
         endif (LW_USE_STATIC_CRT)
        target_compile_definitions(${projectName} PRIVATE "_VARIADIC_MAX=10")
    endif(MSVC)
endmacro(configureCompileFlags)
