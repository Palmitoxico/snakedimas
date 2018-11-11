#include <iostream>
#include <vector>
#include "model.hpp"

namespace model
{
    
//------------------------- Snake -------------------------
    
    /*Snake::Snake()
    {
        
    }*/
    
    Snake::Snake(int pos_x, int pos_y, float speed, Direction direction)
    {
        this->speed = speed;
        this->direction = direction;
        
        Block newBlock;
        newBlock.x = pos_x;
        newBlock.y = pos_y;
        newBlock.ch = 'o';
        
        for(int i = 0; i < 4; i++){
            this->body.push_back(newBlock);
            newBlock.x++;
        }
    }
    
    Snake::~Snake()
    {
        
    }
    
    void Snake::increase(Block newBlock)
    {
        body.push_back(newBlock);
    }
    
    float Snake::getSpeed()
    {
        return this->speed;
    }
    
    void Snake::setSpeed(float speed)
    {
        this->speed = speed;
    }
    
    Direction Snake::getDirection()
    {
        return this->direction;
    }
    
    void Snake::setDirection(Direction direction)
    {
        this->direction = direction;
    }
    
    std::vector<Block> Snake::getBody()
    {
        return body;
    }
    
    void Snake::update_pos()
    {
        for(int i = body.size(); i > 0; i--){
            body[i].x = body[i - 1].x;
            body[i].y = body[i - 1].y;
        }
        
        switch(this->direction){
            case Up:
            body[0].y--;
            break;
            
            case Left:
            body[0].x--;
            break;
                
            case Down:
            body[0].y++;
            break;
                
            case Right:
            body[0].x++;
            break;                    
        }
    }
    
//----------------- Scenario ---------------------------

    Scenario::Scenario()
    {
        this->init();
    }
    
    void Scenario::init()
    {
        for(int i = 0; i < MAP_SIZE; i++){
            for(int j = 0; j < MAP_SIZE; j++){
                if(i == 0 || i == MAP_SIZE - 1 || j == 0 || j == MAP_SIZE - 1)
                    this->map[i*MAP_SIZE + j] = 1;
            
                else
                    this->map[i*MAP_SIZE + j] = 0;
            }
        }
    }
    
    int* Scenario::getMap()
    {
        return map;
    }
    
    int Scenario::getMapSize()
    {
        return MAP_SIZE;
    }
}
