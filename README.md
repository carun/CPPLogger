CPPLogger
=========

A simple and efficient header-only logger library.

About
-----

The library is currently tested with GCC 4.4.6 and 4.7.2 on Linux only.

Usage
-----

Initialize the library with the location of the log file, log filename, maximum
size of the log file, number of files after which you want it to shred the
oldest file, buffering size. Look at CPPLogger.cpp for the usage.
