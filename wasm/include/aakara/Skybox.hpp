#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <utils.h>
#include "Shader.hpp"

class Camera;

class Skybox
{
public:
    Skybox();
    Skybox( const string left, const string front, const string top, const string bottom, const string right,
        const string back );

    /**
     * @brief Draws the skybox prior to drawing the scene.
     *
     * @param camera Camera through which the skybox will be rendered.
     */
    void Draw( Ptr<Camera> camera );

private:
    u32         m_textureID    = 0;
    u32         m_VBO          = 0;
    Ptr<Shader> m_skyboxShader = nullptr;
};

#endif
