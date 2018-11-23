#ifndef SERIALIZE_HPP_
#define SERIALIZE_HPP_

#include <memory>
#include "../model/model.hpp"
#include "../net/net.hpp"

namespace serialize{
    
    /*
     * Class responsible for serialize and unserialize a whole vector of snakes
     */
    class Vector_Serializer
    {
    public:
        std::shared_ptr<net::NetObject> serialize_snake_vector(std::vector<std::shared_ptr<model::Snake>>& snakes);
        void unserialize_snake_vector(net::NetObject& netobj, std::vector<std::shared_ptr<model::Snake>>& snakes);
    };
}

#endif
