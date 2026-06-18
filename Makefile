CXX      = g++
CXXFLAGS = -std=c++17 -Wall -I include
TARGET   = finance_manager

SRCS     = main.cpp \
           $(wildcard src/common/*.cpp) \
           $(wildcard src/model/*.cpp) \
           $(wildcard src/persistence/*.cpp) \
           $(wildcard src/auth/*.cpp) \
           $(wildcard src/transaction_ui/*.cpp) \
           $(wildcard src/budget/*.cpp) \
           $(wildcard src/report/*.cpp)

all:
	$(CXX) $(CXXFLAGS) -static $(SRCS) -o $(TARGET).exe

linux:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	del /Q $(TARGET).exe 2>nul & rm -f $(TARGET)

.PHONY: all linux clean
