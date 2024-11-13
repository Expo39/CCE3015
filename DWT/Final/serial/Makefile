# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -I../shared/jbutil -I../ -Wno-unused-parameter

# Debug build flags
DEBUG_FLAGS = -g -O0 -DDEBUG -Wall -Wextra -Wpedantic

# Release build flags
RELEASE_FLAGS = -O3 -DNDEBUG -Wall -Wextra -Wpedantic

# Target executable names
DEBUG_TARGET = build/DEBUG
RELEASE_TARGET = build/DWT

# Source files
SRCS = main.cpp ../io/io.cpp ../filters/filters.cpp DWT.cpp convolve.cpp inverse.cpp

# Object files
DEBUG_OBJS = $(patsubst %.cpp, build/debug/%.o, $(notdir $(SRCS)))
RELEASE_OBJS = $(patsubst %.cpp, build/release/%.o, $(notdir $(SRCS)))

# Default target
all: release

# Debug build target
debug: $(DEBUG_TARGET)

# Release build target
release: $(RELEASE_TARGET)

# Link the debug target executable
$(DEBUG_TARGET): $(DEBUG_OBJS)
	$(CXX) -o $@ $^

# Link the release target executable
$(RELEASE_TARGET): $(RELEASE_OBJS)
	$(CXX) -o $@ $^

# Compile source files into debug object files
build/debug/%.o: %.cpp
	@mkdir -p build/debug
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -c $< -o $@

build/debug/%.o: ../io/%.cpp
	@mkdir -p build/debug
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -c $< -o build/debug/$*.o

build/debug/%.o: ../filters/%.cpp
	@mkdir -p build/debug
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -c $< -o build/debug/$*.o

# Compile source files into release object files
build/release/%.o: %.cpp
	@mkdir -p build/release
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -c $< -o $@

build/release/%.o: ../io/%.cpp
	@mkdir -p build/release
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -c $< -o build/release/$*.o

build/release/%.o: ../filters/%.cpp
	@mkdir -p build/release
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -c $< -o build/release/$*.o

# Clean up build artifacts
clean:
	rm -rf build

.PHONY: all debug release clean