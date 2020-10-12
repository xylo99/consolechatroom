# ifndef UI_H
# define UI_H

# include <ncurses.h>
# include <unistd.h>
# include <stdlib.h>
# include <unistd.h> /* close */
# include <string.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <netinet/in.h>

# define TEXT_FMT 1
# define Y_TEXT_BOX_START 3
# define X_TEXT_BOX_START 2

# define ALT_KEY_BACKSPACE 127
# define CHAR_LIMIT 196
# define ALT_KEY_ENTER 10
# define RCV_LIMIT 1024

# define CHAR_TO_INT(c) (c - '0')

typedef struct window_props
{
  int parent_xmax, parent_ymax, tbox_xmax, tbox_ymax;
  int box_size;
  int tbox_last_line;
  int backspace_mode;
} window_props;

typedef struct buffers
{
  int user_char_buff[CHAR_LIMIT];
  char u_c_buff_cpy[CHAR_LIMIT];
  int16_t ucb_index;
  int user_char_buff_size;
} buffers;

typedef struct char_props
{
  int new_chr_xpos;
  int first_char_xstart;
  int first_char_ystart;
  int space_val;
} char_props;

void draw_borders(WINDOW *screen);
int char_not_at_corner(int char_xpos);
char * execute_ui();

# endif