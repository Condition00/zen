/*** includes ***/

#include <asm-generic/errno-base.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include <errno.h>

#include <ctype.h>
#include <stdio.h>

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode() {

  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
    die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | IXON); // stops <C-s>&&<C-x>
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag &= ~(CS8);
  raw.c_lflag &=
      ~(ECHO | ICANON | IEXTEN | ISIG); // icanon is a local flag and not a
                                        // input flag (byte by byte input)
                                        ////progaram will quit on 'q'
                                        /// ISIG disables <C-c> & <C-z>
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
}

/*** init ***/

int main() {
  enableRawMode();

  while (1) {
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
      die("read");
    if (iscntrl(c)) { // iscntrl tests whether a char is a control char. (non
                      // printable)
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }
    if (c == 'q')
      break;
  }
  return 0;
}
