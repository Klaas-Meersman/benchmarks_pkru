OPT ?= -O3

CFLAGS   = -std=gnu11 $(OPT) -Wall -Wextra -Wpedantic -Wstrict-aliasing -static

EXEC=pkru

all: $(EXEC)

	
clean: 
	@rm -f $(EXEC)

cleanall: clean

