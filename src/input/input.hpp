#ifndef INPUT_HPP_
#define INPUT_HPP_

#include <ncurses.h>

namespace input
{
    class Keyboard
    {
    public:
        Keyboard();
        chtype getChar();
    };
}

#endif
