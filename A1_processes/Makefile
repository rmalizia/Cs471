all: SharedMemory SharedFile1 SharedFile2
SharedMemory: SharedMemory.o
	gcc -o SharedMemory SharedMemory.o
SharedMemory.o: SharedMemory.c
	gcc -c SharedMemory.c
SharedFile1: SharedFile1.o
	gcc -o SharedFile1 SharedFile1.o
SharedFile1.o: SharedFile1.c
	gcc -c SharedFile1.c
SharedFile2: SharedFile2.o
	gcc -o SharedFile2 SharedFile2.o
SharedFile2.o: SharedFile2.c
	gcc -c SharedFile2.c
clean:
	rm *.o SharedMemory SharedFile1 SharedFile2
	
