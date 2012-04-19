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
INCPATH =  -I$$GTEST_DIR/include

LDFLAGS = -m$(arch)

GTEST_LIB = -L$$GTEST_DIR/linux/$(arch) -lgtest -lpthread

SOURCES = UnitTest.cpp

OBJS = $(SOURCES:.cpp=.o)

TARGET = $(arch)/Test

ifeq ($(mode), release)
	CXX_FLAGS += -O2 -Wuninitialized
else
	CXX_FLAGS += -ggdb
endif

.PHONY:all
all: $(SOURCES) $(TARGET)
ifeq ($(mode), release)
	#strip $(TARGET)
endif

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) $(OBJS) $(GTEST_LIB) -o $@ $(COV_LFLAGS)

.cpp.o:
	$(CXX) -c $(CXX_FLAGS) $(INCPATH) $< -o $@

.PHONY:clean
clean:
	find . -name "*.o" | xargs rm -f
	rm -f *.bb *.bbg *.da *.gcno *.gcda *.info
	rm -f $(TARGET)
