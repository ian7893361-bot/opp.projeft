CXX      = g++
CXXFLAGS = -std=c++17 -Wall -I include
TARGET   = finance_manager
SRCS     = main.cpp src/Student.cpp src/Admin.cpp src/FileManager.cpp src/UserManager.cpp src/ReportGenerator.cpp

# Windows：產生獨立 .exe（不需額外 DLL）
all:
	@if not exist data mkdir data
	@if not exist reports mkdir reports
	$(CXX) $(CXXFLAGS) -static $(SRCS) -o $(TARGET).exe

# Linux / WSL：產生 Linux 執行檔
linux:
	mkdir -p data reports
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	del /Q $(TARGET).exe 2>nul & rm -f $(TARGET)

.PHONY: all linux clean
