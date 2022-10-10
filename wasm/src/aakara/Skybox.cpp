#include <aakara/Skybox.hpp>
#include <aakara/Camera.hpp>

#include <iostream>
#include <fstream>
#include <strstream>
#include <functional>
#include <filesystem>
#include <stbi_image.h>
#include <webgl/webgl2.h>
#include <glm/glm.hpp>
#include <emscripten/html5.h>

namespace fs = std::filesystem;

Ptr<Shader> LoadSkyboxShader();

Skybox::Skybox()
    : Skybox( "/skyboxes/stormy_day/left.jpg", "/skyboxes/stormy_day/front.jpg",
        "/skyboxes/stormy_day/top.jpg", "/skyboxes/stormy_day/bottom.jpg",
        "/skyboxes/stormy_day/right.jpg", "/skyboxes/stormy_day/back.jpg" )
{
}

Skybox::Skybox( const string left, const string front, const string top, const string bottom,
    const string right, const string back )
{
    Array<float> skyboxVertices = {
        // positions
        -1.0f, +1.0f, -1.0f, //
        -1.0f, -1.0f, -1.0f, //
        +1.0f, -1.0f, -1.0f, //
        +1.0f, -1.0f, -1.0f, //
        +1.0f, +1.0f, -1.0f, //
        -1.0f, +1.0f, -1.0f, //

        -1.0f, -1.0f, +1.0f, //
        -1.0f, -1.0f, -1.0f, //
        -1.0f, +1.0f, -1.0f, //
        -1.0f, +1.0f, -1.0f, //
        -1.0f, +1.0f, +1.0f, //
        -1.0f, -1.0f, +1.0f, //

        +1.0f, -1.0f, -1.0f, //
        +1.0f, -1.0f, +1.0f, //
        +1.0f, +1.0f, +1.0f, //
        +1.0f, +1.0f, +1.0f, //
        +1.0f, +1.0f, -1.0f, //
        +1.0f, -1.0f, -1.0f, //

        -1.0f, -1.0f, +1.0f, //
        -1.0f, +1.0f, +1.0f, //
        +1.0f, +1.0f, +1.0f, //
        +1.0f, +1.0f, +1.0f, //
        +1.0f, -1.0f, +1.0f, //
        -1.0f, -1.0f, +1.0f, //

        -1.0f, +1.0f, -1.0f, //
        +1.0f, +1.0f, -1.0f, //
        +1.0f, +1.0f, +1.0f, //
        +1.0f, +1.0f, +1.0f, //
        -1.0f, +1.0f, +1.0f, //
        -1.0f, +1.0f, -1.0f, //

        -1.0f, -1.0f, -1.0f, //
        -1.0f, -1.0f, 1.0f,  //
        +1.0f, -1.0f, -1.0f, //
        +1.0f, -1.0f, -1.0f, //
        -1.0f, -1.0f, 1.0f,  //
        +1.0f, -1.0f, 1.0f,  //
    };

    glGenTextures( 1, &m_textureID );
    glBindTexture( GL_TEXTURE_CUBE_MAP, m_textureID );

    int width, height, channel;
    u8* pixels = nullptr;

    std::function<void( const string, size_t )> LoadFace = []( const string imgpath, size_t i )
    {
        int width, height, channel;
        u8* pixels = stbi_load( imgpath.c_str(), &width, &height, &channel, STBI_rgb );

        glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB,
            GL_UNSIGNED_BYTE, pixels );

        stbi_image_free( pixels );
    };

    LoadFace( right, 0 );
    LoadFace( left, 1 );
    LoadFace( top, 2 );
    LoadFace( bottom, 3 );
    LoadFace( back, 4 );
    LoadFace( front, 5 );

    glGenerateMipmap( GL_TEXTURE_CUBE_MAP );

    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

    m_skyboxShader = LoadSkyboxShader();

    glGenBuffers( 1, &m_VBO );
    glBindBuffer( GL_ARRAY_BUFFER, m_VBO );
    glBufferData(
        GL_ARRAY_BUFFER, sizeof( float ) * skyboxVertices.size(), skyboxVertices.data(), GL_STATIC_DRAW );
}

void Skybox::Draw( Ptr<Camera> camera )
{

    // glm::mat4 view = camera->GetView();
    glm::mat4 view = glm::mat4( glm::mat3( camera->GetView() ) );
    glm::mat4 proj = camera->GetPerspectiveProjection();

    glDepthMask( GL_FALSE );

    m_skyboxShader->Bind();
    u32 shaderId = m_skyboxShader->GetID();

    m_skyboxShader->SetMatrix( "view", view );
    m_skyboxShader->SetMatrix( "projection", proj );

    // Bind vertex buffer and assign it to `v_position` variable in shader
    glBindBuffer( GL_ARRAY_BUFFER, m_VBO );
    m_skyboxShader->EnableVertexAttribute( "v_position" );
    m_skyboxShader->SetVertexAttribute( "v_position", 3, 3 * sizeof( float ) );

    glBindTexture( GL_TEXTURE_CUBE_MAP, m_textureID );
    glDrawArrays( GL_TRIANGLES, 0, 36 );
    glDepthMask( GL_TRUE );

    m_skyboxShader->Unbind();
}

Ptr<Shader> LoadSkyboxShader()
{
    std::fstream stream;
    size_t       len = 0;

    /* --------------------------- Read vertex shader --------------------------- */
    stream.open( "/shaders/skybox.vert", std::ios::in );

    stream.seekg( 0, std::ios::end );
    len = stream.tellg();
    stream.seekg( 0, std::ios::beg );

    string vs_code( len, '\0' );
    stream.read( vs_code.data(), len );
    stream.close();

    /* -------------------------- Read fragment shader -------------------------- */
    stream.open( "/shaders/skybox.frag", std::ios::in );

    stream.seekg( 0, std::ios::end );
    len = stream.tellg();
    stream.seekg( 0, std::ios::beg );

    string fs_code( len, '\0' );
    stream.read( fs_code.data(), len );
    stream.close();

    return std::make_shared<Shader>( vs_code, fs_code );
}
