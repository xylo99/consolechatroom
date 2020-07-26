#!/bin/bash
docker image rm -f srv_chatroom_plat cli_chatroom_plat;
docker container rm -f srv_cr cli1_cr cli2_cr
rm servercontainer/server.c clientcontainer/client.c

