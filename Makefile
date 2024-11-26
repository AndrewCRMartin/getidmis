EXE    = getidmis
OFILES = getidmis.o
LIBS   = -lgen
CC     = cc -I$(HOME)/include -L$(HOME)/lib -ansi -pedantic -Wall -Wno-format-overflow



.c.o :
	$(CC) -c -o $@ $<

$(EXE) : $(OFILES)
	$(CC) -o $(EXE) $(OFILES) $(LIBS)

clean :
	\rm -f $(EXE) $(OFILES)
