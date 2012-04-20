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
#include <gtest/gtest.h>
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

TEST(Logger, Multithread)
{
   pthread_t tid1, tid2;
   pthread_create(&tid1, NULL, func, NULL);
   pthread_create(&tid2, NULL, func, NULL);
   pthread_join(tid1, NULL);
   pthread_join(tid2, NULL);
}

TEST(Logger, Trace)
{
   for (int i = 0; i < 1000; i++)
      LogTrace(("Hasdads12938109381038910398102398123091823098"));
}

TEST(Logger, Stamp)
{
   for (int i = 0; i < 1000; i++)
      LogStamp(("Hasdads12938109381038910398102398123091823098"));
}

TEST(Logger, Info)
{
   for (int i = 0; i < 1000; i++)
      LogInfo(("Hasdads12938109381038910398102398123091823098"));
}

TEST(Logger, Warn)
{
   for (int i = 0; i < 1000; i++)
      LogWarn(("Hasdads12938109381038910398102398123091823098"));
}

TEST(Logger, Error)
{
   for (int i = 0; i < 1000; i++)
      LogError(("Hasdads12938109381038910398102398123091823098"));
}

TEST(Logger, Fatal)
{
   for (int i = 0; i < 1000; i++)
      LogFatal(("Hasdads12938109381038910398102398123091823098"));
}

int main(int argc, char *argv[])
{
   std::string logPath = "Log";
   std::string logFileName = "Test.log";
   testing::InitGoogleTest(&argc, argv);
   CPPLog::Logger::Init(logPath, logFileName, 1024*1024, 1024, 10,
                        CPPLog::Trace);
   int ret = RUN_ALL_TESTS();
   LogTrace((logPath));
   CPPLog::Logger::Destroy();
   return ret;
}
