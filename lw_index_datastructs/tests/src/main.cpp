#if LW_USE_MSVC_LEAK_DETECTOR
#include <crtdbg.h>
#endif
#include "gtest/gtest.h"
#include "GTestMacroses.h"

#include  <time.h>

int main(int argc, char **argv)
{
    gSetupAllowLogRecordPropertyToStdout(true);

	/*
    | Wildcard filter for run test | Meaning          |
    --------------------------------------------------
    |    *GTest*                   | basic test       |
    |    *GPerf*                   | performance test |
    |      *                       | run all tests    |
    ==================================================*/

	/* Notes
		1. Filter setuped via command-line has priority over filter setuped via source code
	*/

    ::testing::GTEST_FLAG(filter) = "-*GPerf*";
    ::testing::GTEST_FLAG(output) = "xml:./unit_and_perf_tests.xml";
    ::testing::GTEST_FLAG(repeat) = 1;
    ::testing::GTEST_FLAG(color) = "yes";
    ::testing::InitGoogleTest(&argc, argv);

    clock_t timestampStartTests = clock();
    int result = RUN_ALL_TESTS();
    clock_t timestampEndTests = clock();

    double deltaSeconds = (timestampEndTests - timestampStartTests) / double(CLOCKS_PER_SEC);

    printf("=======================================================================\n");
    printf("Binary: %s\n", argv[0]);
    printf("Time spent to execution unit tests: %lf seconds\n", deltaSeconds);
    printf("Filter for test execution: %s\n", ::testing::GTEST_FLAG(filter).c_str());
    printf("Size of data 'pointer' in bytes: %i\n", static_cast<int>(sizeof(void*)));

#if   LW_DEBUG_BUILD && !LW_RELEASE_BUILD
    printf("Build type: DEBUG\n");
#elif !LW_DEBUG_BUILD && LW_RELEASE_BUILD
    printf("Build type: RELEASE\n");
#else
    printf("Build type: UNDEFINED (lack of CMake documentation which build configuration is default)\n");
#endif

#if LW_USE_STATIC_CRT
    printf("Compiled with: static CRT\n");
#else
    printf("Compiled with: dynamic CRT\n");
#endif

#if LW_USE_AXV2_SIMD
    printf("Compiled with: support SSE4.2\n");
#else
    printf("Compiled with: no support SSE4.2\n");
#endif

    printf("Date and time for build unittest: " __DATE__ "/" __TIME__ "\n");
    printf("=======================================================================\n");

#if LW_USE_MSVC_LEAK_DETECTOR
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();
#endif

	return result;
}
