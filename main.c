#include <unistd.h>
#include <stdlib.h>
#include <termios.h>

struct termios orig_termios;

void disableRawMode(void){
    write(STDOUT_FILENO, "\x1b[?1049l", 8);   // exit alternate screen
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode(void){
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    write(STDOUT_FILENO, "\x1b[?1049h", 8); // enter alternate screen

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main (void){
    enableRawMode();

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q')  
    {
        /* code */
    }
    return 0;
}
