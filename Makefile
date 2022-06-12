UNAME_S = $(shell uname -s)

LDFLAGS = -Wl,--gc-sections

CXXLAGS  = \
	-std=c++23 \
	-Iinclude \
	-O1 \
	-ggdb \
	-fno-exceptions \
	-funroll-loops \
	-fdata-sections \
	-ffunction-sections \
  -Wno-terminate \
	-Wno-format \
	-Wall \
	-Wextra \
	-Wshadow \
	-Wpedantic \
  -Wconversion \
	-Wno-unused-function \
	-Wno-unused-parameter \
	-DRESEUNE_CELL_LAST_ELEMENT_OPTIMIZATION \
	-DWITH_RESEUNE_POOL

ifeq ($(UNAME_S),Darwin)
	CXX = g++-12
else
	CXX = g++-11
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

dump: clean tmp/main.o
	$(OBJDUMP) -t -d tmp/main.o

blah:
	echo $(UNAME)
