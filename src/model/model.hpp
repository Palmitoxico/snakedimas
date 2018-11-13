
#ifndef MODEL_HPP_
#define MODEL_HPP_

#define MAP_SIZE 200
#define NUMBER_OF_CASHES 20

#include <ncurses.h>
#include <vector>
#include <memory>

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
    protected:
        std::vector<Block> body;
        float speed;
        Direction direction;
        
    public:
        Snake(int pos_x, int pos_y, float speed, Direction direction);
        ~Snake();
        
        void increase();
        int getHeadPosition_x();
        int getHeadPosition_y();
        float getSpeed();
        void setSpeed(float speed);
        Direction getDirection();
        void setDirection(Direction direction);
        std::vector<Block> getBody();
        
        void die();
        void update_pos();
        
        void render_snake(Camera camera);
        
        bool check_body_position(int pos_x, int pos_y);
        bool detect_snake_collision(std::vector<std::shared_ptr<Snake>>& snakes);
    };  
    
    class Scenario
    {
    private:
        int map[MAP_SIZE * MAP_SIZE];
        
    public:
        Scenario();        
        void init();
        int getMapSize();
        int* getMap();
        void manage_block_collision(Snake& snake);            
    };
    
    class Physics
    {
    public:
        Physics();
        void manage_collisions(std::vector<std::shared_ptr<Snake>>& snakes, Scenario scenario);
        void update_all(std::vector<std::shared_ptr<Snake>>& snakes, Scenario scenario);
    };
}

#endif 
