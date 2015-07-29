#! /usr/bin/env python

import sys
import os
import socket
 
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)   
host = '' 
port = 10120
s.bind((host, port))
pid = os.fork()

if pid == 0:
    Alice_send = "Hi brother, I am your sister Alice, my pid is " + str(os.getpid()) + "\n"
    s.listen(1)
    conn, addr = s.accept()
    conn.send(Alice_send)
    from_bob = conn.recv(1000000)
    print "<Alice>     Got from Bob: " + from_bob
    conn.close()
    os._exit(0)

else:
    print "\n<Parent> Alice is born, her pid is " + str(pid)
    pid = os.fork()

    if pid ==0:
        Bob_send = "Hi sister, I am your brother Bob, my pid is " + str(os.getpid()) + "\n"
        bobSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
        bobSock.connect((host, port))
        from_alice = bobSock.recv(1000000)
        print "<Bob>   Got from Alice: " + from_alice
        bobSock.send(Bob_send)
        bobSock.close()
        os._exit(0)
    else:
        print "<Parent> Bob is born, his pid is " + str(pid) + "\n"
        os.wait()
        print "<Parent> Alice is done"
        os.wait()
        print "<Parent> Bob is done\n"
        os._exit(0) 