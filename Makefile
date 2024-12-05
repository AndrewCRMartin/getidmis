EXE    = getidmis
OFILES = getidmis.o
LIBS   = -lgen -lefence
CFLAGS = -ansi -pedantic -Wall -Wno-format-overflow
CC     = cc $(CFLAGS) -I$(HOME)/include -L$(HOME)/lib 

.c.o :
	$(CC) -c -o $@ $<

$(EXE) : $(OFILES)
	$(CC) -o $(EXE) $(OFILES) $(LIBS)

clean :
	\rm -f $(OFILES)

distclean : clean
	\rm -f $(EXE)
