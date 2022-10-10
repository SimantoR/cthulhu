#include <emscripten/html5.h>
#include <webgl/webgl1.h>
#include <glm/ext.hpp>
#include <fstream>
#include <exception>

#include <aakara/Shader.hpp>

Shader::Shader( const std::string& vs, const std::string& fs )
{
    Load( vs.c_str(), fs.c_str() );
}

Shader::~Shader()
{
    glDeleteProgram( m_shaderID );
}

Ptr<Shader> Shader::LoadFromFile( string vertPath, string fragPath )
{
    std::fstream stream;
    size_t       len = 0;

    /* --------------------------- Read vertex shader --------------------------- */
    stream.open( vertPath, std::ios::in );

    stream.seekg( 0, std::ios::end );
    len = stream.tellg();
    stream.seekg( 0, std::ios::beg );

    string vs_code( len, '\0' );
    stream.read( vs_code.data(), len );
    stream.close();

    /* -------------------------- Read fragment shader -------------------------- */
    stream.open( fragPath, std::ios::in );

    stream.seekg( 0, std::ios::end );
    len = stream.tellg();
    stream.seekg( 0, std::ios::beg );

    string fs_code( len, '\0' );
    stream.read( fs_code.data(), len );
    stream.close();

    return std::make_shared<Shader>( vs_code, fs_code );
}

void Shader::Bind()
{
    if ( m_shaderID == 0 )
        emscripten_console_error( "Failed to bind shader!" );
    else
        glUseProgram( m_shaderID );
}

void Shader::Unbind()
{
    glUseProgram( 0 );
}

void Shader::SetInt( std::string location, int value )
{
    int loc = glGetUniformLocation( m_shaderID, location.c_str() );
    if ( loc >= 0 )
        glUniform1i( loc, value );
}

void Shader::SetBool( std::string location, bool value )
{
    int loc = glGetUniformLocation( m_shaderID, location.c_str() );
    if ( loc >= 0 )
        glUniform1i( loc, (int)value );
}

void Shader::SetFloat( std::string location, float value )
{
    int loc = glGetUniformLocation( m_shaderID, location.c_str() );
    if ( loc >= 0 )
        glUniform1f( loc, value );
}

void Shader::SetVector( std::string location, glm::vec2 value )
{
    int loc = glGetUniformLocation( m_shaderID, location.c_str() );
    if ( loc >= 0 )
        glUniform2fv( loc, 1, &value[0] );
}

void Shader::SetVector( std::string location, glm::vec3 value )
{
    int loc = glGetUniformLocation( m_shaderID, location.c_str() );
    if ( loc >= 0 )
        glUniform3fv( loc, 1, &value[0] );
}

void Shader::SetVector( std::string location, glm::vec4 value )
{
    int loc = glGetUniformLocation( m_shaderID, location.c_str() );
    if ( loc >= 0 )
        glUniform4fv( loc, 1, &value[0] );
}

void Shader::SetVectorArray( std::string location, int size, const std::vector<glm::vec2>& values )
{
    unsigned int loc = glGetUniformLocation( m_shaderID, location.c_str() );
    if ( loc >= 0 )
    {
        glUniform2fv( loc, size, (float*)( &values[0].x ) );
    }
}

void Shader::SetVectorArray( std::string location, int size, const std::vector<glm::vec3>& values )
{
    unsigned int loc = glGetUniformLocation( m_shaderID, location.c_str() );
    if ( loc >= 0 )
    {
        glUniform3fv( loc, size, (float*)( &values[0].x ) );
    }
}

void Shader::SetVectorArray( std::string location, int size, const std::vector<glm::vec4>& values )
{
    unsigned int loc = glGetUniformLocation( m_shaderID, location.c_str() );
    if ( loc >= 0 )
    {
        glUniform4fv( loc, size, (float*)( &values[0].x ) );
    }
}

void Shader::SetMatrix( std::string location, const glm::mat2& value )
{
    int loc = glGetUniformLocation( m_shaderID, location.c_str() );
    if ( loc >= 0 )
        glUniformMatrix2fv( loc, 1, GL_FALSE, glm::value_ptr( value ) );
}

