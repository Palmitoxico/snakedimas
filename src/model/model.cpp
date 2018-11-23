#include <iostream>
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

    /*
     * Getters of the attributes of the camera
     */
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

    /*
     * Setters of the attribute of the camera
     * (only the ones that can be changed)
     */
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
    
    /*
     * In the constructor of the snake, we pass the desired inital position for it's head
     * and the initial direction we wish it to move to
     */
    Snake::Snake(int pos_x, int pos_y, Direction direction)
    {
        this->direction = direction;
        /* For every snake that is created, we define a unique id based on
         * the total number of snakes that were allocated on the game, we then
         * increment the counter of the total of snakes
         */
        this->uid = Snake::snake_counter;
        Snake::snake_counter += 1;

        // We fisrt define the head of the snake and add it to it's body
        Block newBlock;
        newBlock.x = pos_x;
        newBlock.y = pos_y;
        newBlock.ch = '@';

        this->body.push_back(newBlock);
        newBlock.ch = 'o';

        /* Based on the initial direction of the snake (passed to the constructor)
         * initializes the rest of the body of the snake in a consistent manner
         */
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

    /*
     * Adds a block to the tale of the snake based in it's actual direction
     */
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

    /*
     * Returns the x coordinate of the head of the snake
     */
    int Snake::getHeadPosition_x()
    {
        return this->body[0].x;
    }

    /*
     * Returns the y coordinate of the head of the snake
     */
    int Snake::getHeadPosition_y()
    {
        return this->body[0].y;
    }

    /*
     * Returns the direction of the snake
     */
    Direction Snake::getDirection()
    {
        return this->direction;
    }

    /*
     * Sets the direction of the snake in a consistent way,that is,
     * the snake can never change it's direction to the opposite of it's 
     * actual one
     */
    void Snake::setDirection(Direction direction)
    {
        if(!((this->direction == Up && direction == Down)||(this->direction == Down && direction == Up)
           ||(this->direction == Left && direction == Right)||(this->direction == Right && direction == Left)))
            this->direction = direction;
    }

    /*
     * Returns the vector containing the body of the snake
     */
    std::vector<Block> Snake::getBody()
    {
        return body;
    }

    /*
     * Returns the unique id of the snake
     */
    int Snake::getUID()
    {
        return this->uid;
    }

    /*
     * Calculates a score based on the size of the snake's body
     */
	int Snake::getScore()
	{
		return this->body.size() - 5;
	}

	/*
     * Returns true if the snake is dead, and false otherwise
     */
    bool Snake::getLiviness()
    {
        return this->dead;
    }

    /*
     * If the snake dies, it's "dead" atribute is set up to true
     */
    void Snake::die()
    {
        this->dead = true;
    }

    /*
     * Updates the position of the snake, based on it's direction
     */
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

    /*
     * Renders the parts of the snake that are in the field of vision 
     * of the camera given as argument
     */
    void Snake::render_snake(Camera camera)
    {
        if(this->dead)
            return;

        int start_x = camera.getCam_x();
        int end_x = start_x + camera.getWidth();
        int start_y = camera.getCam_y();
        int end_y = start_y + camera.getHeight();

        // Notice that the parts of the snake are printed in coordinates relative to the camera
        for(int j = 0; j < this->body.size(); j++){
            if(this->body[j].x >= start_x && this->body[j].x <= end_x
            && this->body[j].y >= start_y && this->body[j].y <= end_y){
                mvaddch(this->body[j].y - start_y, this->body[j].x - start_x, this->body[j].ch);
            }
        }

        // Updates the screen
        refresh();
    }

    /*
     * Given the coordinates pos_x and pos_y, checks if thre's a block of the 
     * snake's body in the provided position. It returns true in affirmative case
     * and false otherwise
     */
    bool Snake::check_body_position(int pos_x, int pos_y)
    {
        for(int i = 0; i < this->body.size(); i++){
            if(this->body[i].x == pos_x && this->body[i].y == pos_y)
                return true;
        }

        return false;
    }

    /*
     * Checks if the snake is going to collide with any of the snakes in the vector
     * provided as an argument. It returns true in affirmative case and false otherwise
     */
    bool Snake::detect_snake_collision(std::vector<std::shared_ptr<Snake>>& snakes)
    {
        int up = 0, left = 0, down = 0, right = 0;
        // Takes the actual position of the head of the snake
        int next_x = this->body[0].x;
        int next_y = this->body[0].y;

        /* Provided the direction of the snake, calculates which if going to be 
         * it's next head position
         */
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

        // Ckecks if there's a block in the next position of the snake
        for(int i = 0; i < snakes.size(); i++){
            if(snakes[i]->check_body_position(next_x, next_y))
                return true;
        }

        return false;
    }

    /*
     * This method serializes the actual snake and wraps it up in the netobj
     * provided as parameter
     */
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

        //Serializes direction
        netobj.data.push_back(dir);

        //Serializes uid
        for(int i = 0; i < 4; i++)
            netobj.data.push_back((this->uid >> i*8) & 0xFF);

        //Serializes dead atribute
        netobj.data.push_back(this->dead);

        //Serializes body size
        int size = this->body.size();
        for(int i = 0; i < 4; i++)
            netobj.data.push_back((size >> i*8) & 0xFF);

        //Serializes the snake's body
        for(int i = 0; i < this->body.size(); i++){
            //Serialize a single block of the body (a field at a time)
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

    /*
     * Provided a netobj, unserializes it's content into a snake
     */
    void Snake::unserialize(NetObject& netobj)
    {
        //Unserializes direction
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

        //Unserializes uid
        this->uid = 0;
        for(int i = 1; i < 5; i++){
            this->uid = (unsigned)this->uid >> 8;
            this->uid |= ((unsigned)netobj.data[i] << 24);
        }

        // Unserializes dead attribute
        this->dead = netobj.data[5];

        // Unserializes the size of the body of the snake
        int size = 0;
        for(int i = 6; i < 10; i++){
            size = (unsigned)size >> 8;
            size |= ((unsigned)netobj.data[i] << 24);
        }

        this->body.clear();

        // Unserializes the snake's body
        for(int i = 10; i < 10 + size*12; i += 12){
            Block newBlock;
            newBlock.x = 0;
            newBlock.y = 0;
            newBlock.ch = 0;

            //Unserialize x field of the block
            for(int j = 0; j < 4; j++){
                newBlock.x = (unsigned)newBlock.x >> 8;
                newBlock.x |= ((unsigned)netobj.data[i + j] << 24);
            }

            //Unserialize y field of the block
            for(int j = 4; j < 8; j++){
                newBlock.y = (unsigned)newBlock.y >> 8;
                newBlock.y |= ((unsigned)netobj.data[i + j] << 24);
            }

            //Unserialize ch field of the block
            for(int j = 8; j < 12; j++){
                newBlock.ch = (unsigned)newBlock.ch >> 8;
                newBlock.ch |= ((unsigned)netobj.data[i + j] << 24);
            }

            this->body.push_back(newBlock);
        }
    }

//----------------- Scenario ---------------------------

    /*
     * Inits a seed that will be used to generate the map of the scenario, and then
     * inits the scenario itself
     */
    Scenario::Scenario()
    {
		this->generator.seed((int)time(NULL));
        this->init();
    }

    /*
     * Inits the map that represents the scenario
     */
    void Scenario::init()
    {
        /* Fills in the map, if a position of the map has a 1, this position represents a brick,
         * if it has a 0, represents an empty space and if it has a 2 the position contains a 
         * cash token
         */
        
        // Fills in the borders of the map with bricks and the rest of the map with empty spaces
        for(int i = 0; i < MAP_SIZE; i++){
            for(int j = 0; j < MAP_SIZE; j++){
                if(i == 0 || i == MAP_SIZE - 1 || j == 0 || j == MAP_SIZE - 1)
                    this->map[i*MAP_SIZE + j] = 1;

                else
                    this->map[i*MAP_SIZE + j] = 0;
            }
        }

        // Fills some random positions of the map with cash tokens
        std::uniform_int_distribution<int> distribution_pos(1,MAP_SIZE - 2);

        int rand_x, rand_y;

        for(int i = 0; i < NUMBER_OF_CASHES; i++){
            rand_x = distribution_pos(this->generator);
            rand_y = distribution_pos(this->generator);

            this->map[rand_y*MAP_SIZE + rand_x] = 2;
        }
    }

    /*
     * Generates a cash token in some random position at the map
     */
    void Scenario::generate_cash()
    {
        std::uniform_int_distribution<int> distribution_pos(1,MAP_SIZE - 2);

        int rand_x = distribution_pos(this->generator);
        int rand_y = distribution_pos(this->generator);

        this->map[rand_y*MAP_SIZE + rand_x] = 2;
    }

    /*
     * Returns the matrix that represents the scenario
     */
    uint8_t* Scenario::getMap()
    {
        return map;
    }

    /*
     * Return the size of the size of the map.
     * 
     * In this game, we considered the dimensions of the map to be equal
     */
    int Scenario::getMapSize()
    {
        return MAP_SIZE;
    }

    /*
     * Checks if a snake, provided as an argument, collides with some item of the scenario,
     * and then takes the proper measures to deal with it
     */
    void Scenario::manage_block_collision(Snake& snake)
    {
        // The collision logic is very similar to that one of the "detect_snake_collision" method
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

        // If the snake collides with a block, it dies
        if(this->map[next_y*MAP_SIZE + next_x] == 1){
            snake.die();
        }
        
        // Else if a snake collides with a cash token, it increases it's size
        else if(this->map[next_y*MAP_SIZE + next_x] == 2){
            map[next_y*MAP_SIZE + next_x] = 0;
            snake.increase();
        }
    }

    /*
     * This method serializes the scenario and wraps it up in the netobj
     * provided as a parameter
     */
	void Scenario::serialize(net::NetObject& netobj)
	{
		netobj.data.clear();
        netobj.id = scenario;

        for(int i = 0; i < MAP_SIZE*MAP_SIZE; i++){
            netobj.data.push_back(this->map[i]);
        }
	}

	/*
     * Provided a netobj, unserializes it's content into a scenario object
     */
	void Scenario::unserialize(net::NetObject& netobj)
	{
        for(int i = 0; i < MAP_SIZE*MAP_SIZE; i++){
            this->map[i] = netobj.data[i];
        }
	}
//------------------- Physics ----------------------------

    Physics::Physics(){}

    /*
     * Provided a vector of shared_ptr to snakes and a scenario, checks if any kind of collision occurs
     * and take the proper measures to manage these
     */
    void Physics::manage_collisions(std::vector<std::shared_ptr<Snake>>& snakes, Scenario& scenario)
    {
        for(int i = 0; i < snakes.size(); i++){
            // For every snake, checks if it collides with any other snake in the vector
            if(snakes[i]->detect_snake_collision(snakes)){
                snakes[i]->die();
            }
            // Also checks if it collide if some element of the scenario
            else{
                scenario.manage_block_collision(*snakes[i]);
            }
        }
    }

    /*
     * Provided a vector of shared_ptr to snakes and a scenario, update all of the elements of the game
     * managing all of the events that can occur in an interaction
     */
    void Physics::update_all(std::vector<std::shared_ptr<Snake>>& snakes, Scenario& scenario)
    {
        this->manage_collisions(snakes, scenario);
        for(int i = 0; i < snakes.size(); i++){
            snakes[i]->update_pos();
        }
    }
}
