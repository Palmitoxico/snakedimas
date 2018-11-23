#ifndef INPUT_HPP_
#define INPUT_HPP_

#include <ncurses.h>

namespace input
{
    /*
     * Class responsible for manage the input from the keyboard
     */
    class Keyboard
    {
    public:
        Keyboard();
        chtype getChar();
    };
}

#endif
