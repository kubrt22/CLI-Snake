CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -D_DEBUG -Iinclude -g

TARGET_NAME = snake

ifeq ($(OS),Windows_NT)
	CXXFLAGS += -I.
	LIBS = ./pdcurses.a 
	RM = del
	TARGET = $(TARGET_NAME).exe
else
	LIBS = -lncurses
	RM = rm -f
	TARGET = $(TARGET_NAME)
endif

SRCS = $(wildcard src/*.cpp)
OBJS = $(patsubst src/%.cpp,build/%.o,$(SRCS))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

build/%.o: src/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
ifeq ($(OS),Windows_NT)
	del $(TARGET)
	del build\*.o
else
	$(RM) $(TARGET) $(OBJS)
endif