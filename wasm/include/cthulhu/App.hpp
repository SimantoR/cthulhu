#ifndef APP_HPP
#define APP_HPP

#include <utils.h>
#include <thread_pool.h>
#include <glm/vec3.hpp>
#include "../aakara/Renderer.hpp"
#include "../aakara/Mesh.hpp"
#include "../aakara/Texture.hpp"
#include "../aakara/Renderer.hpp"
#include "../aakara/Transform.hpp"
#include "../aakara/Camera.hpp"
#include "../aakara/Lights.hpp"
#include "Part.hpp"

class App
{
public:
    App( string canvas_id, int width, int height );
    ~App();

    bool init();

    Ptr<Camera> getCamera();

    size_t draw();

    void setPartTransform( const string& id, const Transform& transform );

    void loadPart( const string& mesh_url, const string& texture_url, Ptr<Transform> transform, JSObject cb );
    void removePart( const string& id );

    Ptr<Renderer> getRenderer() const
    {
        return m_renderer;
    }

    Ptr<DirectionalLight> getLight()
    {
        return m_sunlight;
    }

    Ptr<Camera> m_camera = nullptr;

private:
    Ptr<Renderer>         m_renderer = nullptr;
    Ptr<DirectionalLight> m_sunlight = nullptr;

    std::map<string, Ptr<Part>> m_parts;
    std::queue<Ptr<Part>>       m_queuedParts;

    // mapped JS callbacks for loading parts
    std::map<string, JSObject> m_callbacks;

    thread_pool m_threads;

    void clearById( const string& id );
    void _processQueue();
};

#endif
