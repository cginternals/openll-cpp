
#include <openll/Glyph.h>


namespace openll
{


Glyph::Glyph()
: m_index(0u)
, m_advance(0.0f)
{
}

Glyph::~Glyph()
{
}

size_t Glyph::index() const
{
    return m_index;
}

void Glyph::setIndex(const size_t index)
{
    m_index = index;
}

const glm::vec2 & Glyph::subTextureOrigin() const
{
    return m_subtextureOrigin;
}

void Glyph::setSubTextureOrigin(const glm::vec2 & origin)
{
    //assert(origin.x >= 0.0f);
    //assert(origin.x <= 1.0f);
    //assert(origin.y >= 0.0f);
    //assert(origin.y <= 1.0f);

    m_subtextureOrigin = origin;
}

const glm::vec2 & Glyph::subTextureExtent() const
{
    return m_subtextureExtent;
}

void Glyph::setSubTextureExtent(const glm::vec2 & extent)
{
    assert(extent.x <= 1.0f);
    assert(extent.y <= 1.0f);

    m_subtextureExtent = extent;
}

bool Glyph::depictable() const
{
    return m_subtextureExtent.x > 0.0f && m_subtextureExtent.y > 0.0f;
}

const glm::vec2 & Glyph::bearing() const
{
    return m_bearing;
}

void Glyph::setBearing(const glm::vec2 & bearing)
{
    m_bearing = bearing;
}

void Glyph::setBearing(const float fontAscent, const float xOffset, const float yOffset)
{
    m_bearing.x = xOffset;
    m_bearing.y = fontAscent - yOffset;
}

const glm::vec2 & Glyph::extent() const
{
    return m_extent;
}

void Glyph::setExtent(const glm::vec2 & extent)
{
    m_extent = extent;
}

float Glyph::advance() const
{
    return m_advance;
}

void Glyph::setAdvance(const float advance)
{
    m_advance = advance;
}

float Glyph::kerning(size_t subsequentIndex) const
{
    // Get kerning between this glyph and the subsequent glyph
    const auto it = m_kernings.find(subsequentIndex);
    if (it != m_kernings.cend())
    {
        return it->second;
    }
    else
    {
        return 0.0f;
    }
}

void Glyph::setKerning(size_t subsequentIndex, const float kerning)
{
    // Set kerning between this glyph and the subsequent glyph
    m_kernings[subsequentIndex] = kerning;
}


} // namespace openll
