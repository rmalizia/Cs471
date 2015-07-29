CC		=	gcc 
RM		= rm -rf 
CFLAGS		= $(NORMCFLAGS)	
LIBS       = -lnsl  -lm -lrt -lpthread

PROGS = PEPRA 

all: $(PROGS)

PRABA:  PEPRA.o
	$(CC) $(CFLAGS) -o PEPRA PEPRA.c $(LIBS)
clean:;
	$(RM) $(PROGS)  *.o core 