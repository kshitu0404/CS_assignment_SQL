CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude -Ithird_party

MYSQL_CFLAGS := $(shell mysql_config --cflags 2>/dev/null)
MYSQL_LIBS   := $(shell mysql_config --libs 2>/dev/null)

ifeq ($(strip $(MYSQL_CFLAGS)),)
MYSQL_CFLAGS := -I/usr/include/mysql -I"C:/Program Files/MySQL/MySQL Server 8.0/include"
endif

ifeq ($(strip $(MYSQL_LIBS)),)
ifeq ($(OS),Windows_NT)
MYSQL_LIBS := -L"C:/Program Files/MySQL/MySQL Server 8.0/lib" -llibmysql -lpthread
else
MYSQL_LIBS := -lmysqlclient -lpthread
endif
else
MYSQL_LIBS += -lpthread
endif

TARGET := sql_injection_lab
ifeq ($(OS),Windows_NT)
TARGET := sql_injection_lab.exe
endif

SRCS := src/main.cpp src/auth.cpp src/database.cpp
OBJS := $(SRCS:.cpp=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(MYSQL_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(MYSQL_CFLAGS) -c -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

