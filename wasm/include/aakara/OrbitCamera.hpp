#ifndef ORBIT_CAMERA_HPP
#define ORBIT_CAMERA_HPP

#include <utils.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Camera;

class OrbitCameraControl
{
public:
    OrbitCameraControl( Ptr<Camera> camera, const glm::vec3 focus, f32 distance = 10.0f );

    void orbit( float x, float y );

    Ptr<Camera> getCamera();
    void        setCamera( Ptr<Camera> );

    glm::vec3 getFocus();
    void      setFocus( const glm::vec3 focus );

    glm::vec3 m_focus = glm::vec3( 0.0f );
    f32       m_Distance;

private:
    Ptr<Camera> m_camera      = nullptr;
    glm::vec2   m_orbitAngles = glm::vec3( 0.0f );
};

#endif
