cmake_minimum_required(VERSION 2.8)

project("${LW_PROJECT_NAME}_unittest")

#include_directories(SYSTEM gtest-1.8.0/include)
#target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE )
#add_subdirectory(gtest-1.8.0)

messageVerbose("================Found sources start===========")
file(GLOB_RECURSE original_src "src/*.cpp")
foreach(f ${original_src})
    messageVerbose(${f})
endforeach()
messageVerbose("================Found sources end=============")

messageVerbose("================Found headers start===========")
file(GLOB_RECURSE original_headers "include/*.h")
messageVerbose("================Found headers end=============")

if (original_src)
    createSourceGrouping(${original_src})
endif(original_src)

if (original_headers)
    createHeadersGrouping(${original_headers})
endif(original_headers)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(${PROJECT_NAME} ${original_src} ${original_headers})

# Add link and depencies to gtest. 
target_link_libraries(${PROJECT_NAME} gtest)
target_link_libraries(${PROJECT_NAME} ${LW_PROJECT_NAME})

# Not necessesary to append ADD_DEPENDENCIES https://cmake.org/pipermail/cmake/2007-August/015649.html
# add_dependencies(${PROJECT_NAME} gtest)

configureCompileFlags(${PROJECT_NAME})
addDefinition(LW_DEBUG_BUILD)
addDefinition(LW_RELEASE_BUILD)
addDefinition(LW_USE_MSVC_LEAK_DETECTOR)
addDefinition(LW_USE_STATIC_CRT)
addDefinition(LW_USE_AXV2_SIMD)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_ROOT}/../)

#messageVerbose("================Extra include folders start===========")
#foreach(folder ${LW_EXTRA_INCLUDE_FOLDERS})
#    set(abs_fodler_path "${PROJECT_ROOT}/${folder}")
#    messageVerbose("+ " ${abs_fodler_path})
#    target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${abs_fodler_path})
#endforeach()
#messageVerbose("================Extra include folders end===========")
