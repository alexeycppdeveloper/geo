CXX = clang++
SOURCES = $(shell find . -name '*.cpp')
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = server

CPPFLAGS = -std=c++11 -O2 -c -MMD 
LDFLAGS = -lboost_system -pthread 
DEPS = *.d

-include $(DEPS)

all: $(OBJECTS) $(TARGET)
	
.cpp.o: $(shell find . -name '*.hpp')
	$(CXX) $(CPPFLAGS) $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(DEPS)
