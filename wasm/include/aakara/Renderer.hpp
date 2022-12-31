#ifndef RENDERER_H
#define RENDERER_H

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Lights.hpp"
#include "Texture.hpp"
#include "Skybox.hpp"

#include <queue>
#include <utils.h>
#include <glm/glm.hpp>
#include <emscripten/bind.h>

class Transform;

/**
 * @brief A struct representing a render command
 *
 */
struct RenderCmd
{
    Ptr<Mesh>      mesh      = nullptr;
    Ptr<Transform> transform = nullptr;
    Ptr<Texture>   texture   = nullptr;

    RenderCmd()
    {
    }

    RenderCmd( Ptr<Mesh> mesh, Ptr<Texture> texture, Ptr<Transform> transform )
        : mesh( mesh )
        , texture( texture )
        , transform( transform )
    {
    }
};

class Renderer
{
public:
    Renderer( const std::string& id, int width, int height );

    /**
     * @brief Initial renderer using default shaders
     *
     * @return Boolean indicating if initialization is successful.
     */
    bool init();

    /**
     * @brief Set the background color of viewport
     *
     * @param r Red value between 0-255
     * @param g Green value between 0-255
     * @param b Blue value between 0-255
     */
    void setColor( f32 r, f32 g, f32 b );

    void activateContext();

    /**
     * @brief Draw submitted render jobs.
     *
     * @param queue Render commands to issue to the renderer.
     * @param camera Main camera to use for rendering.
     * @param light Main light to use for rendering.
     */
    void drawItems( std::queue<RenderCmd>& queue, Ptr<Camera> camera, Ptr<Light> light );

    Ptr<Shader> GetShader()
    {
        return m_shader;
    }

private:
    int m_width = -1, m_height = -1;

    int m_glContext = -1;

    glm::vec<3, f32, glm::packed_lowp> m_color;

    Ptr<Shader>  m_shader = nullptr;
    UPtr<Skybox> m_skybox = nullptr;
};

#endif
