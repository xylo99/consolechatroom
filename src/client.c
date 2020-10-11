# include "ui.h"

void draw_borders(WINDOW *screen) 
{
  int x, y, i;

  getmaxyx(screen, y, x);

  start_color();
  init_pair(TEXT_FMT, COLOR_BLACK, COLOR_WHITE);
  wattron(screen, COLOR_PAIR(TEXT_FMT));

  // Text box corners
  mvwprintw(screen, 0, 0, " ");
  mvwprintw(screen, y - 1, 0, " ");
  mvwprintw(screen, 0, x - 1, " ");
  mvwprintw(screen, y - 1, x - 1, " ");

  // Text box left and right sides
  for (i = 1; i < (y - 1); i++) 
  {
    mvwprintw(screen, i, 0, "|");
    mvwprintw(screen, i, x - 1, "|");
  }

  // Text box top and bottom sides 
  for (i = 1; i < (x - 1); i++) 
  {
    mvwprintw(screen, 0, i, "_");
    mvwprintw(screen, y - 1, i, "_");
  }

  wattroff(screen, COLOR_PAIR(TEXT_FMT));
}

int char_not_at_corner(int char_xpos)
{
  // ensure that the current char x coordinate is not at the ow of the
  // textbox.
  return !((char_xpos + X_TEXT_BOX_START) == (X_TEXT_BOX_START));
}

int main(int argc, char *argv[])
{
  window_props wp = {
    .parent_xmax = 0,
    .parent_ymax = 0,
    .tbox_xmax = 0,
    .tbox_ymax = 0,
    .box_size = 4,
    .tbox_last_line = 0,
    .backspace_mode = 0
  };
  buffers b = {
    .user_char_buff = {0},
    .u_c_buff_cpy = {0},
    .user_char_buff_size = 0,
    .ucb_index = 0
  };
  char_props cp = {
    .new_chr_xpos = 0,
    .first_char_xstart = 0,
    .first_char_ystart = 0,
    .space_val = ' '
  };
  
  WINDOW * oa;
  WINDOW * ta;
  int input;

  // Check args.

  if(!argv[1] || !argv[2]) 
  {
    printf("Please provide network params: \n"
           " ./client IP_ADDR PORT \n");
    exit(1);
  }

  //***************************************************
  //***************************************************
  // Config Window
  //***************************************************
  //***************************************************

  initscr();
  noecho();
  curs_set(FALSE);

  getmaxyx(stdscr, wp.parent_ymax, wp.parent_xmax);

  oa = newwin(wp.parent_ymax - wp.box_size, wp.parent_xmax, 
              0, 0);
  ta = newwin(wp.box_size, wp.parent_xmax, 
              wp.parent_ymax - wp.box_size, 0);

  draw_borders(ta);

  getmaxyx(ta, wp.tbox_ymax, wp.tbox_xmax);

  cp.first_char_ystart = (wp.tbox_ymax - Y_TEXT_BOX_START);
  cp.first_char_xstart = (wp.tbox_xmax - X_TEXT_BOX_START);

  const char * srv_msg = "[FROM REMOTE]: ";
  const char * newline = "\n";
  cp.space_val = CHAR_TO_INT(' ');

  scrollok(oa, TRUE);

  //***************************************************
  // end window config ********************************
  //***************************************************


  //***************************************************
  // config networking ********************************
  //***************************************************

	struct sockaddr_in srv;
	int cli_sock, msg;
	int16_t port;

	char send_buff[CHAR_LIMIT] = {0};
	char recv_buff[RCV_LIMIT] = {0};
	cli_sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	port = atoi(argv[2]);

	srv.sin_family = AF_INET;
	srv.sin_addr.s_addr = inet_addr(argv[1]);
	srv.sin_port = htons(port);

	connect(cli_sock, (struct sockaddr *) &srv, sizeof(srv));

  //***************************************************
  // end config network *******************************
  //***************************************************

  while(1)
  {
    msg = recv(cli_sock, recv_buff, sizeof(recv_buff), 0);
    if (msg > 0)
    {
      wprintw(oa, srv_msg);
      wprintw(oa, recv_buff);
      wprintw(oa, newline);
      wrefresh(oa);
    }
    bzero(recv_buff, sizeof(recv_buff));

    if(!wp.backspace_mode && 
       (cp.new_chr_xpos + X_TEXT_BOX_START) == cp.first_char_xstart)
    {
      cp.new_chr_xpos = 0;
      cp.first_char_ystart++;
      wp.tbox_last_line++;
    }
    // Ensure that the left side of the textbox do not get deleted 
    else if(wp.backspace_mode && 
            wp.tbox_last_line &&
            ((cp.new_chr_xpos + X_TEXT_BOX_START) == X_TEXT_BOX_START))
    {
      cp.first_char_ystart--;
      wp.tbox_last_line--;
      cp.new_chr_xpos = cp.first_char_xstart - X_TEXT_BOX_START;
    }

    input = wgetch(ta);
    switch(input)
    {
      case KEY_BACKSPACE:
      case ALT_KEY_BACKSPACE:
      case '\b':
        if(char_not_at_corner(cp.new_chr_xpos))
        {
          wp.backspace_mode = 1;
          cp.new_chr_xpos--;
          mvwaddch(ta, 
                  cp.first_char_ystart, 
                  cp.new_chr_xpos + X_TEXT_BOX_START, 
                  cp.first_char_ystart);
          // Remove char from user buffer and update index and size.
          b.user_char_buff[b.ucb_index] = 0;
          b.user_char_buff_size--;
          b.ucb_index--;
        }
        break;
      case ALT_KEY_ENTER:
        wclear(ta);
        cp.new_chr_xpos = 0;
        wp.tbox_last_line = 0;
        cp.first_char_ystart = wp.tbox_ymax - Y_TEXT_BOX_START;
        draw_borders(ta);

        // Ideally, in this loop we copy the buffer to the send buffer
        // to go to the server.
        for(int i = 0; i < b.user_char_buff_size; i++)
        {
          b.u_c_buff_cpy[i] = b.user_char_buff[i];
          send_buff[i]      = b.user_char_buff[i];
        } 

        wprintw(oa, b.u_c_buff_cpy);
        wprintw(oa, newline);
        
        // send msg to server
     		send(cli_sock, send_buff, sizeof(send_buff), 0);
        // clear the copy buffer for re-use.
        bzero(b.u_c_buff_cpy, sizeof(b.u_c_buff_cpy));
        // Do the same with the send buff.
        bzero(send_buff, sizeof(send_buff));

        // reset (zero) size and index because buffer is now cleared.
        b.ucb_index = 0;
        b.user_char_buff_size = 0;
        draw_borders(ta);
        break;
      default:
        // stop adding chars if we're on the last line
        if(wp.tbox_last_line < 2) 
        {
          wp.backspace_mode = 0;
          mvwaddch(ta, 
                   cp.first_char_ystart, 
                   cp.new_chr_xpos + X_TEXT_BOX_START, 
                   input);
          cp.new_chr_xpos++;
          // increment index and size and add new char to user buffer
          b.user_char_buff[b.ucb_index] = input;
          b.ucb_index++;
          b.user_char_buff_size++;
        }
        draw_borders(ta);
        break;
    }
    wrefresh(oa); 
  	wrefresh(ta);
  }

  endwin();

  return 0;
}
