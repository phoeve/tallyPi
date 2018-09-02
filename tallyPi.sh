#!/bin/sh
#
#    NPCC tallyPi  
#
#       2 possible commands (but NOT both):
#
#    /boot/NPCC/tallyPi/tallySend <port>            ex: /boot/NPCC/tallyPi/tallySend 5001
#            OR
#    /boot/NPCC/tallyPi/tallyRecv <port> <camera>   ex: /boot/NPCC/tallyPi/tallyRecv 5001 1
#
#            note: port must be unique to auditorium  ex: 5001, 5002, 5003
#
#
#
/boot/NPCC/tallyPi/tallySend 5001 
#
#
#/boot/NPCC/tallyPi/tallyRecv 5001 1 
