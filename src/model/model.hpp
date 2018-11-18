
#ifndef MODEL_HPP_
#define MODEL_HPP_

#define MAP_SIZE 200
#define NUMBER_OF_CASHES 100

#include <ncurses.h>
#include <vector>
#include <memory>
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
    
    struct Block
    {
        int x;
        int y;
        chtype ch;        
    };
    
    class Camera
    {
    private:
        int cam_x;
        int cam_y;
        int width = COLS;
        int height = LINES;
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
        
        bool getLiviness();
        void die();
        
        void update_pos();        
        void render_snake(Camera camera);
        
        bool check_body_position(int pos_x, int pos_y);
        bool detect_snake_collision(std::vector<std::shared_ptr<Snake>>& snakes);
        
        void serialize(net::NetObject& netobj);
        void unserialize(net::NetObject& netobj);
    };  
    
    //int Snake::snake_counter = 0;
    
    class Scenario
    {
    private:
        uint8_t map[MAP_SIZE * MAP_SIZE];
        
    public:
        Scenario();        
        void init();
        int getMapSize();
        uint8_t* getMap();
        void Generate_Cash();
        void manage_block_collision(Snake& snake);
        void serialize(net::NetObject& netobj);
        void unserialize(net::NetObject& netobj);
    };
    
    class Physics
    {
    public:
        Physics();
        void manage_collisions(std::vector<std::shared_ptr<Snake>>& snakes, Scenario& scenario);
        void update_all(std::vector<std::shared_ptr<Snake>>& snakes, Scenario& scenario);
    };
}

#endif 
