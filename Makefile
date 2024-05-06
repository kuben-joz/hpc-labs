#
# A template for the 2016 MPI lab at the University of Warsaw.
# Copyright (C) 2016, Konrad Iwanicki.
# Further modifications by Krzysztof Rzadca 2018
#

CC := CC  # use cc on okeanos
CFLAGS := -O3 -c -Wall
LFLAGS  := -O3
CC := mpiCC  # use cc on okeanos
CFLAGS := -O3 -c -Wall
LFLAGS := -O3
# Add new targets below:
ALL         := benchmark.exe

all : $(ALL)


%.exe : %.o
	mpiCC $(LFLAGS) -o $@ $<


%.o : %.c
	mpiCC $(CFLAGS) $<

clean :
	rm -f *.o *.out *.err $(ALL)