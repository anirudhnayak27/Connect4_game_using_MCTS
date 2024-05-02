CC = g++
CFLAGS = -std=c++11 -Wall
SRC_DIR = src
BUILD_DIR = build
EXECUTABLE = connect_four

# List of source files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

# List of object files
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Default target
all: $(EXECUTABLE)

# Rule to compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to link object files into executable
$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Clean target
clean:
	rm -rf $(BUILD_DIR)/*.o $(EXECUTABLE)
