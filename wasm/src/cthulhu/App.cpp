#include <cthulhu/App.hpp>

#include <strstream>
#include <fstream>
#include <emscripten/fetch.h>
#include <emscripten/bind.h>
#include <aakara/Shader.hpp>
#include <aakara/Mesh.hpp>
#include <aakara/Camera.hpp>
#include <aakara/Renderer.hpp>
#include <aakara/Texture.hpp>
#include <aakara/Global.hpp>
#include <aakara/fetch.hpp>
#include <glm/gtx/string_cast.hpp>
#include <uuid.hpp>

#define PTHREAD_POOL_SIZE 4

App::App( string canvas_id, int width, int height )
    : m_threads( PTHREAD_POOL_SIZE )
    , m_parts()
    , m_queuedParts()
    , m_callbacks()
{
    m_renderer = std::make_shared<Renderer>( canvas_id, width, height );

    emscripten_console_logf(
        "Initializing renderer with %s ...", std::to_string( PTHREAD_POOL_SIZE ).c_str() );
    m_renderer->init();

    m_camera           = std::make_shared<Camera>();
    m_camera->Viewport = { width, height };

    m_sunlight = std::make_shared<DirectionalLight>(
        glm::vec3( 1.0f, 0.0f, 0.0f ), glm::vec3( 1.0f, 0.89f, 0.69f ), 1.0f );

    std::stringstream ss;

    ss << "Author:        Simanto Rahman" << std::endl;
    ss << "Project:       Cthulhu" << std::endl;
    ss << "Version:       0.1.0 (beta)" << std::endl;
    ss << "All Rights Reserved by the Author" << std::endl;

    emscripten_console_log( ss.str().c_str() );
}

App::~App()
{
    m_parts.clear();
}

Ptr<Camera> App::getCamera()
{
    return m_camera;
}

void App::setPartTransform( const string& id, const Transform& transform )
{
    if ( m_parts.find( id ) == m_parts.end() )
        return;

    Ptr<Transform> part_transform = m_parts[id]->transform;

    part_transform->position = transform.position;
    part_transform->rotation = transform.rotation;
    part_transform->scale    = transform.scale;
}

void App::loadPart( const string& mesh_url, const string& texture_url, Ptr<Transform> transform, JSObject cb )
{
    string id;
    do
    {
        id = uuid::generate();
    } while ( m_parts.find( id ) != m_parts.end() );

    m_callbacks.insert( std::map<string, JSObject>::value_type( id, JSObject( cb ) ) );

    auto LoadPartFn = [id, this]( string mesh_url, string texture_url, Ptr<Transform> transform )
    {
        emscripten_fetch_attr_t attr;
        emscripten_fetch_t*     fetch = nullptr;
        try
        {
            /* ------------------------------- Fetch mesh ------------------------------- */
            emscripten_fetch_attr_init( &attr );

            attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
            std::strcpy( attr.requestMethod, "GET" );

            fetch = emscripten_fetch( &attr, mesh_url.c_str() );

            if ( fetch->status != 200 || fetch->numBytes == 0 )
                throw std::runtime_error( "Mesh fetch failed" );

            Ptr<Mesh> mesh = Mesh::LoadFromMemory( fetch->data, fetch->numBytes );

            emscripten_fetch_close( fetch );
            fetch = nullptr;

            /* ------------------------------ Fetch texture ----------------------------- */
            emscripten_fetch_attr_init( &attr );

            attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
            std::strcpy( attr.requestMethod, "GET" );

            fetch = emscripten_fetch( &attr, texture_url.c_str() );

            if ( fetch->status != 200 || fetch->numBytes == 0 )
                throw std::runtime_error( "Texture fetch failed" );

            Ptr<Texture> texture
                = Texture::LoadFromMemory( reinterpret_cast<const u8*>( fetch->data ), fetch->numBytes );

            emscripten_fetch_close( fetch );
            fetch = nullptr;

            /* --------------------------- Create and queue part -------------------------- */
            Ptr<Part> part = std::make_shared<Part>( id, mesh, texture, transform );

            this->m_queuedParts.push( part );
        }
        catch ( const std::exception& err )
        {
            emscripten_console_errorf( "Failed to load mesh: %s", err.what() );
            return;
        }
    };

    m_threads.push_task( LoadPartFn, mesh_url, texture_url, transform );
}

void App::removePart( const string& id )
{
    clearById( id );
}

size_t App::draw()
{
    _processQueue();

    std::queue<RenderCmd> queue;

    for ( auto [id, part] : m_parts )
        queue.emplace( part->mesh, part->texture, part->transform );

    // emscripten_console_logf( "Drawing %lu items...", queue.size() );

    size_t queue_size = queue.size();
    m_renderer->drawItems( queue, m_camera, m_sunlight );

    Global::Time::Reset();

    return queue_size;
}

void App::clearById( const string& id )
{
    if ( m_parts.find( id ) != m_parts.end() )
        m_parts.erase( id );
}

void App::_processQueue()
{
    while ( !m_queuedParts.empty() )
    {
        Ptr<Part> part = m_queuedParts.front();
        m_queuedParts.pop();

        string& id = part->id;

        part->mesh->update( m_renderer->GetShader().get() );
        part->texture->update();

        emscripten_console_logf( "Part loaded with vertex count: %lu", part->mesh->Vertices.size() );

        // m_parts.insert( std::map<string, Part>::value_type( id, part ) );

        m_parts[id] = part;

        // call the JS callback function for this given part
        JSObject onPartLoad( m_callbacks.find( id )->second );

        onPartLoad( part );
        m_callbacks.erase( id );
    }
}

void loadScene( string fbxUrl, JSObject callback )
{
}

Ptr<Camera> createCamera( Ptr<App> app )
{
    return std::make_shared<Camera>();
}

void setMainCamera( Ptr<App> app, Ptr<Camera> camera )
{
}

Ptr<DirectionalLight> createLight( Ptr<App> app, glm::vec3 direction, glm::vec3 color, float strength )
{
    return std::make_shared<DirectionalLight>( direction, color, strength );
}

EMSCRIPTEN_BINDINGS( APP_HPP )
{
    emscripten::class_<App>( "App" )
        .smart_ptr_constructor( "App", &std::make_shared<App, string, int, int> )
        .function( "getRenderer", &App::getRenderer )
        .function( "getCamera", &App::getCamera )
        .function( "getLight", &App::getLight )
        .function( "draw", &App::draw )
        // .function( "createCamera", &createCamera )
        // .function( "setMainCamera", &setMainCamera )
        .function( "createLight", &createLight )
        // .function( "deltaTime", &Global::Time::DeltaTime )
        .function( "setTransform", &App::setPartTransform )
        .function( "loadPart", &App::loadPart )
        .function( "removePart", &App::removePart );

    emscripten::value_object<glm::vec3>( "Vec3" )
        .field( "x", &glm::vec3::x )
        .field( "y", &glm::vec3::y )
        .field( "z", &glm::vec3::z );

    emscripten::value_object<glm::vec2>( "Vec2" ) //
        .field( "x", &glm::vec2::x )
        .field( "y", &glm::vec2::y );

    emscripten::class_<std::vector<u8>>( "ByteArray" )
        .smart_ptr_constructor( "ByteArray", std::make_shared<std::vector<u8>, int> );
}
