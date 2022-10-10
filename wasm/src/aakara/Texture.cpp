#define STB_IMAGE_IMPLEMENTATION

#include <exception>
#include <emscripten/fetch.h>
#include <webgl/webgl1.h>

#include <aakara/Texture.hpp>
#include <aakara/fetch.hpp>
#include <stbi_image.h>

Texture::Texture( const std::vector<u8>& pixels, int width, int height, PixelType pixelType )
    : m_pixelBuffer( pixels )
    , m_width( width )
    , m_height( height )
{
    m_format = pixelType == PixelType::RGBA ? GL_RGBA : GL_RGB;
}

Texture::~Texture()
{
    glDeleteTextures( 1, &m_textureId );
};

u32 Texture::getTextureId()
{
    return m_textureId;
}

void Texture::Bind()
{
    if ( m_textureId == 0 )
    {
        emscripten_console_warn( "Invalid texture attempting to bind..." );
        return;
    }

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_textureId );
}

void Texture::update()
{
    if ( m_textureId )
        return;

    u32 texId = 0;
    glGenTextures( 1, &texId );
    glBindTexture( GL_TEXTURE_2D, texId );

    glTexImage2D(
        GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, m_pixelBuffer.data() );

    if ( m_width % 2 == 0 && m_height % 2 == 0 )
    {
        glGenerateMipmap( GL_TEXTURE_2D );
    }
    else
    {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    m_textureId = texId;
}

Ptr<Texture> Texture::LoadFromMemory( const u8* data, u64 size )
{
    int width, height;
    int channel;

    stbi_set_flip_vertically_on_load( 1 );
    u8* buffer = stbi_load_from_memory( data, size, &width, &height, &channel, STBI_rgb_alpha );

    if ( buffer == nullptr )
        throw std::runtime_error( stbi_failure_reason() );

    if ( channel == STBI_rgb_alpha )
    {
        channel = GL_RGBA;
    }
    else
    {
        channel = GL_RGB;
    }

    std::vector<u8> textureBuffer( width * height * 4 );
    std::memcpy( textureBuffer.data(), buffer, 4 * width * height * sizeof( u8 ) );

    stbi_image_free( buffer );

    Ptr<Texture> texture = std::make_shared<Texture>( textureBuffer, width, height,
        channel == GL_RGBA ? Texture::PixelType::RGBA : Texture::PixelType::RGB );
    texture->m_format    = channel;

    return texture;
}

Texture::PixelType Texture::GetFormat()
{
    switch ( m_format )
    {
    case GL_RGBA:
        return PixelType::RGBA;
        break;
    case GL_RGB:
    default:
        return PixelType::RGBA;
        break;
    }
}
