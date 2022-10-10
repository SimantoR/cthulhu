#ifndef Shader_HPP
#define Shader_HPP

#include <utils.h>
#include <glm/glm.hpp>

/**
 * @brief Representation of a set of vertex-fragment shader
 *
 */
class Shader
{
public:
    Shader( const std::string& vs, const std::string& fs );
    ~Shader();

    static Ptr<Shader> LoadFromFile( string vertexPath, string fragPath );

    /**
     * @brief Bind this shader to WebGL
     *
     */
    void Bind();

    /**
     * @brief Unbind this shader from WebGL
     *
     */
    void Unbind();

    /**
     * @brief Get shader ID
     *
     * @return u32 Shader program ID in OpenGL
     */
    u32 GetID()
    {
        return m_shaderID;
    }

    /**
     * @brief Get the Shader Id object
     * @deprecated This function is replaced by Shader::GetID()
     * @return u32 Shader program ID in OpenGL
     */
    u32 GetShaderId()
    {
        return m_shaderID;
    }

    void SetInt( std::string location, int value );
    void SetBool( std::string location, bool value );
    void SetFloat( std::string location, float value );
    void SetVector( std::string location, glm::vec2 value );
    void SetVector( std::string location, glm::vec3 value );
    void SetVector( std::string location, glm::vec4 value );
    void SetVectorArray( std::string location, int size, const std::vector<glm::vec2>& values );
    void SetVectorArray( std::string location, int size, const std::vector<glm::vec3>& values );
    void SetVectorArray( std::string location, int size, const std::vector<glm::vec4>& values );
    void SetMatrix( std::string location, const glm::mat2& value );
    void SetMatrix( std::string location, const glm::mat3& value );
    void SetMatrix( std::string location, const glm::mat4& value );
    void SetMatrixArray( std::string location, int size, glm::mat2* values );
    void SetMatrixArray( std::string location, int size, glm::mat3* values );
    void SetMatrixArray( std::string location, int size, glm::mat4* values );
    void SetVertexAttribute( const std::string& name, u32 size, u32 stride );

    bool EnableVertexAttribute( string location );

private:
    u32 m_shaderID;

    void Load( const char* vert_shader_file, const char* frag_shader_file );
};

#endif
