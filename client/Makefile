CXX = clang++
SOURCES = $(shell find . -name '*.cpp')
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = client

CPPFLAGS = -std=c++11 -O2 -c 
LDFLAGS+=-lboost_system -pthread 

all: $(OBJECTS) $(TARGET)
	
.cpp.o: *.hpp
	$(CXX) $(CPPFLAGS) $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
