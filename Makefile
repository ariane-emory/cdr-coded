CXX = g++-11
CXXLAGS  = \
	-std=c++20 \
	-Iinclude \
	-O1 \
	-ggdb \
	-fno-exceptions \
	-funroll-loops \
  -Wno-terminate \
	-DRESEUNE_WARN #-DRESEUNE_THROW #-DNDEBUG
# -DRESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION \

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
	gobjdump -t -d tmp/main.o
#	objdump --section-headers --line-numbers --disassemble -t -C -d -S tmp/main.o

blah:
	echo $(UNAME)
