CC := g++ -Wno-pointer-arith -Wwrite-strings
DIRFRONT := ../BinaryTranslator/frontend
DIRBACK := ../BinaryTranslator/backend
DIRMIDDLE := ../BinaryTranslator/middleend
DIRFUNC := ../BinaryTranslator/functions
SRC := main.cpp $(DIRFRONT)/frontend.cpp $(DIRFUNC)/text-sort.cpp $(DIRFUNC)/diftree.cpp $(DIRBACK)/ir.cpp $(DIRFUNC)/stack.cpp $(DIRMIDDLE)/middleend.cpp
DIRB := ../BinaryTranslator/lib
DIRCPU := ../Processor
DIRASM := ../Assembler
SRCCPU := $(DIRCPU)/main.cpp $(DIRCPU)/processor.cpp $(DIRCPU)/stack.cpp $(DIRCPU)/text-sort.cpp
SRCASM := $(DIRASM)/main.cpp $(DIRASM)/assembler.cpp $(DIRASM)/stack.cpp $(DIRASM)/text-sort.cpp

.PHONY: all clean

main: $(SRC)
	$(CC) $^ -o $@

clean:
	rm -rf *.png *.dot *.o
