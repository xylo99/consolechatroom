#include <unistd.h> /* close */
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
/* Define global constants. */
#define MAX_CLIENTS   10
#define EXIT_SEQ      "^["
#define MAX_SIZE      255
/* Define debug. */
#define LOG_ACTIVITY  0
#define DEBUG_PRINTS(FMT, ...) do { \
	if (LOG_ACTIVITY) { \
		fprintf(stderr, "%s:%d:%s(): " FMT, __FILE__, \
			__LINE__, __func__, __VA_ARGS__); } } while (0)

int main(int argc, const char * args[]) {

	struct sockaddr_in cli_addr;
	struct timeval timeout_info;
	int srv_sock, sock_active, new_sock;
	int msg; // Recieve Message status.
	int8_t cli_count = 0; // counter for clients.

	fd_set comm_fd; // Working file descriptor is the socket that is awaiting a connection, and comm fd is the socket that holds client's data.

	char cli_buff[1024] = {0};
	int cli_sock[MAX_CLIENTS] = {0};
	uint32_t cli_addr_len = sizeof(cli_addr);

	/* At this stage, there are no clients  */
	uint8_t ctr_char_rcvd = 0;

	/* Deckare socket. */
	srv_sock = socket(AF_INET , SOCK_STREAM , 0);

	/* Listen on port. */
	listen(srv_sock, MAX_CLIENTS);

	/*
	* Define messages generated by the server.
	*/
	char welcome_msg[MAX_SIZE] = "Welcome to the chatroom that lives inside of your Terminal!\n"
	                          "Just type and press enter to send a message.\n"
	                          "CTRL-C exits the chatroom session. \n \0";

	char solo_client_msg[MAX_SIZE] = "*** Waiting on a new client to join the chatroom... ***\n \0";

	char cli_exit_msg[MAX_SIZE] = "Other client left the chatroom. \n \0";

	char room_full_msg[MAX_SIZE] = "A corresponding client is present. Happy chatting! \n \0";

	int i, j, temp, msg_sendr_idx;
	int max_sock_val; // store the maximum socket descritor value.


	while(1) {
		// Listen and queue up to the max number of clients.
		FD_ZERO(&comm_fd);
		FD_SET(srv_sock, &comm_fd);

		max_sock_val = srv_sock; // initialize maximum value.

		/* Add client sockets to the comm_fd set. */
		for(i = 0; i < MAX_CLIENTS; ++i) {
			temp = cli_sock[i];
			FD_SET(temp, &comm_fd);
			if(temp > max_sock_val) {
				max_sock_val = temp; // re-assign maximum value if needed.
			}
		}
		timeout_info.tv_sec = 3;
		timeout_info.tv_usec = 0;

		/* Block on incomming connections */
		//	sock_active = select(max_sock_val+1, &comm_fd, NULL, NULL, NULL);
		sock_active = select(max_sock_val+1, &comm_fd, NULL, NULL, &timeout_info);

		if(sock_active) {
			/* If server socket is set, then we have a new client. */
			if(FD_ISSET(srv_sock, &comm_fd)) {
				new_sock = accept(srv_sock, (struct sockaddr *) &cli_addr, &cli_addr_len);
				if(new_sock > 0) {
					++cli_count;
					write(new_sock, welcome_msg, sizeof(welcome_msg));
			    		read(new_sock, cli_buff, sizeof(cli_buff));
			    		for(i = 0; i < MAX_CLIENTS; ++i) {
						if(cli_sock[i] == 0) {
							cli_sock[i] = new_sock;
							break;
						}
					}
				} else {
					//TODO: log.
					continue;
				}
				DEBUG_PRINTS("%d\n", cli_count);
				continue;
			} else {
				/* Clients are exchanging msgs */
				for(i = 0; i < cli_count; ++i) {
					temp = cli_sock[i];
					msg_sendr_idx = i; // store the index of the msg sender
					if(FD_ISSET(temp, &comm_fd)) {
						msg = read(temp, cli_buff, sizeof(cli_buff));
						/* Handle disconnected sockets. */
						if(msg == 0) {
							--cli_count;
							close(temp);
							cli_sock[i] = 0;
							continue;
						} else {
							for(j = 0; j < cli_count; ++j) {
								temp = cli_sock[j];
								/* Broadcast message to everyone else in chat. */
								if(j != msg_sendr_idx) {
									write(temp, cli_buff, sizeof(cli_buff));
								}
							}
						}
					}
				}
				continue;
			}
		} else {
			//TODO: log
			continue;
		}
	}
	return 0; // Program shouldn't get here!
}
