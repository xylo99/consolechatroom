#!/bin/bash
docker run -it --name cli$1_cr cli_chatroom_plat ash -c "/opt/bin/client $2 $3"
#docker exec cli$1_cr /opt/bin/client $2 $3
