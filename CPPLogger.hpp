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
#ifndef CPPLOGGER_HPP
#define CPPLOGGER_HPP

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#ifndef WIN32
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#else
#include <sys/timeb.h>
#include <Windows.h>
#include <direct.h>
#define __func__ __FUNCTION__
#endif

namespace CPPLog {

const char * const version = "1.0.2";
const size_t MSG_SIZE = 1024*1024;
const size_t MAX_BUFF_SIZE = (512 * 1024);

enum MessageType
{
   None = 0,
   Fatal,
   Error,
   Warn,
   Info,
   Stamp,
   Trace
};

class Mutex
{
private:

#ifndef WIN32
   pthread_mutex_t m_mtx;
#else
   HANDLE m_mtx;
#endif

public:
   Mutex()
   { 
#ifndef WIN32
      pthread_mutex_init(&m_mtx, NULL); 
#else
      m_mtx = CreateMutex(NULL, FALSE, NULL);            
#endif
   }

   ~Mutex()
   { 
#ifndef WIN32
      pthread_mutex_destroy(&m_mtx); 
#else
      CloseHandle(m_mtx);
#endif
   }

   void Lock()
   { 
#ifndef WIN32
      pthread_mutex_lock(&m_mtx); 
#else
      WaitForSingleObject(m_mtx, INFINITE);
#endif
   }

   void Unlock()
   { 
#ifndef WIN32
      pthread_mutex_unlock(&m_mtx); 
#else
      ReleaseMutex(m_mtx);
#endif
   }
};

class MutexLocker
{
private:
   Mutex &m_mtx;

   void operator=(const MutexLocker&);
   MutexLocker(const MutexLocker &);

public:
   MutexLocker(Mutex &mtx) : m_mtx(mtx)
   { mtx.Lock(); }

   virtual ~MutexLocker()
   { m_mtx.Unlock(); }
};

class Logger
{
private:
   Mutex m_oMutex; /**< mutex contol instance */
   MessageType m_logLevel;
   std::string m_buffList;
   std::string m_logFilePath;
   std::string m_logFileName;
   size_t m_maxFileSize;
   size_t m_maxBuffSize;
   uint32_t m_fileCap;

   Logger(std::string& filePath, std::string& fileName,
          const size_t fileSize, const size_t buffSize,
          const uint32_t fileCap, MessageType logLevel) :
      m_logLevel(logLevel),
      m_logFilePath(filePath),
      m_logFileName(fileName),
      m_maxFileSize(fileSize),
      m_maxBuffSize((buffSize > MAX_BUFF_SIZE ? MAX_BUFF_SIZE : buffSize)),
      m_fileCap(fileCap)
   {
      m_maxBuffSize = (m_maxBuffSize > m_maxFileSize) ? 0 : m_maxBuffSize;
      m_buffList.reserve(m_maxBuffSize);
   }

   ~Logger()
   { WriteLog(); }

   // Prevent copy constructor and assignment operator
   Logger(const Logger&)
   { }
   void operator=(const Logger&)
   { }

   void GetDateTime(std::string& date, std::string& time);
   void GetLogLevelString(MessageType msgType, std::string &MsgLevelStr);
   void AddToLogBuff(const std::string& buff, MessageType msgLevel);
   bool WriteLog();
   void ShiftLog();

   // Need to adopt templates or inline function to make the library header-only.
   // This is a new way to make a singleton in a header only library.
   static Logger* MyInstance(Logger* ptr)
   {
      static Logger* myInstance = NULL;
      // I don't want to restrict ptr from being NULL - After Destroy, I would expect
      // this to be NULL for obvious reasons. But then the Instance method will not be
      // able to return this. So now the safety of the logger boils down to the maintainer.
      if (ptr)
         myInstance = ptr;
      return myInstance;
   }

public:

   static void Init(std::string& filePath, std::string& fileName,
                    const size_t fileSize, const size_t buffSize,
                    const uint32_t fileCap, MessageType logLevel)
   {

#ifndef WIN32
      mkdir(filePath.c_str(), 0755);
#else
      _mkdir(filePath.c_str());
#endif
      Logger* pInst = new Logger(filePath, fileName, fileSize,
                                 buffSize, fileCap, logLevel);
      MyInstance(pInst);
   }

   static Logger* Instance()
   { return MyInstance(NULL); }

   static void Destroy()
   {
      Logger *pInst = MyInstance(NULL);
      if (pInst != NULL)
      {
         pInst->WriteLog();
         delete pInst;
      }
   }

