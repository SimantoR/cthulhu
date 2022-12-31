#ifndef TEXTURE_H
#define TEXTURE_H

#include <utils.h>

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG

// LINK: https://docs.gl/es2/glTexImage2D
class Texture
{
public:
    enum class PixelType
    {
        RGB  = 3,
        RGBA = 4
    };

    /**
     * @brief Construct a new Texture object
     *
     * @param pixels pixel data in unsigned int array
     * @param width width of image in pixels
     * @param height height of image in pixels
     */
    Texture( const Vector<u8>& pixels, int width, int height, PixelType pixelType );
    ~Texture();

    // static Ptr<Texture> LoadFromURL( const std::string& url );

    /**
     * @brief Load a new texture from memory.
     *
     * @param data Unsigned char pointer containing encoded image.
     * @param size Size of the buffer.
     * @return Ptr<Texture> A shared pointer to the texture
     * @throws std::runtime_error if the image decoding failed
     */
    static Ptr<Texture> LoadFromMemory( const u8* data, u64 size );

    static Ptr<Texture> LoadFromFile (const string& filepath);

    /**
     * @brief Update texture in WebGL
     *
     */
    void update();

    /**
     * @brief Get the Texture Id as unsigned int
     *
     */
    u32 getTextureId();

    PixelType GetFormat();

    u32 GetWidth()
    {
        return m_width;
    }

    u32 GetHeight()
    {
        return m_height;
    }

    void Bind();

private:
    u32        m_textureId = 0;
    int        m_width = 0, m_height = 0;
    int        m_format = 0;
    Vector<u8> m_pixelBuffer;
};

#endif
