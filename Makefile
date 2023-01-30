CXX		  	:= g++
EXECUTABLE	:= main

BIN_DIR		:= bin
SRC_DIR		:= src

SRCS 		:= $(shell find $(SRC_DIR) -name *.cpp -or -name *.c -or -name *.s)
OBJS 		:= $(SRCS:%=$(BIN_DIR)/%.o)
DEPS 		:= $(OBJS:.o=.d)

INC_DIRS 	:= $(shell find $(SRC_DIR) -type d)
INC_FLAGS 	:= $(addprefix -I,$(INC_DIRS))

CXX_FLAGS 	:= $(INC_FLAGS) -Wall -Wextra -std=c++17 -ggdb -MMD -MP #-O3
LIBRARIES	:= -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network


all: $(BIN_DIR)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN_DIR)/$(EXECUTABLE)

$(BIN_DIR)/$(EXECUTABLE): $(OBJS)
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LIBRARIES)

$(BIN_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) -c $< -o $@

.PHONY: clean
clean:
	-rm -r $(BIN_DIR)/

-include $(DEPS)
