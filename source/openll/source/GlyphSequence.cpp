
#include <openll/GlyphSequence.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <openll/FontFace.h>


namespace openll
{


char32_t GlyphSequence::lineFeed()
{
    static const auto LF = static_cast<char32_t>('\x0A');
    return LF;
}

GlyphSequence::GlyphSequence()
: m_wordWrap(false)
, m_lineWidth(0.0f)
, m_alignment(Alignment::LeftAligned)
, m_anchor(LineAnchor::Baseline)
, m_fontColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
{
}

GlyphSequence::~GlyphSequence()
{
}

const std::u32string & GlyphSequence::string() const
{
    return m_string;
}

void GlyphSequence::setString(const std::u32string & string)
{
    // [TODO] Text may be large and comparison is slow ...
    if (m_string.compare(string) == 0) {
        return;
    }

    m_string = string;
}

size_t GlyphSequence::size() const
{
    return m_string.size();
}

size_t GlyphSequence::size(const FontFace & fontFace) const
{
    auto count = size_t(0);

    // Count visible characters in string
    for (const auto & c : m_string)
    {
        if (fontFace.depictable(c)) {
            ++count;
        }
    }

    return count;
}

const std::vector<char32_t> & GlyphSequence::chars(std::vector<char32_t> & allChars) const
{
    allChars.reserve(allChars.size() + size());

    for (const auto & c : m_string) {
        allChars.push_back(c);
    }

    return allChars;
}

const std::vector<char32_t> & GlyphSequence::chars(std::vector<char32_t> & depictableChars, const FontFace & fontFace) const
{
    // [TODO] Text may be large, so first counting and then iterating again should be avoided ...
    depictableChars.reserve(depictableChars.size() + size(fontFace));

    for (const auto & c : m_string) {
        if (fontFace.depictable(c)) {
            depictableChars.push_back(c);
        }
    }

    return depictableChars;
}

bool GlyphSequence::wordWrap() const
{
    return m_wordWrap;
}

void GlyphSequence::setWordWrap(bool wrap)
{
    m_wordWrap = wrap;
}

float GlyphSequence::lineWidth() const
{
    return m_lineWidth;
}

void GlyphSequence::setLineWidth(float lineWidth, float fontSize, const FontFace & fontFace)
{
    m_lineWidth = glm::max(lineWidth * fontFace.size() / fontSize, 0.0f);
}

Alignment GlyphSequence::alignment() const
{
    return m_alignment;
}

void GlyphSequence::setAlignment(Alignment alignment)
{
    m_alignment = alignment;
}

LineAnchor GlyphSequence::lineAnchor() const
{
    return m_anchor;
}

void GlyphSequence::setLineAnchor(LineAnchor anchor)
{
    m_anchor = anchor;
}

const glm::vec4 & GlyphSequence::fontColor() const
{
    return m_fontColor;
}

void GlyphSequence::setFontColor(const glm::vec4 & color)
{
    m_fontColor = color;
}

const glm::mat4 & GlyphSequence::transform() const
{
    return m_transform;
}

void GlyphSequence::setTransform(const glm::mat4 & transform)
{
    m_transform = transform;
}

void GlyphSequence::setTransform(const glm::vec2 & origin, float fontSize, const FontFace & fontFace, const glm::uvec2 & viewportExtent)
{
    // Start with identity matrix
    m_transform = glm::mat4();

    // Translate to lower left in NDC
    m_transform = glm::translate(m_transform, glm::vec3(-1.0f, -1.0f, 0.0f));

    // Translate to origin in screen space
    m_transform = glm::translate(m_transform, glm::vec3(origin, 0.0f));

    // Scale glyphs of font face to target normalized size
    m_transform = glm::scale(m_transform, glm::vec3(viewportExtent.y * fontSize / fontFace.size()));

    // Scale glyphs to NDC size
    m_transform = glm::scale(m_transform, 2.0f / glm::vec3(viewportExtent.x, viewportExtent.y, 1.0f));
}

void GlyphSequence::setTransform(const glm::vec3 & origin, float fontSizeInWorld, const FontFace & fontFace, const glm::mat4 & rotation)
{
    // Start with identity matrix
    m_transform = glm::mat4();

    // Translate to origin position
    m_transform = glm::translate(m_transform, origin);

    // Scale by font size
    m_transform = glm::scale(m_transform, glm::vec3(fontSizeInWorld / fontFace.size()));

    // Apply rotation
    m_transform = m_transform * rotation;
}

void GlyphSequence::setTransform(const glm::vec2 & origin, float fontSize, const FontFace & fontFace, const glm::uvec2 & viewportExtent, float pixelPerInch, const glm::vec4 & margins)
{
    // Calculate scale factor
    const auto pointsPerInch = 72.0f;
    const auto ppiScale = pixelPerInch / pointsPerInch;

    // Start with identity matrix
    m_transform = glm::mat4();

    // Translate to lower left in NDC
    m_transform = glm::translate(m_transform, glm::vec3(-1.0f, -1.0f, 0.0f));

    // Scale glyphs to NDC size
    m_transform = glm::scale(m_transform, 2.0f / glm::vec3(viewportExtent.x, viewportExtent.y, 1.0f));

    // Scale glyphs to pixel size with respect to the displays ppi
    m_transform = glm::scale(m_transform, glm::vec3(ppiScale));

    // Translate to origin in point space - scale origin within
    // margined extent (i.e., viewport with margined areas removed)
    const auto marginedExtent = glm::vec2(viewportExtent.x, viewportExtent.y) / ppiScale
        - glm::vec2(margins[3] + margins[1], margins[2] + margins[0]);
    m_transform = glm::translate(m_transform
        , glm::vec3((0.5f * origin + 0.5f) * marginedExtent, 0.0f) + glm::vec3(margins[3], margins[2], 0.0f));

    // Scale glyphs of font face to target font size
    m_transform = glm::scale(m_transform, glm::vec3(fontSize / fontFace.size()));
}


} // namespace openll
