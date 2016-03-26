#!/bin/sh
echo "Begain to kill [cranetest],and restart it 3s later!"
#kill -9 'ps -ef|grep cranetest|grep -v grep|awk '{print $2}''
ps
ps -ef|grep cranetest|grep -v grep|awk '{print $1}'|xargs kill -9
sleep 5s
ps
./cranetest -qws
