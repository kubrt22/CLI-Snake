CXX = g++

CXXFLAGS = -std=c++20 -Wall -Wextra -g -O0
INCLUDES =
LDFLAGS =
LIBS = -lncurses

TARGET = main

SRCS = $(wildcard *.cpp)

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) -o $(TARGET) $(SRCS) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) $(LIBS)

clean:
	$(RM) $(TARGET)