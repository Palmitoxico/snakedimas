#include <iostream>
#include <vector>
#include "display.hpp"


namespace display 
{
    
    using namespace model;
    
//------------------------ Snake_Render ---------------------------------------
    Snake_Render::Snake_Render(int pos_x, int pos_y, float speed, Direction direction)
        :Snake(pos_x, pos_y, speed, direction){}
    
    
    void Snake_Render::render_snake()
    {
        for(int j = 0; j < this->body.size(); j++){
            mvaddch(body[j].y, body[j].x, body[j].ch);
        }
        
        refresh();
    }        
    
//-------------------------- Screen -------------------------------------

    Screen::Screen(int start_x, int start_y)
    {
        this->cam_x = start_x;
        this->cam_y = start_y;
    }
    
    Screen::~Screen()
    {
        this->stop();
    }
    
    void Screen::stop()
    {
        endwin();
    }
    
    void Screen::init()
    {
        initscr();              /* Start curses mode 		*/
        raw();                  /* Line buffering disabled	*/
        curs_set(0);            /* Do not display cursor */
    }
    
    void Screen::render_all_snakes(std::vector<Snake_Render>& snakes)
    {   
        //MAYBE ADD SOME DIFFERENT RENDERIZATIONS DEPENDING ON THE "Direction" ATTRIBUTE
        
        //For each snake in the game
        for(int i = 0; i < snakes.size(); i++){
            snakes[i].render_snake();            
        }
    }

    // WE HAVE TO MAKE SURE THAT THE MAP HAS A SIZE ALWAYS BIGGER THAN MAX{COLS,LINES}
    //ANALYSE THE CASE WHERE cam_x + LINES OU cam_y + COLS > scenario.getMapSize()
    void Screen::render_scenario(Scenario scenario)
    {
        int* aux_map = scenario.getMap();
        
        for(int i = cam_y; i < cam_y + LINES && i < scenario.getMapSize(); i++){
            for(int j = cam_x; j < cam_x + COLS && j < scenario.getMapSize(); j++){
                if(aux_map[i*scenario.getMapSize() + j])
                    mvaddch(i - cam_y,j - cam_x,'#');
            }
        }
        
        refresh();
    }
}
