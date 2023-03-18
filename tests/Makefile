OUTPUT_DIR = bin
INPUT_DIR = tests_asm

TEST_SRCS = lda.s 
TEST_BINS = $(basename $(TEST_SRCS))

ASSEMBLER = vasm6502_oldstyle

all: tester $(TEST_BINS:%=$(OUTPUT_DIR)/%)

tester: tester.c ../src/6502.c $(OUTPUT_DIR)
	gcc tester.c ../src/6502.c -o $(OUTPUT_DIR)/tester

$(OUTPUT_DIR):
	mkdir $(OUTPUT_DIR)

$(OUTPUT_DIR)/%: $(INPUT_DIR)/%.s
	$(ASSEMBLER) -Fbin -dotdir $< -o $@

clear: 
	rm -r $(OUTPUT_DIR)	

