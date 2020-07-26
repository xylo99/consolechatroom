#!/bin/bash

trap cleanup INT

function cleanup () {
	docker container rm -f cli1_cr cli2_cr srv_cr
	docker image rm -f cli_chatroom_plat srv_chatroom_plat
}

cp src/client.c ./clientcontainer
cp src/server.c ./servercontainer
cd clientcontainer && docker build -t cli_chatroom_plat . && cd ../
cd servercontainer && docker build -t srv_chatroom_plat . && cd ../
./servercontainer/startscript.sh
PORT=$(docker exec srv_cr ash -c "lsof -Pi" | awk '{print $9}' | sed 1,1d | sed -e s/*://)
IP=$(docker inspect -f "{{ .NetworkSettings.IPAddress }}" srv_cr)
x-terminal-emulator -e ./clientcontainer/startscript.sh 1 $IP $PORT &
sleep 8
x-terminal-emulator -e ./clientcontainer/startscript.sh 2 $IP $PORT
