#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> /* close */
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CHAR_LEN (140) // use the old char count of Twitter
#define EXIT_SEQ     "^["
#define NEW_LINE     '\n'

int main(int argc, const char * args[]) {

	struct sockaddr_in srv;
	int cli_sock, msg;
	int16_t port;
	int8_t wait_on_recv; // Protocol specific variable.

	char send_buff[MAX_CHAR_LEN] = {0};
	char recv_buff[1024] = {0};
	cli_sock = socket(AF_INET, SOCK_STREAM, 0);
	port = atoi(args[2]);
    
        wait_on_recv = 0;

	srv.sin_family = AF_INET; 
	srv.sin_addr.s_addr = inet_addr(args[1]); 
	srv.sin_port = htons(port);

	connect(cli_sock, (struct sockaddr *) &srv, sizeof(srv));

	msg = recv(cli_sock, recv_buff, sizeof(recv_buff), 0);
	if(msg > 0) {
		
		printf("%s", recv_buff);
		send_buff[0] = 'c'; // send control char when first connecting
		send(cli_sock, send_buff, sizeof(send_buff), 0);
        bzero(recv_buff, sizeof(recv_buff));
		recv(cli_sock, recv_buff, sizeof(recv_buff), 0); // Chatroom status msg 
		printf("%s", recv_buff);
		
		if(recv_buff[0] == '*'){
			wait_on_recv = 1;
		}
	}

	while(1) {

		if(wait_on_recv) {

 			bzero(recv_buff, sizeof(recv_buff));
			bzero(send_buff, sizeof(send_buff));
		        send_buff[0] = ' ';     	
		 	send(cli_sock, send_buff, sizeof(send_buff), 0);
			printf("\nRecieving msg...\n");
	        msg = recv(cli_sock, recv_buff, sizeof(recv_buff), 0);		
			wait_on_recv = 0;
            if(msg > 0) {
				if(recv_buff[0] == '\n') {
					printf("\nNo new msgs...\n");
				} else {
	                printf("%s\n", recv_buff);
		        }
			}
		}

		bzero(send_buff, sizeof(send_buff)); 
        bzero(recv_buff, sizeof(recv_buff));
		printf("\n__> ");
        fgets(send_buff, MAX_CHAR_LEN, stdin);
		printf("\nsending msg...\n");
        send(cli_sock, send_buff, sizeof(send_buff), 0);
    	
    	if(strcmp(send_buff, EXIT_SEQ) == 0) {
        	break;
    	}	

		printf("\nRecieving msg...\n");
		msg = recv(cli_sock, recv_buff, sizeof(recv_buff), 0);

		if(msg > 0) { 
			printf("%s\n", recv_buff);
	
        }
    }

	close(cli_sock);	
}