void Shader::SetMatrix( std::string location, const glm::mat3& value )
{
    int loc = glGetUniformLocation( m_shaderID, location.c_str() );
    if ( loc >= 0 )
        glUniformMatrix3fv( loc, 1, GL_FALSE, glm::value_ptr( value ) );
}

void Shader::SetMatrix( std::string location, const glm::mat4& value )
{
    int loc = glGetUniformLocation( m_shaderID, location.c_str() );
    if ( loc >= 0 )
        glUniformMatrix4fv( loc, 1, GL_FALSE, glm::value_ptr( value ) );
}

void Shader::SetVertexAttribute( const std::string& name, u32 size, u32 stride )
{
    GLint attribLocation = glGetAttribLocation( m_shaderID, name.c_str() );
    glVertexAttribPointer( //
        attribLocation,    // location
        size,              // size
        GL_FLOAT,          // type of data in buffer
        GL_FALSE,          // normalize
        stride,            // stride
        0                  // offset in buffer
    );
    glEnableVertexAttribArray( attribLocation );
}

bool Shader::EnableVertexAttribute( string location )
{
    i32 locationIndex = glGetAttribLocation( m_shaderID, location.c_str() );
    if ( !locationIndex )
        return false;

    glEnableVertexAttribArray( locationIndex );
    return true;
}

void Shader::Load( const char* vertexCode, const char* fragCode )
{
    GLenum ErrorCheckValue  = glGetError();
    GLint  gl_shader_result = GL_FALSE;
    int    InfoLogLength;

    u32 vertexShaderId = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertexShaderId, 1, &vertexCode, NULL );
    glCompileShader( vertexShaderId );

    // check for vertex shader errors
    glGetShaderiv( vertexShaderId, GL_COMPILE_STATUS, &gl_shader_result );
    glGetShaderiv( vertexShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength );

    if ( InfoLogLength > 0 )
    {
        std::string msg( InfoLogLength + 1, '\0' );

        glGetShaderInfoLog( vertexShaderId, InfoLogLength, NULL, (GLchar*)msg.c_str() );

        throw std::runtime_error( ( "WASM:: Vertex shader error: " + msg ) );
    }

    u32 fragmentShaderId = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragmentShaderId, 1, &fragCode, NULL );
    glCompileShader( fragmentShaderId );

    // check for vertex shader errors
    glGetShaderiv( fragmentShaderId, GL_COMPILE_STATUS, &gl_shader_result );
    glGetShaderiv( fragmentShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength );

    if ( InfoLogLength > 0 )
    {
        std::string msg( InfoLogLength + 1, '\0' );

        glGetShaderInfoLog( fragmentShaderId, InfoLogLength, NULL, (GLchar*)msg.c_str() );

        throw std::runtime_error( ( "WASM:: Fragment shader error: " + msg ) );
    }

    u32 shaderProgramId = glCreateProgram();
    glAttachShader( shaderProgramId, vertexShaderId );
    glAttachShader( shaderProgramId, fragmentShaderId );
    glLinkProgram( shaderProgramId );

    // // Check the program
    // glGetProgramiv( shaderProgramId, GL_LINK_STATUS, &gl_shader_result );
    // glGetProgramiv( shaderProgramId, GL_INFO_LOG_LENGTH, &InfoLogLength );
    // if ( InfoLogLength > 0 )
    // {
    //     std::string msg( InfoLogLength + 1, '\0' );

    //     glGetProgramInfoLog( shaderProgramId, InfoLogLength, NULL, (GLchar*)msg.c_str() );

    //     emscripten_console_error( ( "WASM:: Shader compilation error: " + msg ).c_str() );

    //     return;
    // }

    glDetachShader( shaderProgramId, vertexShaderId );
    glDetachShader( shaderProgramId, fragmentShaderId );

    glDeleteShader( vertexShaderId );
    glDeleteShader( fragmentShaderId );

    m_shaderID = shaderProgramId;
}
