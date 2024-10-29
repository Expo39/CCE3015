# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -I../shared/jbutil -I../ -Wno-unused-parameter

# Debug build flags
DEBUG_FLAGS = -g -O0 -DDEBUG -Wall -Wextra -Wpedantic

# Release build flags
RELEASE_FLAGS = -O3 -DNDEBUG -Wall -Wextra -Wpedantic

# Target executable names
DEBUG_TARGET = ./debug
RELEASE_TARGET = ./DWT

# Source files
SRCS = main.cpp io.cpp DWT.cpp 

# Object files
DEBUG_OBJS = $(patsubst %.cpp, build/%_debug.o, $(SRCS))
RELEASE_OBJS = $(patsubst %.cpp, build/%_release.o, $(SRCS))

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

# Create symbolic link for debug executable
debug_link: $(DEBUG_TARGET)
	ln -sf $(DEBUG_TARGET) ./debug

# Create symbolic link for release executable
release_link: $(RELEASE_TARGET)
	ln -sf $(RELEASE_TARGET) ./DWT

# Compile source files into debug object files
build/%_debug.o: %.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -c $< -o $@

# Compile source files into release object files
build/%_release.o: %.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf build debug DWT

.PHONY: all debug release clean
