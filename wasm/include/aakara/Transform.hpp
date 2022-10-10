#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <utils.h>
#include <glm/vec3.hpp>

class Transform
{
public:
    glm::vec3 position = glm::vec3( 0.0f );
    glm::vec3 rotation = glm::vec3( 0.0f );
    glm::vec3 scale    = glm::vec3( 1.0f );

    Transform();
    Transform( const Transform& other );
    Transform( glm::vec3 pos, glm::vec3 rot, glm::vec3 scale );

    glm::vec3 forward();
    glm::vec3 up();
    glm::vec3 right();
};

#endif
