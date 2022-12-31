#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "Texture.hpp"
#include "Shader.hpp"

class Material
{
public:
    virtual void UpdateShaderVariables() = 0;
};

class StandardMaterial : public Material
{
public:
    void UpdateShaderVariables() override;
};

#endif
