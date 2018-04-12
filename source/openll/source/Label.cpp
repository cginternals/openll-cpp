
#include <openll/Label.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <openll/Text.h>
#include <openll/FontFace.h>


namespace openll
{


Label::Label()
: m_wordWrap(false)
, m_lineWidth(0.0f)
, m_alignment(Alignment::LeftAligned)
, m_anchor(LineAnchor::Baseline)
, m_textColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
{
}

Label::~Label()
{
}

const std::shared_ptr<Text> & Label::text() const
{
    return m_text;
}

void Label::setText(const std::shared_ptr<Text> & text)
{
    m_text = text;
}

void Label::setText(const std::u32string & text)
{
    m_text = std::shared_ptr<Text>(new Text);
    m_text->setText(text);
}

size_t Label::numChars() const
{
    return m_text->text().size();
}

bool Label::wordWrap() const
{
    return m_wordWrap;
}

void Label::setWordWrap(bool wrap)
{
    m_wordWrap = wrap;
}

float Label::lineWidth() const
{
    return m_lineWidth;
}

void Label::setLineWidth(float lineWidth, float fontSize, const FontFace & fontFace)
{
    m_lineWidth = glm::max(lineWidth * fontFace.size() / fontSize, 0.0f);
}

Alignment Label::alignment() const
{
    return m_alignment;
}

void Label::setAlignment(Alignment alignment)
{
    m_alignment = alignment;
}

LineAnchor Label::lineAnchor() const
{
    return m_anchor;
}

void Label::setLineAnchor(LineAnchor anchor)
{
    m_anchor = anchor;
}

const glm::vec4 & Label::textColor() const
{
    return m_textColor;
}

void Label::setTextColor(const glm::vec4 & color)
{
    m_textColor = color;
}

const glm::mat4 & Label::transform() const
{
    return m_transform;
}

void Label::setTransform(const glm::mat4 & transform)
{
    m_transform = transform;
}

void Label::setTransform2D(const glm::vec2 & origin, float fontSize, const FontFace & fontFace, const glm::uvec2 & viewportExtent)
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

void Label::setTransform2D(const glm::vec2 & origin, float fontSize, const FontFace & fontFace, const glm::uvec2 & viewportExtent, float pixelPerInch, const glm::vec4 & margins)
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

void Label::setTransform3D(const glm::vec3 & origin, float fontSizeInWorld, const FontFace & fontFace, const glm::mat4 & transform)
{
    // Start with identity matrix
    m_transform = glm::mat4();

    // Translate to origin position
    m_transform = glm::translate(m_transform, origin);

    // Scale by font size
    m_transform = glm::scale(m_transform, glm::vec3(fontSizeInWorld));

    // Apply transform
    m_transform = m_transform * transform;
}


} // namespace openll
