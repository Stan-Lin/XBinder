#!/bin/sh	

sudo rmmod client
sudo rmmod server
make clean;
make
sudo insmod server.ko
sudo insmod client.ko
