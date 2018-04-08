
#pragma once


#include <memory>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <openll/openll_api.h>


namespace globjects
{
    class Texture;
    class Buffer;
    class VertexArray;
}


namespace openll
{


class FontFace;
class GlyphSequence;


/**
*  @brief
*    Vertex array that describes each glyph to be rendered on the screen
*/
class OPENLL_API GlyphVertexCloud
{
public:
    /**
    *  @brief
    *    Data for a single vertex (glyph) on the screen
    */
    struct Vertex
    {
        glm::vec3 origin;    ///< Position of the glyph in normalized device coordinates
        glm::vec3 vtan;      ///< Tangent vector
        glm::vec3 vbitan;    ///< Bitangent vector
        glm::vec4 uvRect;    ///< Source image rect of the glyph in the glyph texter (uv-coordinates)
        glm::vec4 fontColor; ///< Color of the glyph
    };


public:
    /**
    *  @brief
    *    Constructor
    *
    *  @remarks
    *    This allocates OpenGL objects, so an OpenGL context must be current when creating this object.
    */
    GlyphVertexCloud();

    /**
    *  @brief
    *    Destructor
    *
    *  @remarks
    *    This allocates OpenGL objects, so an OpenGL context must be current when deleting this object.
    */
    virtual ~GlyphVertexCloud();

    // Forbid copying
    GlyphVertexCloud & operator=(const GlyphVertexCloud &) = delete;

    /**
    *  @brief
    *    Get vertices (in CPU memory)
    *
    *  @return
    *    Vertex list
    */
    const std::vector<Vertex> & vertices() const;

    /**
    *  @brief
    *    Get vertices (in CPU memory)
    *
    *  @return
    *    Vertex list
    */
    std::vector<Vertex> & vertices();

    /**
    *  @brief
    *    Get vertex array
    *
    *  @return
    *    Vertex array
    */
    const globjects::VertexArray * vao() const;

    /**
    *  @brief
    *    Get vertex array
    *
    *  @return
    *    Vertex array
    */
    globjects::VertexArray * vao();

    /**
    *  @brief
    *    Get glyph texture for which the text has been layouted
    *
    *  @return
    *    Glyph texture
    */
    const globjects::Texture * texture() const;

    /**
    *  @brief
    *    Set glyph texture for which the text has been layouted
    *
    *  @param[in] texture
    *    Glyph texture
    */
    void setTexture(globjects::Texture * texture);

    /**
    *  @brief
    *    Update VAO
    *
    *    Uploads the contents of the vertex list (see vao())
    *    onto the VAO on the GPU.
    *
    *  @param[in] texture
    *    Glyph texture
    */
    void update();

    /**
    *  @brief
    *    Update VAO
    *
    *    Uploads the contents of the vertex list (see vao())
    *    onto the VAO on the GPU.
    *
    *  @param[in] vertices
    *    Vertex list
    */
    void update(const std::vector<Vertex> & vertices);

    /**
    *  @brief
    *    Optimize vertex array for rendering
    *
    *    This optimizes the vertex array for rendering performance
    *    by sorting the glyphs in a way that all instances of a single
    *    glyph are rendered subsequently (texture cache optimization).
    *
    *  @param[in] sequences
    *    List of texts which are to be rendered
    *  @param[in] fontFace
    *    Font face to use
    *
    *  @remarks
    *    Keep in mind that optimizing the vertex array is a slow process
    *    itself, as each character of the text has to be processed and
    *    for each character it has to be determined, whether the glyph
    *    is visible or not. So the entire text has to be processed at
    *    least once. Therefore, optimization might not be advisable for
    *    large texts.
    */
    void optimize(const std::vector<GlyphSequence> & sequences, const FontFace & fontFace);

    /**
    *  @brief
    *    Draw glyph vertex array
    *
    *    This function draws the vertex array by means of OpenGL geometry.
    *    It only emits the geometry, but it doesn't setup the rendering
    *    process for text rendering, such as binding the glyph texture
    *    or shader programs. Thus, the rendering has to be setup before
    *    calling this function (see GlyphRenderer).
    */
    void draw() const;


protected:
    std::vector<Vertex>                       m_vertices; ///< Vertex list (CPU memory)
    std::unique_ptr<globjects::Buffer>        m_buffer;   ///< Vertex buffer (GPU memory)
    std::unique_ptr<globjects::VertexArray>   m_vao;      ///< Vertex array object
    globjects::Texture                      * m_texture;  ///< Glyph texture
};


} // namespace openll
