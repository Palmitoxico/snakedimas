#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include "model.hpp"

namespace model
{
    using namespace net;
    
//------------------- Camera -----------------------------
    int Snake::snake_counter = 0;
    
    Camera::Camera(int cam_x, int cam_y)
    {
        this->cam_x = cam_x;
        this->cam_y = cam_y;
    }
    
    int Camera::getCam_x()
    {
        return this->cam_x;
    }
    
    int Camera::getCam_y()
    {
        return this->cam_y;
    }
    
    int Camera::getWidth()
    {
        return this->width;
    }
    
    int Camera::getHeight()
    {
        return this->height;
    }
    
    void Camera::setCam_x(int cam_x)
    {
        if(!(cam_x < 0 || cam_x > this->max_x))
            this->cam_x = cam_x;        
    }
    
    void Camera::setCam_y(int cam_y)
    {
        if(!(cam_y < 0 || cam_y > this->max_y))
            this->cam_y = cam_y;
    }
    
//------------------------- Snake -------------------------
    
    /*Snake::Snake()
    {
        
    }*/
    
    Snake::Snake(int pos_x, int pos_y, Direction direction)
    {
        this->direction = direction;
        this->uid = Snake::snake_counter;
        Snake::snake_counter += 1;
        
        Block newBlock;
        newBlock.x = pos_x;
        newBlock.y = pos_y;        
        newBlock.ch = '@';
        
        this->body.push_back(newBlock);
        newBlock.ch = 'o';
        
        int up = 0, down = 0, left = 0, right = 0;
        
        switch(this->direction){
            case Up:
            up = 1;
            break;
            
            case Left:
            left = 1;
            break;
                
            case Down:
            down = 1;
            break;
                
            case Right:
            right = 1;
            break;                    
        }
        
        for(int i = 0; i < 4; i++){
            newBlock.x += left - right;
            newBlock.y += up - down;
            this->body.push_back(newBlock);
        }
    }
    
    Snake::~Snake()
    {
        
    }
    
    void Snake::increase()
    {
        Block newBlock;
        newBlock.x = body[body.size() - 1].x;
        newBlock.y = body[body.size() - 1].y;
        newBlock.ch = 'o';
        
        switch(this->direction){
            case Up:
            newBlock.y++;
            break;
            
            case Left:
            newBlock.x++;
            break;
                
            case Down:
            newBlock.y--;
            break;
                
            case Right:
            newBlock.x--;
            break;                    
        }
        
        body.push_back(newBlock);
    }
    
    int Snake::getHeadPosition_x()
    {
        return this->body[0].x;
    }
    
    int Snake::getHeadPosition_y()
    {
        return this->body[0].y;
    }
    
    Direction Snake::getDirection()
    {
        return this->direction;
    }
    
    void Snake::setDirection(Direction direction)
    {
        if(!((this->direction == Up && direction == Down)||(this->direction == Down && direction == Up)
           ||(this->direction == Left && direction == Right)||(this->direction == Right && direction == Left)))
            this->direction = direction;
    }
    
    std::vector<Block> Snake::getBody()
    {
        return body;
    }
    
    int Snake::getUID()
    {
        return this->uid;
    }
    
    bool Snake::getLiviness()
    {
        return this->dead;
    }
    
