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
    using namespace model;
        
    struct Border
    {
        chtype 	lside, rside, tside, bside, 
        tlcorner, trcorner, blcorner, brcorner;
    };
    
    class Snake_Render : public Snake
    {
        
    public:
        Snake_Render(int pos_x, int pos_y, float speed, Direction direction);
        void render_snake();        
    };
    
    class Screen 
    {
        
    private:
        int height = 20;
        int width = 20;
        int cam_x;
        int cam_y;
        Border border;
        std::unique_ptr<Scenario> scenario;
        
    public:
        Screen(int start_x, int start_y);
        ~Screen();
        void stop();
        void init();
        //void create_box();
        
        // Snake methods
        void add_snake(int pos_x, int pos_y, float speed, Direction direction);
        void render_all_snakes(std::vector<Snake_Render>& snakes);
        void render_scenario(Scenario scenario);
        //void erase_box();
        void update();
    };
    
    
}

#endif
