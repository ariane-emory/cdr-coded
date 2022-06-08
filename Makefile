CXX = g++-11
CXXLAGS  = \
	-std=gnu++23 \
	-Iinclude \
	-O1 \
	-ggdb \
	-fno-exceptions \
	-funroll-loops \
  -Wno-terminate \
	-Wno-format \
	-Wall \
	-DRESEUNE_WARN \
	-DRESEUNE_POOL \
  -DRESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION
	#-DNDEBUG

GDB      = gdb
OBJDUMP  = objdump
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

dump: clean tmp/main.o
	$(GOBJDUMP) -t -d tmp/main.o

blah:
	echo $(UNAME)
