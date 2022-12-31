#include <emscripten/bind.h>
#include <emscripten/html5.h>
#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cthulhu/OrbitCamera.hpp>
#include <aakara/Global.hpp>
#include <aakara/Camera.hpp>

OrbitCameraControl::OrbitCameraControl( Ptr<Camera> camera, const glm::vec3 focus, f32 distance )
    : m_camera( camera )
    , m_focus( focus )
    , m_Distance( distance )
{
    orbit( 0, 0 );
}

Ptr<Camera> OrbitCameraControl::getCamera()
{
    return m_camera;
}

void OrbitCameraControl::setCamera( Ptr<Camera> cam )
{
    m_camera = cam;
}

void OrbitCameraControl::orbit( float x, float y )
{
    Ptr<Transform> transform  = m_camera->transform;
    glm::vec2      mouseInput = { x, y };

    m_orbitAngles += mouseInput * Global::Time::DeltaTimeS();

    transform->rotation = glm::vec3( m_orbitAngles.x, m_orbitAngles.y, 0 );

    glm::quat lookRotation  = glm::quat( transform->rotation );
    glm::vec3 lookDirection = lookRotation * glm::vec3( 0.0f, 0.0f, 1.0f );

    transform->position = m_focus - lookDirection * m_Distance;

    emscripten_console_logf(
        "Distance between camera and target: %.f", glm::distance( transform->position, m_focus ) );
}

glm::vec3 OrbitCameraControl::getFocus()
{
    return m_focus;
}

void OrbitCameraControl::setFocus( const glm::vec3 focus )
{
    m_focus = focus;
    orbit( 0, 0 );
}

EMSCRIPTEN_BINDINGS( ORBIT_CAMERA_HPP )
{
    emscripten::class_<OrbitCameraControl>( "OrbitCameraControl" )
        .smart_ptr_constructor(
            "OrbitCameraControl", &std::make_shared<OrbitCameraControl, Ptr<Camera>, const glm::vec3, f32> )
        .property( "focus", &OrbitCameraControl::m_focus )
        .property( "distance", &OrbitCameraControl::m_Distance )
        .function( "orbit", &OrbitCameraControl::orbit )
        .function( "getCamera", &OrbitCameraControl::getCamera, emscripten::allow_raw_pointers() )
        .function( "setCamera", &OrbitCameraControl::setCamera, emscripten::allow_raw_pointers() );
}
