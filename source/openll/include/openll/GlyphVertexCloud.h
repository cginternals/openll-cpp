
#pragma once


#include <memory>
#include <vector>
#include <unordered_map>

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


class OPENLL_API GlyphVertexCloud
{
public:
    struct Vertex
    {
        glm::vec3 origin;
        glm::vec3 vtan;
        glm::vec3 vbitan;
        // vec2 lowerLeft and vec2 upperRight in glyph texture (uv)
        glm::vec4 uvRect;
        glm::vec4 fontColor;
    };

    using Vertices = std::vector<Vertex>;


public:
    GlyphVertexCloud();
    virtual ~GlyphVertexCloud();

    GlyphVertexCloud & operator=(const GlyphVertexCloud &) = delete;

    const globjects::Texture * texture() const;
    void setTexture(globjects::Texture * texture);

    globjects::VertexArray * vao();
    const globjects::VertexArray * vao() const;

    Vertices & vertices();
    const Vertices & vertices() const;

    void update();
    // allows for volatile optimizations
    void update(const Vertices & vertices);

    void optimize(
        const std::vector<GlyphSequence> & sequences
    ,   const FontFace & fontFace);

    void draw() const;


protected:
    std::unique_ptr<globjects::Buffer>      m_buffer;
    std::unique_ptr<globjects::VertexArray> m_vao;
    std::unordered_map<size_t, globjects::Buffer *> m_buffers;

    Vertices             m_vertices;
    globjects::Texture * m_texture;
};


} // namespace openll
