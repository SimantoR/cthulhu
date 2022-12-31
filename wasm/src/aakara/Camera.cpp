#include <aakara/Camera.hpp>
#include <cmath>
#include <emscripten/bind.h>
#include <emscripten/console.h>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/quaternion_common.hpp>

Camera::Camera()
{
    transform = std::make_shared<Transform>();
}

glm::mat4 Camera::GetView()
{
    // return glm::lookAt(
    //     glm::normalize( Position ), glm::normalize( Position + Forward ), glm::normalize( Up ) );

    glm::vec3 pos = transform->position;
    return glm::lookAt( pos, pos + transform->forward(), transform->up() );
}

glm::mat4 Camera::GetOrthoProjection()
{
    return glm::ortho( -45.0f, +45.0f, -45.0f, +45.0f, 0.1f, 30.0f );
}

glm::mat4 Camera::GetPerspectiveProjection()
{
    return glm::perspective<f32>( FOV, Viewport.x / Viewport.y, ZNear, ZFar );
}

glm::vec3 Camera::toScreenCoordinates( const glm::vec3& pos, const glm::mat4& mvp )
{
    // LINK: https://biturl.top/ueAn6f
    glm::vec4 s = {
        pos.x * mvp[0][0] + pos.y * mvp[1][0] + pos.z * mvp[2][0] + mvp[3][0],
        pos.x * mvp[0][1] + pos.y * mvp[1][1] + pos.z * mvp[2][1] + mvp[3][1],
        pos.x * mvp[0][2] + pos.y * mvp[1][2] + pos.z * mvp[2][2] + mvp[3][2],
        pos.x * mvp[0][3] + pos.y * mvp[1][3] + pos.z * mvp[2][3] + mvp[3][3],
    };

    glm::vec3 screen = {
        s[0] / s[3] * Viewport.x / 2 + Viewport.y / 2,
        s[1] / s[3] * Viewport.x / 2 + Viewport.y / 2,
        s[2] / s[3],
    };

    return screen;
}

glm::vec2 getViewport( Ptr<Camera> cam )
{
    return cam->Viewport;
}

void setViewport( Ptr<Camera> cam, glm::vec2 viewport )
{
    cam->Viewport = viewport;
}

Ptr<Transform> getTransform( Ptr<Camera> cam )
{
    return cam->transform;
}

EMSCRIPTEN_BINDINGS( CAMERA_HPP )
{
    emscripten::class_<Camera>( "Camera" ) //
        .smart_ptr_constructor( "Camera", &std::make_shared<Camera> )
        // .property( "transform", &Camera::transform )
        .function( "getTransform", &getTransform )
        .function( "getViewport", &getViewport )
        .function( "setViewport", &setViewport );
}
