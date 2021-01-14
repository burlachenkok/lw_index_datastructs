/** @file
* Algorithm for object comparison. Class, method of which are used as entry point to perform comparisons. But really it called operations ">", "=", "<" for type.
* @author konstantin.burlachenko@kaust.edu.sa
*/
#pragma once

#include <math.h>
#include <stddef.h>

namespace lw_index_datastructs
{
    /** Comparator with semantics "a > b" with total order properties:
    * - antisymmetry
    * - transitivity
    * - totality (either v<=w or w<=v or both)
    */
    template<class T>
    struct Comparator
    {
        /*
        * @return 1 if "a > b", 0 if "a==b", -1 if "a < b"
        */
        int operator()(const T& a, const T& b) const {
            if (b < a)       ///< i.e. if a > b
                return 1;
            else if (a < b)
                return -1;   ///< if a less then b
            else
                return 0;
        }
    };

    template<>
    struct Comparator<double>
    {
        int operator()(const double& a, const double& b) const {
            const double kDoubleEps = 1e-6;

            if (b <= a) // i.e. if a >= b
            {
                if (a - b < kDoubleEps)
                    return 0;
                else
                    return 1;
            }
            else /* if (a < b)*/
            {
                if (b - a < kDoubleEps)
                    return 0;
                else
                    return -1; ///< if a less then b
            }
        }
    };

    template<>
    struct Comparator<float>
    {
        int operator()(const float& a, const float& b) const {
            const float kFloatEps = 1e-6f;

            if (b <= a) // i.e. if a >= b
            {
                if (a - b < kFloatEps)
                    return 0;
                else
                    return 1;
            }
            else /* if (a < b)*/
            {
                if (b - a < kFloatEps)
                    return 0;
                else
                    return -1; ///< if a less then b
            }
        }
    };

    struct CmpHelper
    {
        static bool IsGreater(int comparatorRes) {
            return comparatorRes == 1 ? true : false;
        }
        static bool IsGreaterOrEqual(int comparatorRes) {
            return comparatorRes == 1 || comparatorRes == 0 ? true : false;
        }
        static bool IsLessOrEqual(int comparatorRes) {
            return comparatorRes == -1 || comparatorRes == 0 ? true : false;
        }
        static bool IsLess(int comparatorRes) {
            return comparatorRes == -1 ? true : false;
        }
        static bool IsEqual(int comparatorRes) {
            return comparatorRes == 0 ? true : false;
        }
        static bool IsNotEqual(int comparatorRes) {
            return comparatorRes != 0;
        }
    };

    template<class T>
    struct StringLastCharTest
    {
        bool operator()(const T* str, size_t index) const {
            return str[index] == T();
        }
    };
}
