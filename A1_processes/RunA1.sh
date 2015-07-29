#! /bin/sh
echo "\n\nSharedMemory:"
echo      -------------
./SharedMemory
echo "\n\nSharedMemory Done"
echo      -------------
read x < /dev/tty
echo "\n\nSharedSocket:"
echo      -------------
./SharedSocket.py
echo "\n\nSharedSocket Done"
echo      -------------
read x < /dev/tty
echo "\n\nSharedPipe1:"
echo      ------------
./SharedPipe1.py
echo "\n\nSharedPipe1 Done"
echo      -------------
read x < /dev/tty
echo "\n\nSharedPipe2:"
echo      ------------
./SharedPipe2.py
echo "\n\nSharedPipe2 Done"
echo      -------------
read x < /dev/tty
echo "\n\nSharedFile1:"
echo      ------------
./SharedFile1 
echo "\n\nSharedFile1 Done"
echo      -------------
read x < /dev/tty
echo "\n\nSharedFile2:"
echo      ------------
./SharedFile2
echo "\n\nSharedFile2 Done"
echo      -------------
