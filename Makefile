OPT ?= -O3

CFLAGS   = -std=gnu11 $(OPT) -Wall -Wextra -Wpedantic -Wstrict-aliasing -static

EXEC=pkru pkru_debug

all: $(EXEC)

	
clean: 
	@rm -f $(EXEC)

cleanall: clean

