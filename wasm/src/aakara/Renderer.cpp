#include <functional>
#include <exception>
#include <emscripten/html5.h>
#include <emscripten/fetch.h>
#include <emscripten/bind.h>
#include <webgl/webgl1.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <aakara/Renderer.hpp>
#include <aakara/Texture.hpp>
#include <aakara/fetch.hpp>
#include <aakara/Transform.hpp>

std::string readFile( FILE* file )
{
    fseek( file, 0L, SEEK_END );
    long buffSize = ftell( file );
    rewind( file );

    std::string buffer( buffSize, '\0' );

    fread( (void*)buffer.c_str(), 1, buffSize, file );

    return buffer;
}

Renderer::Renderer( const std::string& id, int width, int height )
    : m_width( width )
    , m_height( height )
    , m_color( 1.0f, 0.0f, 0.0f )
{
    /* --------------------------- Get OpenGL Context --------------------------- */
    EmscriptenWebGLContextAttributes attrs;
    attrs.explicitSwapControl          = 0;
    attrs.depth                        = 1;
    attrs.stencil                      = 1;
    attrs.antialias                    = 1;
    attrs.majorVersion                 = 1;
    attrs.minorVersion                 = 0;
    attrs.renderViaOffscreenBackBuffer = 0;

    m_glContext = emscripten_webgl_create_context( id.c_str(), &attrs );
    if ( m_glContext < 0 )
    {
        emscripten_console_errorf( "WASM:: Failed to create webgl context %d\n", m_glContext );
        return;
    }

    activateContext();

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );

    glEnable( GL_CULL_FACE );

    glViewport( 0, 0, m_width, m_height );

    glClearColor( m_color.r, m_color.g, m_color.b, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    emscripten_console_warn( "Initializing Skybox..." );

    try
    {
        m_skybox = std::make_unique<Skybox>();
    }
    catch ( const std::exception& err )
    {
        emscripten_console_error( err.what() );
        throw err;
    }

    emscripten_console_logf(
        "WASM:: OpenGL version supported by this platform : %s\n", glGetString( GL_VERSION ) );
}

bool Renderer::init()
{
    try
    {
        activateContext();
        m_shader = Shader::LoadFromFile( "/shaders/standard.vert", "/shaders/standard.frag" );
        return true;
    }
    catch ( const std::exception& e )
    {
        emscripten_console_error( e.what() );
        return false;
    }
}

void Renderer::drawItems( std::queue<RenderCmd>& queue, Ptr<Camera> camera, Ptr<Light> light )
{
    activateContext();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glViewport( 0, 0, camera->Viewport.x, camera->Viewport.y );

    Ptr<Mesh>      mesh      = nullptr;
    Ptr<Transform> transform = nullptr;
    Ptr<Texture>   texture   = nullptr;

    m_skybox->Draw( camera );

    u32 shaderId = m_shader->GetID();
    m_shader->Bind();
    {
        light->Bind( m_shader.get() );

        while ( !queue.empty() )
        {
            // log_stream.clear();
            RenderCmd cmd = queue.front();
            queue.pop();

            mesh      = cmd.mesh;
            transform = cmd.transform;
            texture   = cmd.texture;

            if ( !mesh->Bind( m_shader.get() ) )
            {
                emscripten_console_error( "Failed to bind mesh" );
                continue;
            }

            texture->Bind();
            m_shader->SetInt( "diffuseTex", 0 );

            glm::mat4 view       = camera->GetView();
            glm::mat4 projection = camera->GetPerspectiveProjection();

            glm::mat4 model( 1.0f );

            model = glm::translate( model, transform->position );

            // forward = z, right = x, top = y
            model = glm::rotate( model, glm::radians( transform->rotation.x ), { 0.0f, 1.0f, 0.0f } );
            model = glm::rotate( model, glm::radians( transform->rotation.y ), { 0.0f, 0.0f, 1.0f } );
            model = glm::rotate( model, glm::radians( transform->rotation.z ), { 1.0f, 0.0f, 0.0f } );

            model = glm::scale( model, transform->scale );

            m_shader->SetMatrix( "model", model );
            m_shader->SetMatrix( "view", view );
            m_shader->SetMatrix( "proj", projection );

            mesh->Draw();
        }
    }
    m_shader->Unbind();
}

void Renderer::setColor( f32 r, f32 g, f32 b )
{
    activateContext();

    m_color = { r, g, b };
    glClearColor( r, g, b, 1.0f );
}

void Renderer::activateContext()
{
    EMSCRIPTEN_RESULT r = emscripten_webgl_make_context_current( m_glContext );
    if ( r < 0 )
        emscripten_console_errorf( "failed to make webgl current %d\n", r );
}

EMSCRIPTEN_BINDINGS( RENDERER_H )
{
    emscripten::class_<Renderer>( "Renderer" )
        .smart_ptr_constructor( "Renderer", &std::make_shared<Renderer, string, int, int> )
        .function( "setColor", &Renderer::setColor );

    emscripten::value_object<RenderCmd>( "RenderCmd" )
        .field( "mesh", &RenderCmd::mesh )
        .field( "transform", &RenderCmd::transform )
        .field( "texture", &RenderCmd::texture );
}
