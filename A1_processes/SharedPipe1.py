#! /usr/bin/env python

import sys
import os
import time
 
r, w = os.pipe()
pid = os.fork()

if pid == 0:
    Alice_send = "Hi brother, I am your sister Alice, my pid is " + str(os.getpid()) + "\n"
    os.write(w, Alice_send)
    time.sleep(1)
    from_bob = os.read(r, 1000000)
    print "<Alice>     Got from Bob: " + from_bob
    os._exit(0)

else:
    print "\n<Parent> Alice is born, her pid is " + str(pid)
    pid = os.fork()
    
    if pid ==0:
        from_alice = os.read(r, 1000000)
        Bob_send = "Hi sister, I am your brother Bob, my pid is " + str(os.getpid()) + "\n"
        print "<Bob>   Got from Alice: " + from_alice
        os.write(w, Bob_send)
        time.sleep(1)
        os._exit(0)
    else:
        print "<Parent> Bob is born, his pid is " + str(pid) + "\n"
        os.wait()
        print "<Parent> Alice is done"
        os.wait()
        print "<Parent> Bob is done\n"
        os._exit(0) 