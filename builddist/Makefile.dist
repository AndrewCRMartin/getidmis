EXE    = getidmis
OFILES = getidmis.o
LIBS   = 
CFLAGS = -g -ansi -pedantic -Wall -Wno-format-overflow
CC     = cc $(CFLAGS)
LFILES = bioplib/StoreString.o

.c.o :
	$(CC) -c -o $@ $<

$(EXE) : $(OFILES) $(LFILES)
	$(CC) -o $(EXE) $(OFILES) $(LFILES)

clean :
	\rm -f $(OFILES) $(LFILES)

distclean : clean
	\rm -f $(EXE)
