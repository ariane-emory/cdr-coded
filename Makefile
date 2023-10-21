UNAME_S = $(shell uname -s)
CXXLAGS  = \
	-std=c++23 \
	-Iinclude \
	-O3 \
	-ggdb \
	-ffunction-sections \
	-fno-exceptions \
	-funroll-loops \
	-fdata-sections \
	-ffunction-sections \
	-Wfatal-errors \
  -Wno-terminate \
	-Wno-format \
	-Wall \
	-Wextra \
	-Wshadow \
	-Wpedantic \
  -Wconversion \
	-Wno-unused-function \
	-Wno-unused-parameter \
	-DRESEUNE_SINGLETON_ALLOCATOR \
	-DRESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION \
	-DRESEUNE_USE_TOKENIZER_SPAN_WITH_CHILDREN \
	-DWITH_RESEUNE_POOL \
	-DRESEUNE_USE_ALLOC_INFO_WITH_UNFREE_FLAG

ifeq ($(UNAME_S),Darwin)
	CXX = g++-13
endif

GDB      = gdb
OBJDUMP  = objdump
SRC      = $(shell find src -name "*.cpp")
OBJ      = $(patsubst src/%.cpp, tmp/%.o, $(SRC))
BIN      = cdr-coded

all:: $(BIN) 

tmp/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CXXLAGS)

$(BIN): $(OBJ)
	$(CXX) -o $@ $^ $(CXXLAGS) $(LDFLAGS)

clean::
	rm -rf $(BIN) tmp

test: clean all
	./$(BIN)

debug: clean all
	$(GDB) ./$(BIN)

dump: clean tmp/main.o $(BIN)
	$(OBJDUMP) -t -d tmp/main.o
