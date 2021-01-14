if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP") # Parallel compilation (https://msdn.microsoft.com/en-us/library/bb385193.aspx)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996") # MS: The POSIX name for this item is deprecated, ME: No it's not.

    #=============Profiling memory leaks via MSVC start===========================================
    if(LW_USE_MSVC_LEAK_DETECTOR)
        add_definitions(-D_CRTDBG_MAP_ALLOC)
    #   add_definitions("-DDBG_NEW=new(_NORMAL_BLOCK,__FILE__,__LINE__)")
    #   add_definitions(-Dnew=DBG_NEW)
    endif(LW_USE_MSVC_LEAK_DETECTOR)
    #=============Profiling memory leaks via MSVC end ============================================   

    if(LW_USE_AXV2_SIMD)
        # Support of AVX2 instructions with extended XMM* (128bit long) to YMM* (256bit long registers)
        # https://msdn.microsoft.com/en-us/library/jj620901(v=vs.120).aspx
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:sse2")     # Not need for x86_64
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__SSE4_2__")    # declare used standart of the language
    endif(LW_USE_AXV2_SIMD)
endif(MSVC)


if (CMAKE_COMPILER_IS_GNUCXX)
    set (CXX_STANDARD "c++11")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=${CXX_STANDARD}")    # declare used standart of the language
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing")          # ignore narrowing. 
                                                                       # widening  -- is when you cast e.g. 'integer' => 'double' and you increase precision
                                                                       # narrowing -- is when you cast e.g. 'double'  => 'integer' and you lose precision
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")                # append pthread as compiler flag

    if (LW_CODE_COVERAGE_GCOV_IS_ON)
        # First gcc flag lead to save statistics of the source code execution
        # Second gcc flag lead to save statistics for branching in the source code
        add_definitions(-fprofile-arcs -ftest-coverage)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lgcov --coverage")
        message(STATUS "Code coverage via gcov have been activated...")

        # next you need to run the executable to generate files required for coverage report generation
    endif (LW_CODE_COVERAGE_GCOV_IS_ON)

    if(LW_USE_AXV2_SIMD)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2")
    endif(LW_USE_AXV2_SIMD)
endif(CMAKE_COMPILER_IS_GNUCXX)

# Select version of CRT for: Google Test  
#if(LW_USE_STATIC_CRT)
#    set(gtest_force_shared_crt 0)
#else()
#    set(gtest_force_shared_crt 1)
#endif(LW_USE_STATIC_CRT)

# Append OpenMP
#find_package(OpenMP)
#if (OPENMP_FOUND)
#    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#    message(STATUS "OpenMP have been found project-" ${PROJECT_NAME})
#else()
#    message(STATUS "OpenMP have not been found project-" ${PROJECT_NAME})
#endif()

