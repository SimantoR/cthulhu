#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <utils.h>
#include <glm/glm.hpp>
#include "Transform.hpp"

class Camera
{
public:
    Ptr<Transform> transform = nullptr;
    float          FOV       = 45.0f;
    float          ZNear     = 0.1f;
    float          ZFar      = 100.0f;
    float          PanSpeed  = 1.0f;
    glm::vec2      Viewport  = glm::vec2( 1.0f );

    Camera();

    glm::mat4 GetView();
    glm::mat4 GetOrthoProjection();
    glm::mat4 GetPerspectiveProjection();

    glm::vec3 toScreenCoordinates( const glm::vec3& pos, const glm::mat4& mvp );
};

#endif
