#ifndef LIGHTS_HPP
#define LIGHTS_HPP

#include <utils.h>
#include <glm/vec3.hpp>

class Shader;

class Light
{
public:
    glm::vec3 Color;
    float     Strength;

    Light( const glm::vec3& color, float strength )
        : Color( color )
        , Strength( strength ) {};

    /**
     * @brief Bind this light to given shader.
     * 
     * @param shader Shader to bind the light to.
     */
    virtual void Bind( Shader* shader ) = 0;
};

class DirectionalLight : public Light
{
public:
    glm::vec3 Direction;

    DirectionalLight( const glm::vec3& direction, const glm::vec3& color, float strength );
    void Bind( Shader* shader ) override;
};

#endif
