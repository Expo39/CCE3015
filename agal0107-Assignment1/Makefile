# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Iinclude -Iinclude/utilities

# Debug build flags
DEBUG_FLAGS = -g -O0 -DDEBUG -Wall -Wextra -Wpedantic

# Release build flags
RELEASE_FLAGS = -O3 -DNDEBUG -Wall -Wextra -Wpedantic

# Target executable names
DEBUG_TARGET = DEBUG
RELEASE_TARGET = DWT

# Source files
SRCS = src/main.cpp src/io/io.cpp src/filters/filters.cpp src/DWT.cpp src/convolve.cpp

# Object files
DEBUG_OBJS = $(addprefix build/debug/, $(notdir $(SRCS:.cpp=.o)))
RELEASE_OBJS = $(addprefix build/release/, $(notdir $(SRCS:.cpp=.o)))

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
build/debug/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -c $< -o $@

# Compile source files into release object files
build/release/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf build $(DEBUG_TARGET) $(RELEASE_TARGET)

.PHONY: all debug release clean