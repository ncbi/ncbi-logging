.PHONY : clean run

PROG = parser
SRC = $(wildcard src/*.c)
HDR = $(wildcard src/*.h)

all: $(PROG)

$(PROG) : $(SRC) $(HDR) Makefile
	gcc $(SRC) -g -O3 -lm -o $@

clean :
	@rm -rf $(PROG)

run : $(PROG)
	head -n 3 some_events.txt | ./$(PROG) --verbose > output.txt
