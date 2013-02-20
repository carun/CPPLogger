# Copyright 2012 Arun Chandrasekaran <visionofarun@gmail.com>
#
# This file is part of CPPLogger.
#
# CPPLogger is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# CPPLogger is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with CPPLogger.  If not, see <http://www.gnu.org/licenses/>.
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
