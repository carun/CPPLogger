/*
 * Copyright 2012 Arun Chandrasekaran <visionofarun@gmail.com>
 *
 * This file is part of CPPLogger.
 *
 * CPPLogger is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CPPLogger is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CPPLogger.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include "CPPLogger.hpp"

#define UNUSED __attribute__((unused))
void *func(void UNUSED *arg)
{
   std::string ver = CPPLog::GetVersion();
   LogTrace(("Logger version: %s", ver.c_str()));
   for (int i = 0; i < 1000; i++)
   {
      LogTrace(("Hasdads12938109381038910398102398123091823098: %d", i));
      LogStamp(("Hasdads12938109381038910398102398123091823098: %d", i));
      LogInfo(("Hasdads12938109381038910398102398123091823098: %d", i));
      LogWarn(("Hasdads12938109381038910398102398123091823098: %d", i));
      LogError(("Hasdads12938109381038910398102398123091823098: %d", i));
      LogFatal(("Hasdads12938109381038910398102398123091823098: %d", i));
   }
   pthread_exit(0);
}

void threaded_test(void)
{
   pthread_t tid1, tid2;
   pthread_create(&tid1, NULL, func, NULL);
   pthread_create(&tid2, NULL, func, NULL);
   pthread_join(tid1, NULL);
   pthread_join(tid2, NULL);
}

void test_trace(int loop_cnt)
{
   for (int i = 0; i < loop_cnt; i++)
      LogTrace(("Hasdads12938109381038910398102398123091823098"));
}

void test_stamp(int loop_cnt)
{
   for (int i = 0; i < loop_cnt; i++)
      LogStamp(("Hasdads12938109381038910398102398123091823098"));
}

void test_info(int loop_cnt)
{
   for (int i = 0; i < loop_cnt; i++)
      LogInfo(("Hasdads12938109381038910398102398123091823098"));
}

void test_warn(int loop_cnt)
{
   for (int i = 0; i < loop_cnt; i++)
      LogWarn(("Hasdads12938109381038910398102398123091823098"));
}

void test_error(int loop_cnt)
{
   for (int i = 0; i < loop_cnt; i++)
      LogError(("Hasdads12938109381038910398102398123091823098"));
}

void test_fatal(int loop_cnt)
{
   for (int i = 0; i < loop_cnt; i++)
      LogFatal(("Hasdads12938109381038910398102398123091823098"));
}

int main()
{
   std::string logPath = "log";
   std::string logFileName = "Test.log";

   CPPLog::Logger::Init(logPath, // Path where to log
                        logFileName, // Log file name
                        1024*1024, // Max size of each log file
                        1024, // Max buffer size
                        10, // Max number of files after which rotation starts
                        CPPLog::Trace); // Log level

   LogTrace(("Trace Test"));
   test_trace(1000);
   LogStamp(("Stamp Test"));
   test_stamp(1000);
   LogInfo(("Info Test"));
   test_info(1000);
   LogWarn(("Warn Test"));
   test_warn(1000);
   LogError(("Error Test"));
   test_error(1000);
   LogFatal(("Fatal Test"));
   test_fatal(1000);
   LogTrace(("Multithreaded Test"));
   threaded_test();

   CPPLog::Logger::Destroy();
   return EXIT_SUCCESS;
}
