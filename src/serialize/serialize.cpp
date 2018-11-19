#include "serialize.hpp"
#include <iostream>

namespace serialize{
    
    std::shared_ptr<net::NetObject> Vector_Serializer::serialize_snake_vector(std::vector<std::shared_ptr<model::Snake>>& snakes)
    {   
        auto final_netobj = std::make_shared<net::NetObject>();
        final_netobj->id = net::snake_vector;
        
        //Serializes the total number of snakes
        for(int i = 0; i < 4; i++)
            final_netobj->data.push_back((snakes.size() >> i*8) & 0xFF);
        
        for(int i = 0; i < snakes.size(); i++){
            net::NetObject netobj;
            snakes[i]->serialize(netobj);
            
            //Serializes the netobj
            
            //Serializes the netobj data's field size
            for(int i = 0; i < 4; i++)
                final_netobj->data.push_back((netobj.data.size() >> i*8) & 0xFF);
            
            //Serializes the netobj's data field
            for(int i = 0; i < netobj.data.size(); i++){
                final_netobj->data.push_back((netobj.data[i]));
            }
        }
        
        return final_netobj;
    }
    
    void Vector_Serializer::unserialize_snake_vector(net::NetObject& netobj, std::vector<std::shared_ptr<model::Snake>>& snakes)
    {
        //Unserializes the total number of snakes
        int size = 0;
        for(int i = 0; i < 4; i++){
            size >>= 8;
            size |= (netobj.data[i] << 24);   
        }
        
        std::cout << "the size of the snake is: " << size << std::endl;
        
        //Unserializes the vector of snakes
        
        //Unserializes each of the NetObjects
        int base = 4;
        
        for(int i = 0; i < size; i++){
            
            net::NetObject aux;
            aux.id = net::snake;
            
            //Unserializes the data's field size
            int data_size = 0;
            for(int j = 0; j < 4; j++){
                data_size = (unsigned)data_size >> 8;
                data_size |= (unsigned)(netobj.data[base + j] << 24);
            }
            
            //Unserializes the data fiel
            aux.data.clear();
            for(int j = 4; j < data_size + 4; j++){
                aux.data.push_back(netobj.data[base + j]);
            }
            
            //Unserializes a snake from the NetObject that have been just obtained
            auto new_snake = std::make_shared<model::Snake>(0,0,model::Up);
            new_snake->unserialize(aux);
            
            snakes.push_back(new_snake);
            
            base += data_size + 4;
        }
    }
}