    void Snake::die()
    {
        this->dead = true;
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
    
    void Snake::render_snake(Camera camera)
    {
        if(this->dead)
            return;
        
        int start_x = camera.getCam_x();
        int end_x = start_x + camera.getWidth();
        int start_y = camera.getCam_y();
        int end_y = start_y + camera.getHeight();
        
        for(int j = 0; j < this->body.size(); j++){
            if(this->body[j].x >= start_x && this->body[j].x <= end_x 
            && this->body[j].y >= start_y && this->body[j].y <= end_y){
                mvaddch(this->body[j].y - start_y, this->body[j].x - start_x, this->body[j].ch);
            }
        }
        
        refresh();
    }
    
    bool Snake::check_body_position(int pos_x, int pos_y)
    {
        for(int i = 0; i < this->body.size(); i++){
            if(this->body[i].x == pos_x && this->body[i].y == pos_y)
                return true;
        }
        
        return false;
    }
    
    bool Snake::detect_snake_collision(std::vector<std::shared_ptr<Snake>>& snakes)
    {
        int up = 0, left = 0, down = 0, right = 0;
        int next_x = this->body[0].x;
        int next_y = this->body[0].y;
        
        switch(this->direction){
            case Up:
            next_y--;
            break;
            
            case Left:
            next_x--;
            break;
            
            case Down:
            next_y++;
            break;
            
            case Right:
            next_x++;
            break;
        }
        
        for(int i = 0; i < snakes.size(); i++){
            if(snakes[i]->check_body_position(next_x, next_y))
                return true;
        }
        
        return false;
    }
    
    void Snake::serialize(NetObject& netobj)
    {
        netobj.id = snake;
        uint8_t dir;
        
        netobj.data.clear();
        
        switch(this->direction){
            case Up:
            dir = 1;
            break;
            
            case Right:
            dir = 2;
            break;
            
            case Down:
            dir = 3;
            break;
            
            case Left:
            dir = 4;
            break;
        }
        
        //Serialize direction
        netobj.data.push_back(dir);
        
        //Serialize uid 
        for(int i = 0; i < 4; i++)
            netobj.data.push_back((this->uid >> i*8) & 0xFF);
        
        //Serialize dead atribute
        netobj.data.push_back(this->dead);
        
        //Serialize body size
        int size = this->body.size();
        for(int i = 0; i < 4; i++)
            netobj.data.push_back((size >> i*8) & 0xFF);
        
        //Serialize snake's body
        for(int i = 0; i < this->body.size(); i++){
            //Serialize a single block of the body
            for(int j = 0; j < 4; j++){
                netobj.data.push_back((this->body[i].x >> j*8) & 0xFF);
            }
            
            for(int j = 0; j < 4; j++){
                netobj.data.push_back((this->body[i].y >> j*8) & 0xFF);
            }
            
            for(int j = 0; j < 4; j++){
                netobj.data.push_back((this->body[i].ch >> j*8) & 0xFF);
            }
        }    
    }
    
    void Snake::unserialize(NetObject& netobj)
    {
        //Unserialize direction
        int dir = netobj.data[0];
        switch(dir){
            case 1:
            this->direction = Up;
            break;
            
            case 2:
            this->direction = Right;
            break;
            
            case 3:
            this->direction = Down;
            break;
            
            case 4:
            this->direction = Left;
            break;
        }
        
        //Unserialize uid
        this->uid = 0;
        for(int i = 1; i < 5; i++){
            this->uid >>= 8;
            this->uid |= (netobj.data[i] << 24);
        }
        
        this->dead = netobj.data[5];
        
        int size = 0;
        for(int i = 6; i < 10; i++){
            size >>= 8;
            size |= (netobj.data[i] << 24);
        }
    
        this->body.clear();
        
        for(int i = 10; i < 10 + size*12; i += 12){
            Block newBlock;
            newBlock.x = 0;
            newBlock.y = 0;
            newBlock.ch = 0;
            
            //Unserialize x field of the block
            for(int j = 0; j < 4; j++){
                newBlock.x >>= 8;
                newBlock.x |= (netobj.data[i + j] << 24);  
            }
            
            //Unserialize y field of the block
            for(int j = 4; j < 8; j++){
                newBlock.y >>= 8;
                newBlock.y |= (netobj.data[i + j] << 24);  
            }
            
            //Unserialize ch field of the block
            for(int j = 8; j < 12; j++){
                newBlock.ch >>= 8;
                newBlock.ch |= (netobj.data[i + j] << 24);  
            }
            
            this->body.push_back(newBlock);
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
        
        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution_pos(1,MAP_SIZE - 1);
        generator.seed((int)time(NULL));        
        
        int rand_x, rand_y;
        
        for(int i = 0; i < NUMBER_OF_CASHES; i++){
            rand_x = distribution_pos(generator);
            rand_y = distribution_pos(generator);
            
            this->map[rand_y*MAP_SIZE + rand_x] = 2;
        }        
    }
    
    uint8_t* Scenario::getMap()
    {
        return map;
    }
    
    int Scenario::getMapSize()
    {
        return MAP_SIZE;
    }
    
    void Scenario::manage_block_collision(Snake& snake)
    {
        int up = 0, left = 0, down = 0, right = 0;
        int next_x = snake.getHeadPosition_x();
        int next_y = snake.getHeadPosition_y();
        
        switch(snake.getDirection()){
            case Up:
            next_y--;
            break;
            
            case Left:
            next_x--;
            break;
            
            case Down:
            next_y++;
            break;
            
            case Right:
            next_x++;
            break;
        }
        
        if(this->map[next_y*MAP_SIZE + next_x] == 1){
            snake.die();
        }
        else if(this->map[next_y*MAP_SIZE + next_x] == 2){
            map[next_y*MAP_SIZE + next_x] = 0;
            snake.increase();
        }
    }

	void Scenario::serialize(net::NetObject& netobj)
	{
		netobj.data.clear();
        netobj.id = scenario;
        
        for(int i = 0; i < MAP_SIZE*MAP_SIZE; i++){
            netobj.data.push_back(this->map[i]);
        }
	}

	void Scenario::unserialize(net::NetObject& netobj)
	{
        for(int i = 0; i < MAP_SIZE*MAP_SIZE; i++){
            this->map[i] = netobj.data[i];
        }
	}
//------------------- Physics ----------------------------

    Physics::Physics(){}

    void Physics::manage_collisions(std::vector<std::shared_ptr<Snake>>& snakes, Scenario& scenario)
    {
        for(int i = 0; i < snakes.size(); i++){
            if(snakes[i]->detect_snake_collision(snakes)){
                snakes[i]->die();
            }
            else{
                scenario.manage_block_collision(*snakes[i]);
            }            
        }
    }
    
    void Physics::update_all(std::vector<std::shared_ptr<Snake>>& snakes, Scenario& scenario)
    {
        this->manage_collisions(snakes, scenario);
        for(int i = 0; i < snakes.size(); i++){
            snakes[i]->update_pos();
        }        
    }
}

