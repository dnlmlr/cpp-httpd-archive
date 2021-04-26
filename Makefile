# Name of the executable
NAME = prog.run

# Main source code directory
SRC_DIR = src
# Build output directory
BUILD_DIR = build


TARGET = $(addprefix $(BUILD_DIR)/, $(NAME))

SRC = $(wildcard $(SRC_DIR)/*.cpp)

OBJ = $(addprefix $(BUILD_DIR)/, $(notdir $(SRC:.cpp=.o)))

LD_FLAGS = -g -pthread
COMPILE_FLAGS = -g -c -O3


# Build rule for the main target executable
$(TARGET): $(OBJ)
	g++ $(LD_FLAGS) -o $@ $(OBJ)

# Build rule for normal source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(COMPILE_FLAGS) -o $@ $<



.PHONY: clean
clean:
	rm $(OBJ) $(TARGET)

.PHONY: run
run: $(TARGET)
	./$(TARGET)
