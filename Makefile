# Copyright (c) 2013 Arun Chandrasekaran <visionofarun@gmail.com>
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use, copy,
# modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
mode = debug
arch = 64

cov  := 1
ifeq ($(cov), 0)
else
	COV_CFLAGS := -fprofile-arcs -ftest-coverage
	COV_LFLAGS := -lgcov
endif

DEFINES =

CXX_FLAGS = -m$(arch) -fPIC -Wall -W -Werror $(DEFINES) $(COV_CFLAGS)
INCPATH =

LDFLAGS = -m$(arch)

LIBS = -lpthread

SOURCES = CPPLogger.cpp

OBJS = $(SOURCES:.cpp=.o)

TARGET = CPPLogger

ifeq ($(mode), release)
	CXX_FLAGS += -O2 -Wuninitialized
else
	CXX_FLAGS += -ggdb
endif

all: $(SOURCES) $(TARGET)
ifeq ($(mode), release)
	#strip $(TARGET)
endif

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) $(OBJS) $(LIBS) -o $@ $(COV_LFLAGS)

.cpp.o:
	$(CXX) -c $(CXX_FLAGS) $(INCPATH) $< -o $@

clean:
	find . -name "*.o" | xargs rm -f
	rm -f *.bb *.bbg *.da *.gcno *.gcda *.info
	rm -f $(TARGET)

st:
	clang++ --analyze -ferror-limit=0 *.?pp $(INCPATH)

.PHONY: all clean st