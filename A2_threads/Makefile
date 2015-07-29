all:  
	make clean
	make TCC	
	make TCS

clean:
	rm -rf TCS TCC *.o

TCS:
	gcc TCS.c -pthread -lnsl -o TCS

TCC:
	gcc TCC.c -pthread -lnsl -o TCC
