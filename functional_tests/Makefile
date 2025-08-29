OPT ?= -O3

CFLAGS   = -std=gnu11 $(OPT) -Wall -Wextra -Wpedantic -Wstrict-aliasing -static

EXEC= my_program

all: $(EXEC)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<
	
clean: 
	@rm -f $(EXEC)

cleanall: clean