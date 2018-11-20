#include <iostream>
#include <vector>
#include "display.hpp"


namespace display 
{
    
    using namespace model;
    
//-------------------------- Screen -------------------------------------

    Screen::Screen(){}
    
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

	void Screen::print_msg(const std::string& str, int posx, int posy)
	{
		mvprintw(posy, posx,"%s", str.c_str());
        refresh();
	}

    void Screen::render_all_snakes(std::vector<std::shared_ptr<Snake>>& snakes, Camera camera)
    {
        //MAYBE ADD SOME DIFFERENT RENDERIZATIONS DEPENDING ON THE "Direction" ATTRIBUTE

        //For each snake in the game
        for(int i = 0; i < snakes.size(); i++){
            snakes[i]->render_snake(camera);
        }
    }

    // WE HAVE TO MAKE SURE THAT THE MAP HAS A SIZE ALWAYS BIGGER THAN MAX{COLS,LINES}
    //ANALYSE THE CASE WHERE cam_x + LINES OU cam_y + COLS > scenario.getMapSize()
    void Screen::render_scenario(Scenario scenario, Camera camera)
    {
        uint8_t* aux_map = scenario.getMap();
        int cam_x = camera.getCam_x();
        int cam_y = camera.getCam_y();
        int width = camera.getWidth();
        int height = camera.getHeight();

        for(int i = cam_y; i < cam_y + height && i < scenario.getMapSize(); i++){
            for(int j = cam_x; j < cam_x + width && j < scenario.getMapSize(); j++){
                if(aux_map[i*scenario.getMapSize() + j] == 1)
                    mvaddch(i - cam_y,j - cam_x,'#');

                else if (aux_map[i*scenario.getMapSize() + j] == 2)
                    mvaddch(i - cam_y,j - cam_x,'$');
            }
        }

        refresh();
    }
    
    void Screen::clear_screen()
    {
        clear();
    }
}
