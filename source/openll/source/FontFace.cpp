
#include <openll/FontFace.h>


namespace openll
{


FontFace::FontFace()
: m_ascent (0.0f)
, m_descent(0.0f)
, m_linegap(0.0f)
{
}

FontFace::~FontFace()
{
}

float FontFace::size() const
{
    // Note: m_descent is usually negative.
    return m_ascent - m_descent;
}

float FontFace::ascent() const
{
    return m_ascent;
}

void FontFace::setAscent(const float ascent)
{
    assert(ascent > 0.0f);

    m_ascent = ascent;
}

float FontFace::descent() const
{
    return m_descent;
}

void FontFace::setDescent(const float descent)
{
    // Note: No assert here:
    // There might be fonts with their lowest descender above baseline.
    // assert(descent < 0.0f);

    m_descent = descent;
}

float FontFace::linegap() const
{
    return m_linegap;
}

void FontFace::setLinegap(const float linegap)
{
    m_linegap = linegap;
}

float FontFace::linespace() const
{
    if (lineHeight() == 0.0f)
    {
        return 0.0f;
    }

    return size() / lineHeight();
}

void FontFace::setLinespace(const float spacing)
{
    m_linegap = size() * (spacing - 1);
}

float FontFace::lineHeight() const
{
    return size() + linegap();
}

void FontFace::setLineHeight(const float lineHeight)
{
    m_linegap = lineHeight - size();
}

const glm::uvec2 & FontFace::glyphTextureExtent() const
{
    return m_glyphTextureExtent;
}

void FontFace::setGlyphTextureExtent(const glm::uvec2 & extent)
{
    assert(extent.x > 0);
    assert(extent.y > 0);

    m_glyphTextureExtent = extent;
    m_inverseGlyphTextureExtent = 1.0f / glm::vec2(m_glyphTextureExtent);
}

const glm::vec2 & FontFace::inverseGlyphTextureExtent() const
{
    return m_inverseGlyphTextureExtent;
}

const glm::vec4 & FontFace::glyphTexturePadding() const
{
    return m_glyphTexturePadding;
}

void FontFace::setGlyphTexturePadding(const glm::vec4 & padding)
{
    assert(padding[0] >= 0.0f);
    assert(padding[1] >= 0.0f);
    assert(padding[2] >= 0.0f);
    assert(padding[3] >= 0.0f);

    m_glyphTexturePadding = padding;
}

globjects::Texture * FontFace::glyphTexture() const
{
    return m_glyphTexture.get();
}

void FontFace::setGlyphTexture(std::unique_ptr<globjects::Texture> && texture)
{
    m_glyphTexture = std::move(texture);
}

bool FontFace::hasGlyph(const size_t index) const
{
    return m_glyphs.find(index) != m_glyphs.cend();
}

Glyph & FontFace::glyph(const size_t index)
{
    const auto & existing = m_glyphs.find(index);

    if (existing != m_glyphs.cend())
    {
        return existing->second;
    }

    auto glyph = Glyph();
    glyph.setIndex(index);

    const auto inserted = m_glyphs.emplace(glyph.index(), std::move(glyph));

    return inserted.first->second;
}

const Glyph & FontFace::glyph(const size_t index) const
{
    static const auto empty = Glyph();

    const auto & existing = m_glyphs.find(index);
    if (existing != m_glyphs.cend())
    {
        return existing->second;
    }

    return empty;
}

void FontFace::addGlyph(const Glyph & glyph)
{
    assert(m_glyphs.find(glyph.index()) == m_glyphs.cend());

    m_glyphs.emplace(glyph.index(), glyph);
}

void FontFace::addGlyph(Glyph && glyph)
{
    assert(m_glyphs.find(glyph.index()) == m_glyphs.cend());

    m_glyphs.emplace(glyph.index(), std::move(glyph));
}

std::vector<size_t> FontFace::glyphs() const
{
    auto glyphs = std::vector<size_t>();
    glyphs.reserve(m_glyphs.size());

    for (const auto & i : m_glyphs)
    {
        glyphs.push_back(i.first);
    }

    return glyphs;
}

bool FontFace::depictable(const size_t index) const
{
    return glyph(index).depictable();
}

float FontFace::kerning(const size_t index, const size_t subsequentIndex) const
{
    if (index == std::get<0>(m_kerningRequestCache) && subsequentIndex == std::get<1>(m_kerningRequestCache))
    {
        return std::get<2>(m_kerningRequestCache);
    }

    const auto it = m_glyphs.find(index);

    if (it == m_glyphs.cend())
    {
        return 0.0f;
    }

    const auto kerning = it->second.kerning(subsequentIndex);

    std::get<0>(m_kerningRequestCache) = index;
    std::get<1>(m_kerningRequestCache) = subsequentIndex;
    std::get<2>(m_kerningRequestCache) = kerning;

    return kerning;
}

void FontFace::setKerning(const size_t index, const size_t subsequentIndex, const float kerning)
{
    assert(hasGlyph(index));
    assert(hasGlyph(subsequentIndex));

    const auto it = m_glyphs.find(index);
    if (it == m_glyphs.cend())
    {
        assert(false);
        return;
    }

    it->second.setKerning(subsequentIndex, kerning);
}


} // namespace openll
