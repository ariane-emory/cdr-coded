CXX      = g++-11
CXXLAGS  = -std=gnu++20 -Iinclude -O0 -flto -g -Wno-terminate -DRESEUNE_THROW #-DNDEBUG
GDB      = gdb

SRC      = $(shell find src -name "*.cpp")
OBJ      = $(patsubst src/%.cpp, tmp/%.o, $(SRC))
BIN      = cdr-coded

all: $(BIN)

tmp/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CXXLAGS)

$(BIN): $(OBJ)
	$(CXX) -o $@ $^ $(CXXLAGS)

clean:
	rm -rf $(BIN) tmp

test: clean all
	./$(BIN)
 
debug: clean all
	$(GDB) ./$(BIN)

