#include "lw_index_datastructs/headers_public/KdTree.h"
#include "GTestMacroses.h"

#include <vector>

TEST(Utils, KdTreeGTest)
{
    {
        lw_index_datastructs::KDtree<int, 2, double> kd;
        EXPECT_TRUE(kd.size() == 0);
        EXPECT_TRUE(kd.height() == 0);

        {
            int points[][2] = { { 0, 2 }, { 10, 10 }, { 0, 10 }, {15, 0} };
            kd.pushInTree(points[1]);
            EXPECT_TRUE(kd.sizeOfDisabledPoints() == 0);
            EXPECT_TRUE(kd.sizeOfEnablePoints() == 1);            
            EXPECT_TRUE(kd.size() == 1);
            EXPECT_TRUE(kd.height() == 1);
            kd.pushInTree(points[2]);
            EXPECT_TRUE(kd.size() == 2);
            EXPECT_TRUE(kd.height() == 2);

            lw_index_datastructs::KDtree<int, 2, double> kdCopy(kd);
            kd.removeAll();
            EXPECT_TRUE(kdCopy.size() == 2);
            EXPECT_TRUE(kd.size() == 0);
            EXPECT_TRUE(kd.kDimensions() == 2);

            kd.removeAll();
            for (size_t i = 0; i < 4; ++i)
                kd.pushInTree(points[i]);
           
            int reqA[] = { -1, -1 };           
            const int* resA = kd.nearestPointInEuclidianMetric(reqA);
            const int iExpectedA = 0;
            EXPECT_TRUE(resA[0] == points[iExpectedA][0] && resA[1] == points[iExpectedA][1]);

            int reqB[] = { 14, 1 };
            const int* resB = kd.nearestPointInEuclidianMetric(reqB);
            const int iExpectedB = 3;
            EXPECT_TRUE(resB[0] == points[iExpectedB][0] && resB[1] == points[iExpectedB][1]);

            {
                const int* resB = kd.nearestPointInEuclidianMetric(reqB, false);
                const int iExpectedB = 3;
                EXPECT_TRUE(resB[0] == points[iExpectedB][0] && resB[1] == points[iExpectedB][1]);
                EXPECT_TRUE(kd.sizeOfDisabledPoints() == 1);
                EXPECT_TRUE(kd.sizeOfEnablePoints() == 3);
            }
            {
                const int* resB = kd.nearestPointInEuclidianMetric(reqB, false);
                const int iExpectedB = 1;
                EXPECT_TRUE(resB[0] == points[iExpectedB][0] && resB[1] == points[iExpectedB][1]);
                EXPECT_TRUE(kd.sizeOfDisabledPoints() == 2);
                EXPECT_TRUE(kd.sizeOfEnablePoints() == 2);
            }
            {
                const int* resB = kd.nearestPointInEuclidianMetric(reqB, false);
                const int iExpectedB = 0;
                EXPECT_TRUE(resB[0] == points[iExpectedB][0] && resB[1] == points[iExpectedB][1]);
                EXPECT_TRUE(kd.sizeOfDisabledPoints() == 3);
                EXPECT_TRUE(kd.sizeOfEnablePoints() == 1);
            }
            {
                const int* resB = kd.nearestPointInEuclidianMetric(reqB, false);
                const int iExpectedB = 2;
                EXPECT_TRUE(resB[0] == points[iExpectedB][0] && resB[1] == points[iExpectedB][1]);
                EXPECT_TRUE(kd.sizeOfDisabledPoints() == 4);
                EXPECT_TRUE(kd.sizeOfEnablePoints() == 0);

                EXPECT_TRUE(kd.nearestPointInEuclidianMetric(reqB, false) == nullptr);
                EXPECT_TRUE(kd.sizeOfDisabledPoints() == 4);
                EXPECT_TRUE(kd.sizeOfEnablePoints() == 0);
                EXPECT_TRUE(kd.size() == 4);
            }

            kd.makeAllPointsEnable();
            EXPECT_TRUE(kd.sizeOfDisabledPoints() == 0);

            int bbA[][2] = { { 0, 0 }, { 20, 20 } };
            std::vector<lw_index_datastructs::KDtree<int, 2, double>::TPointerToTCoordinates> bbA_res;
            kd.rangeSearchWithBoundingBox(bbA_res, bbA[0], bbA[1]);

            EXPECT_TRUE(bbA_res.size() == 4);
            for (size_t i = 0; i < bbA_res.size(); ++i)
                EXPECT_TRUE(bbA_res[i] != nullptr);

            int bbB[][2] = { { -1, -1 }, { 1, 20 } };
            std::vector<lw_index_datastructs::KDtree<int, 2, double>::TPointerToTCoordinates> bbB_res;
            kd.rangeSearchWithBoundingBox(bbB_res, bbB[0], bbB[1]);
            EXPECT_TRUE(bbB_res.size() == 2);
            for (size_t i = 0; i < bbB_res.size(); ++i)
                EXPECT_TRUE(bbB_res[i] != nullptr);

            kd.removeAll();
            std::vector<lw_index_datastructs::KDtree<int, 2, double>::TPointerToTCoordinates> bbC_res;
            kd.rangeSearchWithBoundingBox(bbC_res,bbB[0], bbB[1]);
            EXPECT_TRUE(bbC_res.empty());
        }
    }
}
