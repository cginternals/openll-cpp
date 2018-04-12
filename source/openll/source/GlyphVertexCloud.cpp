
#include <openll/GlyphVertexCloud.h>

#include <numeric>
#include <algorithm>

#include <cppassist/memory/make_unique.h>
#include <cppassist/memory/offsetof.h>

#include <glbinding/gl/enum.h>
#include <glbinding/gl/boolean.h>

#include <globjects/Texture.h>
#include <globjects/Buffer.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>


namespace openll
{


GlyphVertexCloud::GlyphVertexCloud()
: m_buffer(cppassist::make_unique<globjects::Buffer>())
, m_vao(cppassist::make_unique<globjects::VertexArray>())
, m_texture(nullptr)
{
    // Setup vertex array object
    m_vao->binding(0)->setAttribute(0);
    m_vao->binding(0)->setBuffer(m_buffer.get(), 0, sizeof(Vertex));
    m_vao->binding(0)->setFormat(3, gl::GL_FLOAT, gl::GL_FALSE, cppassist::offset(&Vertex::origin));
    m_vao->enable(0);

    m_vao->binding(1)->setAttribute(1);
    m_vao->binding(1)->setBuffer(m_buffer.get(), 0, sizeof(Vertex));
    m_vao->binding(1)->setFormat(3, gl::GL_FLOAT, gl::GL_FALSE, cppassist::offset(&Vertex::vtan));
    m_vao->enable(1);

    m_vao->binding(2)->setAttribute(2);
    m_vao->binding(2)->setBuffer(m_buffer.get(), 0, sizeof(Vertex));
    m_vao->binding(2)->setFormat(3, gl::GL_FLOAT, gl::GL_FALSE, cppassist::offset(&Vertex::vbitan));
    m_vao->enable(2);

    m_vao->binding(3)->setAttribute(3);
    m_vao->binding(3)->setBuffer(m_buffer.get(), 0, sizeof(Vertex));
    m_vao->binding(3)->setFormat(4, gl::GL_FLOAT, gl::GL_FALSE, cppassist::offset(&Vertex::uvRect));
    m_vao->enable(3);

    m_vao->binding(4)->setAttribute(4);
    m_vao->binding(4)->setBuffer(m_buffer.get(), 0, sizeof(Vertex));
    m_vao->binding(4)->setFormat(4, gl::GL_FLOAT, gl::GL_FALSE, cppassist::offset(&Vertex::textColor));
    m_vao->enable(4);
}

GlyphVertexCloud::~GlyphVertexCloud()
{
}

const std::vector<GlyphVertexCloud::Vertex> & GlyphVertexCloud::vertices() const
{
    return m_vertices;
}

std::vector<GlyphVertexCloud::Vertex> & GlyphVertexCloud::vertices()
{
    return m_vertices;
}

const globjects::VertexArray * GlyphVertexCloud::vao() const
{
    return m_vao.get();
}

globjects::VertexArray * GlyphVertexCloud::vao()
{
    return m_vao.get();
}

const globjects::Texture * GlyphVertexCloud::texture() const
{
    return m_texture;
}

void GlyphVertexCloud::setTexture(globjects::Texture * texture)
{
    m_texture = texture;
}

void GlyphVertexCloud::update()
{
    m_buffer->setData(m_vertices, gl::GL_STATIC_DRAW);
}

void GlyphVertexCloud::update(const std::vector<Vertex> & vertices)
{
    m_buffer->setData(vertices, gl::GL_STATIC_DRAW);
}

void GlyphVertexCloud::draw() const
{
    m_vao->drawArrays(gl::GL_POINTS, 0, m_vertices.size());
}


} // namespace openll
