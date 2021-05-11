# Name of the executable
NAME = cpphttpd.a

# Include directory
INC_DIR = inc
# Main source code directory
SRC_DIR = src
# Build output directory
BUILD_DIR = build


TARGET = $(addprefix $(BUILD_DIR)/, $(NAME))

SRC = $(wildcard $(SRC_DIR)/*.cpp)

OBJ = $(addprefix $(BUILD_DIR)/, $(notdir $(SRC:.cpp=.o)))

LD_FLAGS = -g -pthread -I$(INC_DIR)
COMPILE_FLAGS = -g -c -O3 -I$(INC_DIR)


# Build rule for the main target executable
$(TARGET): $(OBJ)
	ar rcs $@ $(OBJ)


# Build rule for normal source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(COMPILE_FLAGS) -o $@ $<


example: $(TARGET)
	g++ $(LD_FLAGS) -o build/example.run example/main.cpp $(TARGET)

.PHONY: clean
clean:
	rm $(OBJ) $(TARGET)

.PHONY: run
run: $(TARGET) example
	./build/example.run
