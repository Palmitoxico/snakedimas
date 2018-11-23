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
    
    /* 
     * Terminates the ncurses mode stoping the screen's functionalities
     */
    void Screen::stop()
    {
        endwin();
    }
    
    /* 
     * Starts up the curses mode and sets it up
     */
    void Screen::init()
    {
        initscr();              /* Start curses mode */
        raw();                  /* Line buffering disabled */
        curs_set(0);            /* Do not display cursor */
    }

    /*
     * Prints a string on the screen beginning in the coordinates given by posx and posy
     */
	void Screen::print_msg(const std::string& str, int posx, int posy)
	{
		mvprintw(posy, posx,"%s", str.c_str());
        refresh();
	}

	/* 
     * Method responsible for render all of the snakes in the game that are in the field
     * of vision determined by the camera
     */
    void Screen::render_all_snakes(std::vector<std::shared_ptr<Snake>>& snakes, Camera camera)
    {
        //For each snake in the game, make it render itself dependending on the camera
        for(int i = 0; i < snakes.size(); i++){
            snakes[i]->render_snake(camera);
        }
    }

    /*
     * Renders the parts of the scenario that are in the fild of vision given by the camera
     */
    void Screen::render_scenario(Scenario scenario, Camera camera)
    {
        // Gets the parameters of the camera
        uint8_t* aux_map = scenario.getMap();
        int cam_x = camera.getCam_x();
        int cam_y = camera.getCam_y();
        int width = camera.getWidth();
        int height = camera.getHeight();
        
        /* Prints the characters in the coordinates relative to the camera if they are in it's
         * field of vision */
        for(int i = cam_y; i < cam_y + height && i < scenario.getMapSize(); i++){
            for(int j = cam_x; j < cam_x + width && j < scenario.getMapSize(); j++){
                if(aux_map[i*scenario.getMapSize() + j] == 1)
                    mvaddch(i - cam_y,j - cam_x,'#');

                else if (aux_map[i*scenario.getMapSize() + j] == 2)
                    mvaddch(i - cam_y,j - cam_x,'$');
            }
        }
        
        // Updates the screen
        refresh();
    }
    
    /*
     * Clears the screen
     */
    void Screen::clear_screen()
    {
        clear();
    }
}
