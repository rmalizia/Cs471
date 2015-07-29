1.  SharedMemory.c:
The parent creates a shared memory segments to be used by both Alice and Bob to exchange messages.
The parent must removes the allocated shared memory segments at the end.
 
2.  SharedSocket.py:
The parent creates a server socket and binds it to any free port. Alice uses this socket to accept 
connection from Bob.
 
3.  SharedPipe1.py:
The parent creates one pipe to be used by both Alice and Bob to exchange messages.
 
4.  SharedPipe2.py:
The parent creates two pipes one for each child to be used for exchanging messages with the other.
 
5.  SharedFile1.c:
The parent creates & opens a temporary shared file. The two children uses the open file descriptor 
to exchange their messages.
 
6.  SharedFile2.c:
The parent creates a temporary shared file. Each child independently opens the file to communicate 
with the other.