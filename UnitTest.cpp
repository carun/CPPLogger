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
   CPPLog::Logger::Destroy();
   return ret;
}
