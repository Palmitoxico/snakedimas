/*
 * Rendering pipeline
 */

#ifndef DISPLAY_HPP_
#define DISPLAY_HPP_

#include <memory>
#include <vector>
#include <ncurses.h>
#include "../model/model.hpp"

namespace display 
{
    struct Border
    {
        chtype 	lside, rside, tside, bside, 
        tlcorner, trcorner, blcorner, brcorner;
    };
    
    class Screen 
    {
    public:
        Screen();
        ~Screen();
        void stop();
        void init();
        
        // render methods
        void render_all_snakes(std::vector<std::shared_ptr<model::Snake>>& snakes, model::Camera camera);
        void render_scenario(model::Scenario scenario, model::Camera camera);
        void clear_screen();
        
        //void update();
    };    
}

#endif
