#ifndef RAY_HPP
#define RAY_HPP

#include "../utils.h"
#include <glm/vec3.hpp>

class Ray
{
public:
    glm::vec3 m_Direction;
    f32       m_Distance;

    Ray( glm::vec3 direction, f32 distance );

    bool operator==( const Ray& );
};

#endif
