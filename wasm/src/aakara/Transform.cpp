#include <emscripten/bind.h>
#include <glm/gtx/rotate_vector.hpp>

#include <aakara/Transform.hpp>

Transform::Transform()
{
}

Transform::Transform( const Transform& other )
    : position( other.position )
    , rotation( other.rotation )
    , scale( other.scale )
{
}

Transform::Transform( glm::vec3 pos, glm::vec3 rot, glm::vec3 scale )
    : position( pos )
    , rotation( rot )
    , scale( scale )
{
}

glm::vec3 Transform::forward()
{
    glm::vec3 forward = { 0.0f, 0.0f, 1.0f };

    forward = glm::rotateX( forward, this->rotation.x );
    forward = glm::rotateY( forward, this->rotation.y );
    forward = glm::rotateZ( forward, this->rotation.z );

    return glm::normalize( forward );
}

glm::vec3 Transform::up()
{
    glm::vec3 up = { 0.0f, 1.0f, 0.0f };

    up = glm::rotateX( up, this->rotation.x );
    up = glm::rotateY( up, this->rotation.y );
    up = glm::rotateZ( up, this->rotation.z );

    return glm::normalize( up );
}

glm::vec3 Transform::right()
{
    return glm::normalize( glm::cross( this->up(), this->forward() ) );
}

void translate( Ptr<Transform> transform, glm::vec3 delta )
{
    transform->position += delta;
}

EMSCRIPTEN_BINDINGS( TRANSFORM_HPP )
{
    emscripten::class_<Transform>( "Transform" )
        .smart_ptr_constructor( "Transform", &std::make_shared<Transform, glm::vec3, glm::vec3, glm::vec3> )
        .property( "position", &Transform::position )
        .property( "rotation", &Transform::rotation )
        .property( "scale", &Transform::scale )
        .function( "translate", &translate );
}
