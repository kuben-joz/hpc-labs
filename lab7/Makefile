#
# A template for the 2016 MPI lab at the University of Warsaw.
# Copyright (C) 2016, Konrad Iwanicki.
# Further modifications by Krzysztof Rzadca 2018
#

CC          := mpic++   # use cc on okeanos
CFLAGS      := -O3 -c -Wall -g
LFLAGS      := -O3 -g
# Add new targets below:
ALL         := ring.exe benchmark.exe

all : $(ALL)


%.exe : %.o
	$(CC) $(LFLAGS) -o $@ $<


%.o : %.c
	$(CC) $(CFLAGS) $<

clean :
	rm -f *.o *.out *.err $(ALL)

