﻿/********************************************************************
	created:	2012/11/29
	created:	29:11:2012   23:38
	filename: 	\QCoreUnitTest\Units\TestBase.h
	file path:	\QCoreUnitTest\Units
	file base:	TestBase
	file ext:	h
	author:		lwch

	purpose:
*********************************************************************/

#ifndef _QCORE_UNIT_TEST_TESTBASE_H_
#define _QCORE_UNIT_TEST_TESTBASE_H_

#include "../../QCore/Library/error.h"
#include "../../QCore/Library/iostream.h"
#include "../../QCore/Library/pair.h"
#include "../../QCore/Library/string.h"

#include <time.h>

namespace QLanguage
{
    using namespace Library;

    namespace UnitTest
    {
        class CaseMap
        {
        public:
            typedef bool(*testPtr)();
            typedef bool(*nameFilterPtr)(const string&);
            #define MAX_CASE_COUNT 1024
        public:
            CaseMap() : iCaseCount(0), nameFilter(NULL) {}
            ~CaseMap() {}

            void pushCase(const pair<string, testPtr>& p, const char*, size_t)
            {
                if (iCaseCount >= MAX_CASE_COUNT) throw "too many case!";
                caseArray[iCaseCount].caseName = p.first;
                caseArray[iCaseCount++].func   = p.second;
            }

            const size_t runAllCase()const
            {
                size_t iSuccessed = 0;
                for (size_t i = 0; i < iCaseCount; ++i)
                {
                    if (nameFilter)
                    {
                        if (nameFilter(caseArray[i].caseName) && caseArray[i].func()) ++iSuccessed;
                    }
                    else
                    {
                        if (caseArray[i].func()) ++iSuccessed;
                    }
                }
                return iSuccessed;
            }

            inline const size_t caseCount()const { return iCaseCount; }

            inline void clear()
            {
                iCaseCount = 0;
            }

            inline void setNameFilter(nameFilterPtr ptr)
            {
                nameFilter = ptr;
            }

            static CaseMap* getInstance()
            {
                static CaseMap caseMap;
                return &caseMap;
            }
        protected:
            size_t iCaseCount;
            struct
            {
                string  caseName;
                testPtr func;
            } caseArray[MAX_CASE_COUNT];
            nameFilterPtr nameFilter;
        };

        #define TEST_SPEED_INSERT_COUNT 10000

        #define PrintMessage(str) \
        cout.setColor(cout.lightWith(stdstream::green)); \
        cout << str << endl; \
        cout.setColor(stdstream::white);
        #define PrintInformation(str) \
        cout.setColor(cout.lightWith(stdstream::white)); \
        cout << str << endl; \
        cout.setColor(stdstream::white);
        #define PrintError(str) \
        cerr << str << endl;

        inline void printCostTimeSeconds(clock_t n)
        {
#ifdef WIN32
            PrintInformation(string::format("time cost: %f seconds", (double)n / CLOCKS_PER_SEC));
#else
            PrintInformation(string::format("time cost: %f seconds", (double)n / sysconf(_SC_CLK_TCK)));
#endif
        }

        #define TIME_START { clock_t _clock_start_ = clock()
        #define SHOW_TIME_COST PrintInformation("time cost: %ld", clock() - _clock_start_); }
        #define SHOW_TIME_COST_SECONDS printCostTimeSeconds(clock() - _clock_start_); }//PrintInformation("time cost: %f seconds", (double)(clock() - _clock_start_) / CLOCKS_PER_SEC); }

        #define TEST_ASSERT(expression, fmt, ...) \
        if (expression) throw error<const char*>(string::format(fmt, ##__VA_ARGS__).c_str(), __FILE__, __LINE__);

        #define TEST_CASE(moduleName) \
        using namespace QLanguage::UnitTest; \
        extern bool TestImpl##moduleName(); \
        extern void Case_##moduleName(); \
        struct CaseMapImpl##moduleName \
        { \
            CaseMapImpl##moduleName() \
            { \
                CaseMap::getInstance()->pushCase(pair<string, CaseMap::testPtr>(#moduleName, TestImpl##moduleName), __FILE__, __LINE__); \
            } \
        } caseMapImpl##moduleName; \
        bool TestImpl##moduleName() \
        { \
            try \
            { \
                PrintMessage(#moduleName); \
                Case_##moduleName(); \
                PrintInformation(string::format("%s run successed!", #moduleName)); \
                return true; \
            } \
            catch (const char* e) \
            { \
                PrintError(e); \
                return false; \
            } \
            catch (const error<char*>& e) \
            { \
                e.print(); \
                return false; \
            }\
            catch (const error<const char*>& e) \
            { \
                e.print(); \
                return false; \
            } \
            catch (...) \
            { \
                PrintError("unknown error"); \
                return false; \
            } \
        } \
        void Case_##moduleName()

        #define RUN_ALL_CASE CaseMap::getInstance()->runAllCase()

        #define ALL_CASE_COUNT CaseMap::getInstance()->caseCount()

        #define SET_NAME_FILTER(func) CaseMap::getInstance()->setNameFilter(func);
    }
}

#endif
