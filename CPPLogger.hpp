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

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

namespace CPPLog {

const char * const version = "1.0.1";
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
   pthread_mutex_t m_Mutex;

public:
   Mutex()
   { pthread_mutex_init(&m_Mutex, NULL); }

   ~Mutex()
   { pthread_mutex_destroy(&m_Mutex); }

   void Lock()
   { pthread_mutex_lock(&m_Mutex); }

   void Unlock()
   { pthread_mutex_unlock(&m_Mutex); }
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
   std::string m_LogFilePath;
   std::string m_LogFileName;
   size_t m_maxFileSize;
   size_t m_maxBuffSize;
   size_t m_currBuffSize;
   uint32_t m_fileCap;

   Logger(std::string &filePath, std::string &fileName,
          const size_t fileSize, const size_t buffSize,
          const uint32_t fileCap, MessageType logLevel) :
      m_logLevel(logLevel),
      m_LogFilePath(filePath),
      m_LogFileName(fileName),
      m_maxFileSize(fileSize),
      m_maxBuffSize((buffSize > MAX_BUFF_SIZE ? MAX_BUFF_SIZE : buffSize)),
      m_currBuffSize(0),
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
   void AddToLogBuff(std::string& buff, MessageType msgLevel);
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

   static void Init(std::string &filePath, std::string &fileName,
                    const size_t fileSize, const size_t buffSize,
                    const uint32_t fileCap, MessageType logLevel)
   {

      mkdir(filePath.c_str(), 0755);
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

inline void Logger::GetDateTime(std::string& Date, std::string& Time)
{
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

   Time = strm.str();

   strm.str(std::string());
   strm << (tm1.tm_year + 1900) << "-"
      << std::setw(2) << std::setfill('0') << (tm1.tm_mon + 1) << "-"
      << std::setw(2) << std::setfill('0') << tm1.tm_mday;
      
   Date = strm.str();
}

inline void Logger::GetLogLevelString(MessageType msgType,
                                      std::string &MsgLevelStr)
{
   switch (msgType)
   {
   case Fatal:
      { MsgLevelStr = "[FATAL]"; break; }
   case Error:
      { MsgLevelStr = "[ERROR]"; break; }
   case Warn:
      { MsgLevelStr = "[ WARN]"; break; }
   case Info:
      { MsgLevelStr = "[ INFO]"; break; }
   case Stamp:
      { MsgLevelStr = "[STAMP]"; break; }
   case Trace:
      { MsgLevelStr = "[TRACE]"; break; }
   default:
      { break; }
   }
}

inline void Logger::AddToLogBuff(std::string& buff, MessageType msgLevel)
{
   if ((m_currBuffSize + buff.length()) > m_maxBuffSize)
      WriteLog();
   else if (msgLevel == Fatal || msgLevel == Error || msgLevel == Warn)
      WriteLog();

   m_buffList.append(buff);
   m_currBuffSize += buff.length();
}

inline void Logger::ShiftLog()
{
   char fname[300] = {0};
   char strSrcFName[512] = {0};
   char strDstFName[512] = {0};
   char LastFile[512] = {0};

   strcpy(fname, m_LogFileName.c_str());
   char *baseName = strtok(fname, ".");
   char *ext = strtok(NULL, ".");
   sprintf(LastFile, "%s/%s.%d.%s", m_LogFilePath.c_str(), baseName, m_fileCap, ext);

   struct stat statbuf;
   uint32_t startIdx = 1;
   if (stat(LastFile, &statbuf) == 0)
   {
      for (uint32_t i = 2; i <= m_fileCap; i++)
      {
         sprintf(strSrcFName, "%s/%s.%d.%s", m_LogFilePath.c_str(), baseName, i, ext);
         sprintf(strDstFName, "%s/%s.%d.%s", m_LogFilePath.c_str(), baseName, i - 1, ext);
         rename(strSrcFName, strDstFName);
      }
      startIdx = m_fileCap;
   }

   for (uint32_t fileNo = startIdx; fileNo <= m_fileCap; fileNo++)
   {
      char renLogFile[512] = {0};
      sprintf(renLogFile, "%s/%s.%d.%s", m_LogFilePath.c_str(), baseName, fileNo, ext);

      struct stat statbuf;
      if (stat(renLogFile, &statbuf) != 0)
      {
         std::string oldName(m_LogFilePath);
         oldName += "/";
         oldName += m_LogFileName;

         rename(oldName.c_str(), renLogFile);
         break;
      }
   }
}

inline bool Logger::WriteLog()
{
   std::string logFile;
   logFile += m_LogFilePath;
   logFile += "/";
   logFile += m_LogFileName;
   std::ofstream logStream(logFile.c_str(), std::ios::out | std::ios::app);

   if (!logStream.is_open())
   {
      std::cerr << "Unable to open log file: " << m_LogFileName.c_str();
      return false;
   }

   logStream << m_buffList;
   m_buffList.clear();
   m_currBuffSize = 0;

   logStream.flush();
   uint64_t currentSize = logStream.tellp();
   logStream.close();

   if (currentSize >= m_maxFileSize)
      ShiftLog();
   return true;
}

inline void Logger::Log(std::string& strFileName, std::string& strFuncName, int nLineNum,
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
      << "[0x" << std::hex << pthread_self() << "] " << std::dec
      << MsgLevelStr << " "
      << strFileName.c_str() << ":"
      << strFuncName.c_str() << ":"
      << nLineNum << " "
      << strMessage.c_str() << std::endl;

   std::string buff(strm.str());

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
                  int nLineNum, MessageType msgLevel, std::string strMessage)
{
   Logger *pLogger = Logger::Instance();
   if (pLogger)
   {
      std::string strFileName = fileName;
      std::string strFuncName = funcName;
      pLogger->Log(strFileName, strFuncName, nLineNum, msgLevel, strMessage);
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
