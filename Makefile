CXX = g++

CXXFLAGS = -std=c++20 -s -O3
LIBS = -lncurses

TARGET = main

SRCS = $(wildcard *.cpp)

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) -o $(TARGET) $(SRCS) $(CXXFLAGS) $(LIBS)

clean:
	$(RM) $(TARGET)