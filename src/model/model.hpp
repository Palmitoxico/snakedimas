
#ifndef MODEL_HPP_
#define MODEL_HPP_

#define MAP_SIZE 192

#include <ncurses.h>
#include<vector>

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
    
    class Snake 
    {
    protected:
        std::vector<Block> body;
        float speed;
        Direction direction;
        
    public:
        //Snake();
        Snake(int pos_x, int pos_y, float speed, Direction direction);
        ~Snake();
        
        void increase(Block newBlock);
        //float getPosition();
        //void setPosition(float position);
        float getSpeed();
        void setSpeed(float speed);
        Direction getDirection();
        void setDirection(Direction direction);
        std::vector<Block> getBody();
        void update_pos();
        bool detect_Body_Collision(int pos_x, int pos_y);
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
    };
    
    class Physics
    {
        void update_all(std::vector<Snake>& snakes);
    };
}

#endif 
