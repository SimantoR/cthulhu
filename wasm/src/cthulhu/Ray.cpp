#include <cthulhu/Ray.hpp>

Ray::Ray( glm::vec3 direction, f32 distance )
    : m_Direction( direction )
    , m_Distance( distance )
{
}

bool Ray::operator==( const Ray& other )
{
    return m_Direction == other.m_Direction && m_Distance == other.m_Distance;
}
