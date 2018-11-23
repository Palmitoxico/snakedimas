#include "input.hpp"

namespace input
{
    /*
     * Initializes the input functionalities of the ncurses
     */
    Keyboard::Keyboard()
    {
        cbreak();                /* Line buffering disabled	*/
        nodelay(stdscr, FALSE);
        keypad(stdscr, TRUE);    /* We get F1, F2 etc..		*/
        noecho();			     /* Don't echo() while we do getch */
        curs_set(0);             /* Do not display cursor */
    }
    
    /*
     * Returns a char typed by the user in the terminal
     */
    chtype Keyboard::getChar()
    {
        return getch();
    }
}
