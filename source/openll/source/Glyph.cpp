
#include <openll/Glyph.h>

#include <openll/FontFace.h>


namespace openll
{


Glyph::Glyph(const FontFace * fontFace)
: m_fontFace(fontFace)
, m_index(0u)
, m_advance(0.0f)
{
}

Glyph::~Glyph()
{
}

void Glyph::setFontFace(const FontFace * fontFace)
{
    m_fontFace = fontFace;

    if (m_fontFace != nullptr)
    {
        const auto & padding = m_fontFace->glyphTexturePadding();
        m_penOrigin = glm::vec2(m_bearing.x - padding[3], m_bearing.y - m_extent.y - padding[2]);
        m_penTangent = glm::vec2(m_extent.x + padding[1] + padding[3], 0.f);
        m_penBitangent = glm::vec2(0.f, m_extent.y + padding[0] + padding[2]);

        const auto & extentScale = m_fontFace->inverseGlyphTextureExtent();
        const auto ll = m_subtextureOrigin
            - glm::vec2(padding[3], padding[2]) * extentScale;
        const auto ur = m_subtextureOrigin + m_subtextureExtent
            + glm::vec2(padding[1], padding[0]) * extentScale;
        m_subtextureRect = glm::vec4(ll, ur);
    }
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

    if (m_fontFace != nullptr)
    {
        const auto & padding = m_fontFace->glyphTexturePadding();
        const auto & extentScale = m_fontFace->inverseGlyphTextureExtent();
        const auto ll = m_subtextureOrigin
            - glm::vec2(padding[3], padding[2]) * extentScale;
        const auto ur = m_subtextureOrigin + m_subtextureExtent
            + glm::vec2(padding[1], padding[0]) * extentScale;
        m_subtextureRect = glm::vec4(ll, ur);
    }
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
    m_depictable = m_subtextureExtent.x > 0.0f && m_subtextureExtent.y > 0.0f;

    if (m_fontFace != nullptr)
    {
        const auto & padding = m_fontFace->glyphTexturePadding();
        const auto & extentScale = m_fontFace->inverseGlyphTextureExtent();
        const auto ll = m_subtextureOrigin
            - glm::vec2(padding[3], padding[2]) * extentScale;
        const auto ur = m_subtextureOrigin + m_subtextureExtent
            + glm::vec2(padding[1], padding[0]) * extentScale;
        m_subtextureRect = glm::vec4(ll, ur);
    }
}

bool Glyph::depictable() const
{
    return m_depictable;
}

const glm::vec2 & Glyph::bearing() const
{
    return m_bearing;
}

void Glyph::setBearing(const glm::vec2 & bearing)
{
    m_bearing = bearing;

    if (m_fontFace != nullptr)
    {
        const auto & padding = m_fontFace->glyphTexturePadding();
        m_penOrigin = glm::vec2(m_bearing.x - padding[3], m_bearing.y - m_extent.y - padding[2]);
    }
}

void Glyph::setBearing(const float fontAscent, const float xOffset, const float yOffset)
{
    m_bearing.x = xOffset;
    m_bearing.y = fontAscent - yOffset;

    if (m_fontFace != nullptr)
    {
        const auto & padding = m_fontFace->glyphTexturePadding();
        m_penOrigin = glm::vec2(m_bearing.x - padding[3], m_bearing.y - m_extent.y - padding[2]);
    }
}

const glm::vec2 & Glyph::extent() const
{
    return m_extent;
}

void Glyph::setExtent(const glm::vec2 & extent)
{
    m_extent = extent;

    if (m_fontFace != nullptr)
    {
        const auto & padding = m_fontFace->glyphTexturePadding();
        m_penOrigin = glm::vec2(m_bearing.x - padding[3], m_bearing.y - m_extent.y - padding[2]);
        m_penTangent = glm::vec2(m_extent.x + padding[1] + padding[3], 0.f);
        m_penBitangent = glm::vec2(0.f, m_extent.y + padding[0] + padding[2]);
    }
}

float Glyph::advance() const
{
    return m_advance;
}

void Glyph::setAdvance(const float advance)
{
    m_advance = advance;
}

const glm::vec2 & Glyph::penOrigin() const
{
    return m_penOrigin;
}


const glm::vec2 & Glyph::penTangent() const
{
    return m_penTangent;
}

const glm::vec2 & Glyph::penBitangent() const
{
    return m_penBitangent;
}

const glm::vec4 & Glyph::subtextureRectangle() const
{
    return m_subtextureRect;
}


} // namespace openll
