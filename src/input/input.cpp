#include "input.hpp"

namespace input
{
    Keyboard::Keyboard()
    {
        cbreak();                /* Line buffering disabled	*/
        nodelay(stdscr, FALSE);
        keypad(stdscr, TRUE);    /* We get F1, F2 etc..		*/
        noecho();			     /* Don't echo() while we do getch */
        curs_set(0);             /* Do not display cursor */
    }
    
    chtype Keyboard::getChar()
    {
        return getch();
    }
}
