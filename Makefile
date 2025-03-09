CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++11 -Iinclude -O2
EXEC := main
MAIN := $(EXEC).cpp

SRC_FILES := $(wildcard src/**/*.cpp) $(wildcard src/*.cpp)
MAIN_OBJ := obj/$(MAIN:.cpp=.o)
OBJ_FILES := $(patsubst src/%.cpp, obj/%.o, $(SRC_FILES)) $(MAIN_OBJ)

all: $(EXEC)

$(EXEC): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

obj/%.o: src/%.cpp include/%.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(MAIN_OBJ): $(MAIN)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf obj $(EXEC)

.PHONY: all clean

