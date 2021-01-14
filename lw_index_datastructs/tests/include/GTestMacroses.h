#pragma once

#include "gtest/gtest.h"
#include <iostream>
#include <sstream>
#include <ctype.h>

inline bool& gRefAllowLogRecordPropertyToStdout()
{
    static bool allowLogToStdout = true;
    return allowLogToStdout;
}

inline bool gIsAllowLogRecordPropertyToStdout()
{
    return gRefAllowLogRecordPropertyToStdout();
}

inline void gSetupAllowLogRecordPropertyToStdout(bool allow)
{
    gRefAllowLogRecordPropertyToStdout() = allow;
}

inline std::string gPrintNumber(size_t number)
{
    std::stringstream tmp;
    if ( (number / 1000000) * (1000000) == number)
        tmp << number / (1000000) << "M";
    else if ( (number / (1000)) * (1000) == number)
        tmp << number / (1000) << "K";
    else
        tmp << number;
    return tmp.str(); 
}

inline std::string gCleanXmlAtribute(std::string name)
{
    name = "__"  + name + "____";
    // replace all not alphabetic and numeric characters to digit
    for (size_t i = 0; i < name.size(); ++i)
    {
        if (!isalnum(name[i]))
            name[i] = '_';
    }

    return name;
}

template <class T>
inline void gProxiedRecordProperty(const std::string& key, const T& value) 
{
    std::stringstream strTemp;
    strTemp << /* key << ":" <<*/ value;

    std::string cleanKey = /*std::string("msg_") +*/ gCleanXmlAtribute(key);
    ::testing::Test::RecordProperty(cleanKey, strTemp.str());
	
    if (gIsAllowLogRecordPropertyToStdout()) {         
        std::cout << std::endl
                  << ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name()
                  << "."
                  << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << key << "=> " << (strTemp.str()) << std::endl;
    }
}

inline void gProxiedRecordPerf(const std::string& experimentName, size_t iterations, size_t elements, double milliseconds)
{
    std::stringstream strTemp;
    strTemp << "repeat test " << gPrintNumber(iterations) << " wtih " << gPrintNumber(elements) << " elements takes";
    strTemp << ">> " << milliseconds << " milliseconds";

    std::string cleanKey = /*std::string("msg_") +*/ gCleanXmlAtribute(experimentName);
    ::testing::Test::RecordProperty(cleanKey, strTemp.str());

    if (gIsAllowLogRecordPropertyToStdout()) {
        std::cout << std::endl
                  << ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name()
                  << "."
                  << ::testing::UnitTest::GetInstance()->current_test_info()->name()
                  << experimentName << "=> " << (strTemp.str()) << std::endl;
    }
}
