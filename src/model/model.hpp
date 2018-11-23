
#ifndef MODEL_HPP_
#define MODEL_HPP_

#define MAP_SIZE 200
#define NUMBER_OF_CASHES 100

#include <ncurses.h>
#include <vector>
#include <memory>
#include <random>
#include "../net/net.hpp"

namespace model 
{
        
    enum Direction 
    {
        Up,
        Right,
        Down,
        Left,
    };
    
    /*
     * Defines the structure of a single block of a snake
     */
    struct Block
    {
        int x;
        int y;
        chtype ch;        
    };
    
    /*
     * Class responsible for wrapping up the model of a camera and it's view
     * of the elements of the game
     */
    class Camera
    {
    private:
        int cam_x;
        int cam_y;
        int width = COLS;
        int height = LINES;
        
        // Defines maximum values for the coordinates x and y that the camera can go
        int max_x = MAP_SIZE - COLS;
        int max_y = MAP_SIZE - LINES;
        
    public:
        Camera(int cam_x, int cam_y);
        int getCam_x();
        int getCam_y();
        int getWidth();
        int getHeight();
        
        void setCam_x(int cam_x);
        void setCam_y(int cam_y);
    };
    
    /*
     * Class responsible for wrapping up a single snake
     */
    class Snake 
    {
    private:
        std::vector<Block> body;
        Direction direction;
        int uid;
        static int snake_counter;
        bool dead = false;

    public:
        Snake(int pos_x, int pos_y, Direction direction);
        ~Snake();
        
        void increase();
        int getHeadPosition_x();
        int getHeadPosition_y();
        
        Direction getDirection();
        void setDirection(Direction direction);
        
        std::vector<Block> getBody();
        int getUID();
		int getScore();
        
        bool getLiviness();
        void die();
        
        void update_pos();        
        void render_snake(Camera camera);
        
        // Internal method, it's used by the method "detect_snake_collision"
        bool check_body_position(int pos_x, int pos_y);
        bool detect_snake_collision(std::vector<std::shared_ptr<Snake>>& snakes);
        
        void serialize(net::NetObject& netobj);
        void unserialize(net::NetObject& netobj);
    };  
    
    /*
     * Class responsible for wrapping up the scenario of the game
     */
    class Scenario
    {
    private:
        uint8_t map[MAP_SIZE * MAP_SIZE];
        std::default_random_engine generator;

    public:
        Scenario();        
        void init();
        int getMapSize();
        uint8_t* getMap();
        void generate_cash();
        void manage_block_collision(Snake& snake);
        void serialize(net::NetObject& netobj);
        void unserialize(net::NetObject& netobj);
    };
    
    /*
     * Class responsible for update the elements of the game
     */
    class Physics
    {
    public:
        Physics();
        void manage_collisions(std::vector<std::shared_ptr<Snake>>& snakes, Scenario& scenario);
        void update_all(std::vector<std::shared_ptr<Snake>>& snakes, Scenario& scenario);
    };
}

#endif 
