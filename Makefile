CXX = g++

AR = ar

CXXFLAGS = -O2 -g -Wall -fmessage-length=0 -Iinclude -std=c++14

SRCS = $(shell find src/ -type f -name *.cc)

OBJS = $(SRCS:.cc=.o)

LIBS = 

LDFLAGS =

LDLIBS = -lpthread -lboost_system -lboost_log -lboost_program_options -lys_util -lys_srv -lpqxx

TARGET = ys-td

TEST_FLAGS = -O2 -g -Wall -fmessage-length=0 -Iinclude -std=c++14

TEST_SRCS = $(shell find test/ -type f -name *.cc)

TEST_OBJS = $(filter-out src/$(TARGET).o, $(OBJS))

TEST_LIBS =

TESTS = $(TEST_SRCS:.cc=.test)

CHECKS = $(TESTS:.test=.check)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(LDFLAGS) $(LDLIBS) $(OBJS) $(LIBS)

test: $(TESTS)

%.test: %.cc $(TEST_OBJS)
	    $(CXX) -o $@ $(TEST_FLAGS) $(LDFLAGS) $(LDLIBS) $^ $(LIBS)

%.check: %.test
	    $<

check: $(CHECKS)

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f $(TESTS)

.PHONY: clean all test check

