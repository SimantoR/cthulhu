#include <emscripten/bind.h>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/geometric.hpp>

#include <aakara/Lights.hpp>
#include <aakara/Shader.hpp>

DirectionalLight::DirectionalLight( const glm::vec3& direction, const glm::vec3& color, float strength )
    : Light( color, strength )
    , Direction( direction )
{
}

void DirectionalLight::Bind( Shader* shader )
{
    shader->SetVector( "light.direction", this->Direction );
    shader->SetVector( "light.color", this->Color );
    shader->SetFloat( "light.strength", this->Strength );
}

glm::vec3 getColor( Ptr<Light> light )
{
    return light->Color;
}

void setColor( Ptr<Light> light, glm::vec3 color )
{
    light->Color = color;
}

float getStrength( Ptr<Light> light )
{
    return light->Strength;
}

void setStrength( Ptr<Light> light, float strength )
{
    light->Strength = strength;
}

glm::vec3 getDirection( Ptr<DirectionalLight> light )
{
    return light->Direction;
}

void setDirection( Ptr<DirectionalLight> light, glm::vec3 dir )
{
    light->Direction = dir;
}

void setRotation( Ptr<DirectionalLight> light, glm::vec3 rotation )
{
    glm::vec3 direction = {
        cos( glm::radians( rotation.y ) ) * sin( glm::radians( rotation.z ) ),
        cos( glm::radians( rotation.y ) ) * cos( glm::radians( rotation.z ) ),
        sin( glm::radians( rotation.y ) ),
    };

    light->Direction = glm::normalize( direction );
}

EMSCRIPTEN_BINDINGS( LIGHTS_HPP )
{
    emscripten::class_<DirectionalLight>( "Light" )
        .smart_ptr_constructor(
            "Light", &std::make_shared<DirectionalLight, const glm::vec3&, const glm::vec3&, float> )
        .function( "getStrength", &getStrength )
        .function( "setStrength", &setStrength )
        // .property( "strength", &DirectionalLight::Strength )
        .function( "getColor", &getColor )
        .function( "setColor", &setColor )
        .function( "getDirection", &getDirection )
        .function( "setDirection", &setDirection )
        .function( "setRotation", &setRotation );
}
