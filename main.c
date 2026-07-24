/* Includes */
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>

/* Defines */
#define ENTER_ALT_SCREEN "\x1b[?1049h"
#define EXIT_ALT_SCREEN  "\x1b[?1049l"

/* Data */
struct termios orig_termios;

/* terminal */
void die(const char *s){
    perror(s);
    exit(1);
}

void disableRawMode(void){
    write(STDOUT_FILENO, EXIT_ALT_SCREEN, sizeof(EXIT_ALT_SCREEN) - 1);   // exit alternate screen
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1){
        die("tcsetattr");
    }
}

void enableRawMode(void){
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1){
        die("tcgetattr");
    }
    atexit(disableRawMode);

    write(STDOUT_FILENO, ENTER_ALT_SCREEN,  sizeof(ENTER_ALT_SCREEN) - 1); // enter alternate screen

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);  
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1){
        die("tcsetattr");
    }
}

/* Init */

int main (void){
    enableRawMode();

    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN){
            die("read");
        } 
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == 'q') { break ; }
    }
    return 0;
}
