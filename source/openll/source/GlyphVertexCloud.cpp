
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

#include <openll/GlyphSequence.h>
#include <openll/FontFace.h>


namespace
{


// Get permutation to sort a vector with a given comparison function
// http://stackoverflow.com/a/17074810 (thanks to Timothy Shields)
template <typename T, typename Compare>
std::vector<std::size_t> sort_permutation(const std::vector<T> & vec, const Compare & compare)
{
    std::vector<std::size_t> p(vec.size());

    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(), [&] (std::size_t i, std::size_t j)
    {
        return compare(vec[i], vec[j]);
    });

    return p;
}

// Apply permutation to vector
template <typename T>
std::vector<T> apply_permutation(const std::vector<T> & vec, const std::vector<std::size_t> & p)
{
    std::vector<T> sorted_vec(p.size());
    std::transform(p.begin(), p.end(), sorted_vec.begin(), [&] (std::size_t i)
    {
        return vec[i];
    });

    return sorted_vec;
}


} // namespace


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

void GlyphVertexCloud::optimize(const std::vector<GlyphSequence> & sequences, const FontFace & fontFace)
{
    // Perform L1/texture-cache optimization: sort vertex cloud by glyphs

    // Create string associated with all depictable glyphs
    auto depictableChars = std::vector<char32_t>();
    for (const auto & sequence : sequences) {
        sequence.chars(depictableChars, fontFace);
    }

    // Get vertices
    auto vertices = m_vertices;
    assert(vertices.size() == depictableChars.size());

    // Get permutation to sort characters
    const auto p = sort_permutation(depictableChars, [] (const char32_t & a, const char32_t & b)
    {
        return a < b;
    });

    // Apply perfumtation to vertices
    update(apply_permutation(vertices, p));
}

void GlyphVertexCloud::draw() const
{
    m_vao->drawArrays(gl::GL_POINTS, 0, m_vertices.size());
}


} // namespace openll
