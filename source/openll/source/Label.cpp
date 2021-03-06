
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
: m_fontFace(nullptr)
, m_fontSize(16)
, m_wordWrap(false)
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

void Label::setText(std::u32string && text)
{
    m_text = std::shared_ptr<Text>(new Text);
    m_text->setText(std::move(text));
}

const FontFace * Label::fontFace() const
{
    return m_fontFace;
}

void Label::setFontFace(FontFace & fontFace)
{
    m_fontFace = &fontFace;
}

float Label::fontSize() const
{
    return m_fontSize;
}

void Label::setFontSize(float fontSize)
{
    m_fontSize = fontSize;
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

void Label::setLineWidth(float lineWidth)
{
    m_lineWidth = lineWidth;
}

const glm::vec4 & Label::margins() const
{
    return m_margins;
}

void Label::setMargins(const glm::vec4 & margins)
{
    m_margins = margins;
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

float Label::lineAnchorOffset() const
{
    switch (m_anchor)
    {
    case LineAnchor::Ascent:
        return -m_fontFace->ascent();
        break;

    case LineAnchor::Center:
        return -m_fontFace->size() * 0.5f + m_fontFace->descent();
        break;

    case LineAnchor::Descent:
        return -m_fontFace->descent();
        break;

    case LineAnchor::Baseline:
    default:
        return 0.0f;
    }
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

void Label::setTransform2D(const glm::vec2 & origin, const glm::uvec2 & viewportExtent, float pixelPerInch)
{
    assert(m_fontFace != nullptr);

    // Abort operation if no font face is set
    if (!m_fontFace) return;

    // Calculate scale factor
    const auto pointsPerInch = 72.0f;
    const auto ppiScale = pixelPerInch / pointsPerInch;

    // Start with identity matrix
    m_transform = glm::mat4(1.0f);

    // Translate to lower left in NDC
    m_transform = glm::translate(m_transform, glm::vec3(-1.0f, -1.0f, 0.0f));

    // Scale glyphs to NDC size
    m_transform = glm::scale(m_transform, 2.0f / glm::vec3(viewportExtent.x, viewportExtent.y, 1.0f));

    // Scale glyphs to pixel size with respect to the displays ppi
    m_transform = glm::scale(m_transform, glm::vec3(ppiScale, ppiScale, 1.0f));

    // Translate to origin in point space - scale origin within
    // margined extent (i.e., viewport with margined areas removed)
    const auto marginedExtent = glm::vec2(viewportExtent.x, viewportExtent.y) / ppiScale
        - glm::vec2(m_margins[3] + m_margins[1], m_margins[2] + m_margins[0]);
    m_transform = glm::translate(m_transform
        , glm::vec3((0.5f * origin + 0.5f) * marginedExtent, 0.0f) + glm::vec3(m_margins[3], m_margins[2], 0.0f));

    // Scale glyphs of font face to target font size
    m_transform = glm::scale(m_transform, glm::vec3(glm::vec2(m_fontSize / m_fontFace->size()), 1.0f));
}

void Label::setTransform3D(const glm::vec3 & origin, const glm::mat4 & transform)
{
    // Start with identity matrix
    m_transform = glm::mat4(1.0f);

    // Translate to origin position
    m_transform = glm::translate(m_transform, origin);

    // Scale by font size
    m_transform = glm::scale(m_transform, glm::vec3(m_fontSize));

    // Apply transform
    m_transform = m_transform * transform;
}


} // namespace openll