   MessageType LogLevel() const
   { return m_logLevel; }

   void Log(std::string& strFileName, std::string& strFuncName,
            int nLineNum, MessageType msgLevel, std::string& strMessage);
};

inline void Logger::GetDateTime(std::string& date, std::string& time_str)
{
#ifndef WIN32
   struct timeval detail_time;

   time_t long_time = 0;
   time(&long_time);                /* Get time as long integer. */
   struct tm tm1;
   localtime_r(&long_time, &tm1);

   gettimeofday(&detail_time, NULL);

   std::stringstream strm;
   strm << std::setw(2) << std::setfill('0') << tm1.tm_hour << ":"
      << std::setw(2) << std::setfill('0') << tm1.tm_min << ":"
      << std::setw(2) << std::setfill('0') << tm1.tm_sec << "."
      << std::setw(6) << detail_time.tv_usec;

   time_str = strm.str();

   strm.clear();
   strm << (tm1.tm_year + 1900) << "-"
      << std::setw(2) << std::setfill('0') << (tm1.tm_mon + 1) << "-"
      << std::setw(2) << std::setfill('0') << tm1.tm_mday;

   date = strm.str();
#else
   SYSTEMTIME systemTime;
   GetLocalTime(&systemTime);
   std::stringstream strm;
   strm << std::setw(2) << std::setfill('0') << systemTime.wHour << ":"
      << std::setw(2) << std::setfill('0') << systemTime.wMinute << ":"
      << std::setw(2) << std::setfill('0') << systemTime.wSecond << "."
      << std::setw(6) << systemTime.wMilliseconds;
   time_str = strm.str();

   strm.clear();
   strm << (systemTime.wYear) << "-" << std::setw(2) << std::setfill('0') << (systemTime.wMonth) << "-" << std::setw(2) << std::setfill('0') << systemTime.wDay;
   date = strm.str();
#endif
}

inline void Logger::GetLogLevelString(MessageType msgType,
                                      std::string& msgLevelStr)
{
   switch (msgType)
   {
   case Fatal:
      { msgLevelStr = "[FATAL]"; break; }
   case Error:
      { msgLevelStr = "[ERROR]"; break; }
   case Warn:
      { msgLevelStr = "[ WARN]"; break; }
   case Info:
      { msgLevelStr = "[ INFO]"; break; }
   case Stamp:
      { msgLevelStr = "[STAMP]"; break; }
   case Trace:
      { msgLevelStr = "[TRACE]"; break; }
   default:
      { break; }
   }
}

inline void Logger::AddToLogBuff(const std::string& buff, MessageType msgLevel)
{
   m_buffList.append(buff);

   if (m_buffList.size() > m_maxBuffSize)
      WriteLog();
   else if (msgLevel == Fatal || msgLevel == Error || msgLevel == Warn)
      WriteLog();
}

inline void Logger::ShiftLog()
{
   char fname[300] = {0};
   char strSrcFName[512] = {0};
   char strDstFName[512] = {0};
   char LastFile[512] = {0};

   strcpy(fname, m_logFileName.c_str());
   char *baseName = strtok(fname, ".");
   char *ext = strtok(NULL, ".");
   sprintf(LastFile, "%s/%s.%d.%s", m_logFilePath.c_str(), baseName, m_fileCap, ext);

   struct stat statbuf;
   uint32_t startIdx = 1;
   if (stat(LastFile, &statbuf) == 0)
   {
      for (uint32_t i = 2; i <= m_fileCap; i++)
      {
         sprintf(strSrcFName, "%s/%s.%d.%s", m_logFilePath.c_str(), baseName, i, ext);
         sprintf(strDstFName, "%s/%s.%d.%s", m_logFilePath.c_str(), baseName, i - 1, ext);
         rename(strSrcFName, strDstFName);
      }
      startIdx = m_fileCap;
   }

   for (uint32_t fileNo = startIdx; fileNo <= m_fileCap; fileNo++)
   {
      char renLogFile[512] = {0};
      sprintf(renLogFile, "%s/%s.%d.%s", m_logFilePath.c_str(), baseName, fileNo, ext);

      struct stat statbuf;
      if (stat(renLogFile, &statbuf) != 0)
      {
         std::string oldName(m_logFilePath);
         oldName += "/";
         oldName += m_logFileName;

         rename(oldName.c_str(), renLogFile);
         break;
      }
   }
}

inline bool Logger::WriteLog()
{
   std::string logFile;
   logFile += m_logFilePath;
   logFile += "/";
   logFile += m_logFileName;
   std::ofstream logStream(logFile.c_str(), std::ios::out | std::ios::app);

   if (!logStream.is_open())
   {
      std::cerr << "Unable to open log file: " << m_logFileName.c_str();
      return false;
   }

   logStream << m_buffList;
   m_buffList.clear();

   logStream.flush();
   uint64_t currentSize = logStream.tellp();
   logStream.close();

   if (currentSize >= m_maxFileSize)
      ShiftLog();
   return true;
}

inline void Logger::Log(std::string& strFileName, std::string& strFuncName, int lineNum,
                        MessageType msgLevel, std::string& strMessage)
{
   if (msgLevel > m_logLevel)
      return;

   std::string DateStr, TimeStr;
   std::string MsgLevelStr;

   MutexLocker lock(m_oMutex);

   GetDateTime(DateStr, TimeStr);
   GetLogLevelString(msgLevel, MsgLevelStr);

   std::stringstream strm;
   strm << DateStr.c_str() << " " << TimeStr.c_str() << " "
      << "[0x" << std::hex
#ifndef WIN32
      << pthread_self()
#else
      << GetCurrentThreadId()
#endif
      << "] " << std::dec
      << MsgLevelStr << " "
      << strFileName.c_str() << ":"
      << strFuncName.c_str() << ":"
      << lineNum << " "
      << strMessage.c_str() << std::endl;

   std::string buff(strm.str());

#ifdef LOG_ON_SCREEN
   std::cout << buff;
#endif
   AddToLogBuff(buff, msgLevel);
}

inline std::string ConstructMsg(std::string& msg)
{
   return msg;
}

inline std::string ConstructMsg(const char *format, ...)
{
   va_list argList;
   char buf[MSG_SIZE] = {0};
   va_start(argList, format);
   vsnprintf(buf, MSG_SIZE, format, argList);
   va_end(argList);

   std::string strBuf(buf);
   return strBuf;
}

inline void Print(const char *fileName, const char *funcName,
                  int lineNum, MessageType msgLevel, std::string strMessage)
{
   Logger *pLogger = Logger::Instance();
   if (pLogger)
   {
      std::string strFileName = fileName;
      std::string strFuncName = funcName;
      pLogger->Log(strFileName, strFuncName, lineNum, msgLevel, strMessage);
   }
}

inline std::string GetVersion()
{ return version; }

}

