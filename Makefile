CC := g++ -Wno-pointer-arith -Wwrite-strings
DIRFRONT := ../BinaryTranslator/frontend
DIRBACK := ../BinaryTranslator/backend
DIRMIDDLE := ../BinaryTranslator/middleend
DIRFUNC := ../BinaryTranslator/functions
SRC := main.cpp $(DIRFRONT)/frontend.cpp $(DIRFUNC)/text-sort.cpp $(DIRFUNC)/diftree.cpp $(DIRBACK)/ir.cpp $(DIRBACK)/elfwrite.cpp  $(DIRBACK)/backend.cpp $(DIRBACK)/translator.cpp $(DIRFUNC)/stack.cpp $(DIRMIDDLE)/middleend.cpp
DIRB := ../BinaryTranslator/lib
DIRCPU := ../Processor
DIRASM := ../Assembler
DIRTEST := ../BinaryTranslator/tests
SCRTEST := $(DIRTEST)/testmain.cpp
SRCCPU := $(DIRCPU)/main.cpp $(DIRCPU)/processor.cpp $(DIRCPU)/stack.cpp $(DIRCPU)/text-sort.cpp
SRCASM := $(DIRASM)/main.cpp $(DIRASM)/assembler.cpp $(DIRASM)/stack.cpp $(DIRASM)/text-sort.cpp
NASM := nasm -f elf64 -l
LD := ld -s -o

.PHONY: all clean

main: $(SRC)
	$(CC) $^ -o $@ -fsanitize=address,bool,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr

cpu: $(SRCCPU)
	$(CC) $^ -o $@

asm: $(SRCASM)
	$(CC) $^ -o $@

test: $(DIRTEST)/testmain.cpp
	$(CC) $(DIRTEST)/testmain.cpp -o $@

cmd:
	sudo $(NASM) commands.lst commands.asm
	sudo $(LD) commands commands.o

scanf:
	sudo $(NASM) -f bin binscanf scanf.asm

print:
	sudo nasm -f bin printf.asm -l binprint

clean:
	rm -rf *.png *.dot *.o
