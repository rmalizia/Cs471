CC		=	gcc 
RM		= rm -rf 
CFLAGS		= $(NORMCFLAGS)	
#LIBS       = -lsocket -lnsl  -lm -lrt -lpthread
LIBS       = -lnsl  -lm -lrt -lpthread

PROGS = Parking  

all: $(PROGS)
	@chmod 755 ${PROGS}

Parking:  Parking.o
	$(CC) $(CFLAGS) -o Parking Parking.c $(LIBS)
clean:;
	$(RM) $(PROGS) *.o core 