#ifndef SERIALIZE_HPP_
#define SERIALIZE_HPP_

#include <memory>
#include "../model/model.hpp"
#include "../net/net.hpp"

namespace serialize{
    
    class Vector_Serializer
    {
    public:
        net::NetObject serialize_snake_vector(std::vector<std::shared_ptr<model::Snake>>& snakes);
        std::vector<std::shared_ptr<model::Snake>> unserialize_snake_vector(net::NetObject& netobj);
    };
}

#endif