#define LogTrace(msg)   do { \
   using namespace CPPLog; \
   Logger *pLog = Logger::Instance(); \
   if (pLog) { \
      if (pLog->LogLevel() == None || pLog->LogLevel() < Trace) \
         break; \
      Print(__FILE__, __func__, __LINE__, Trace, ConstructMsg msg); \
   } \
} while (0)

#define LogStamp(msg)   do { \
   using namespace CPPLog; \
   Logger *pLog = Logger::Instance(); \
   if (pLog) { \
      if (pLog->LogLevel() == None || pLog->LogLevel() < Stamp) \
         break; \
      Print(__FILE__, __func__, __LINE__, Stamp, ConstructMsg msg); \
   } \
} while (0)

#define LogInfo(msg)    do { \
   using namespace CPPLog; \
   Logger *pLog = Logger::Instance(); \
   if (pLog) { \
      if (pLog->LogLevel() == None || pLog->LogLevel() < Info) \
         break; \
      Print(__FILE__, __func__, __LINE__, Info, ConstructMsg msg); \
   } \
} while (0)

#define LogWarn(msg)    do { \
   using namespace CPPLog; \
   Logger *pLog = Logger::Instance(); \
   if (pLog) { \
      if (pLog->LogLevel() == None || pLog->LogLevel() < Warn) \
         break; \
      Print(__FILE__, __func__, __LINE__, Warn, ConstructMsg msg); \
   } \
} while (0)

#define LogError(msg)   do { \
   using namespace CPPLog; \
   Logger *pLog = Logger::Instance(); \
   if (pLog) { \
      if (pLog->LogLevel() == None || pLog->LogLevel() < Error) \
         break; \
      Print(__FILE__, __func__, __LINE__, Error, ConstructMsg msg); \
   } \
} while (0)

#define LogFatal(msg)   do { \
   using namespace CPPLog; \
   Logger *pLog = Logger::Instance(); \
   if (pLog) { \
      if (pLog->LogLevel() == None || pLog->LogLevel() < Fatal) \
         break; \
      Print(__FILE__, __func__, __LINE__, Fatal, ConstructMsg msg); \
   } \
} while (0)
#endif