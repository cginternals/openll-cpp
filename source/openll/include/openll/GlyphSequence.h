
#pragma once


#include <string>
#include <vector>

#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <openll/Alignment.h>
#include <openll/LineAnchor.h>
#include <openll/openll_api.h>


namespace openll
{


class FontFace;


/**
*  @brief
*    A sequence of text that is to be rendered with a given font face
*/
class OPENLL_API GlyphSequence
{
public:
    /**
    *  @brief
    *    Get line feed character
    *
    *  @return
    *    Character that marks the end of a line
    */
    char32_t lineFeed();


public:
    /**
    *  @brief
    *    Constructor
    */
    GlyphSequence();

    /**
    *  @brief
    *    Destructor
    */
    ~GlyphSequence();

    /**
    *  @brief
    *    Get text that is rendered (32 bit unicode string)
    *
    *  @return
    *    Text (32 bit unicode string)
    */
    const std::u32string & string() const;

    /**
    *  @brief
    *    Set text that is rendered (32 bit unicode string)
    *
    *  @param[in] string
    *    Text (32 bit unicode string)
    */
    void setString(const std::u32string & string);

    /**
    *  @brief
    *    Get number of characters
    *
    *  @return
    *    Number of characters in the glyph sequence
    */
    size_t size() const;

    /**
    *  @brief
    *    Get number of characters that will be rendered using a given font face
    *
    *    This function returns the number of glyphs that will actually be rendered
    *    using the given font face, ignoring all glyphs that are not visible/renderable.
    *    The runtime complexity is O(n) w.r.t the number of characters in the string.
    *
    *  @param[in] fontFace
    *    Font face that used for rendering
    *
    *  @return
    *    Number of visible characters in the glyph sequence
    */
    size_t size(const FontFace & fontFace) const;

    /**
    *  @brief
    *    Get all characters of the glyph sequence
    *
    *  @return
    *    List of characters in the glyph sequence (32 bit unicode)
    */
    const std::vector<char32_t> & chars(std::vector<char32_t> & allChars) const;

    /**
    *  @brief
    *    Get all visible characters of the glyph sequence using a given font face
    *
    *    This function returns the characters that will actually be rendered
    *    using the given font face, ignoring all glyphs that are not visible/renderable.
    *
    *  @return
    *    List of visible characters in the glyph sequence (32 bit unicode)
    */
    const std::vector<char32_t> & chars(std::vector<char32_t> & allChars, const FontFace & fontFace) const;

    /**
    *  @brief
    *    Get if words are wrapped at the end of a line
    *
    *  @return
    *    'true' if word wrap is enabled, else 'false'
    */
    bool wordWrap() const;

    /**
    *  @brief
    *    Set if words are wrapped at the end of a line
    *
    *  @param[in] wrap
    *    'true' if word wrap is enabled, else 'false'
    */
    void setWordWrap(bool wrap);

    /**
    *  @brief
    *    Get line width (in pt)
    *
    *  @return
    *    Width of a single line (in pt)
    *
    *  @remarks
    *    Since typesetting is done in the font-face's font size,
    *    the line width is scaled w.r.t. the font size of the given font face.
    */
    float lineWidth() const;

    /**
    *  @brief
    *    Set line width (in pt)
    *
    *  @param[in] lineWidth
    *    Width of a single line (in pt)
    *  @param[in] fontSize
    *    Font size for rendering (in pt)
    *  @param[in] fontFace
    *    Font face to use
    *
    *  @remarks
    *    Since typesetting is done in the font-face's font size,
    *    the line width is scaled w.r.t. the font size of the given font face.
    */
    void setLineWidth(float lineWidth, float fontSize, const FontFace & fontFace);

    /**
    *  @brief
    *    Get horizontal text alignment
    *
    *  @return
    *    Horizontal text alignment
    */
    Alignment alignment() const;

    /**
    *  @brief
    *    Set horizontal text alignment
    *
    *  @param[in] alignment
    *    Horizontal text alignment
    */
    void setAlignment(Alignment alignment);

    /**
    *  @brief
    *    Get vertical text anchor point
    *
    *  @return
    *    Vertical anchor point
    */
    LineAnchor lineAnchor() const;

    /**
    *  @brief
    *    Set vertical text anchor point
    *
    *  @param[in] anchor
    *    Vertical anchor point
    */
    void setLineAnchor(LineAnchor anchor);

    /**
    *  @brief
    *    Get font color (rgba)
    *
    *  @return
    *    Font color (rgba)
    */
    const glm::vec4 & fontColor() const;

    /**
    *  @brief
    *    Set font color (rgba)
    *
    *  @param[in] color
    *    Font color (rgba)
    */
    void setFontColor(const glm::vec4 & color);

    /**
    *  @brief
    *    Get transformation matrix for rendering the glyph sequence
    *
    *  @return
    *    Transformation for the glyph sequence
    */
    const glm::mat4 & transform() const;

    /**
    *  @brief
    *    Set transformation matrix for rendering the glyph sequence
    *
    *  @param[in] transform
    *    Transformation for the glyph sequence
    *
    *  @remarks
    *    Instead of setting the transformation matrix directly,
    *    use the other signatures of setTransform to calculate the
    *    transformation matrix for rendering text in 2D or 3D spaces.
    */
    void setTransform(const glm::mat4 & transform);

    /**
    *  @brief
    *    Set transformation matrix for rendering text in a 2D space
    *
    *  @param[in] origin
    *    Point of origin (normalized coordinates w.r.t. the given viewport)
    *  @param[in] fontSize
    *    Font size (in pt)
    *  @param[in] fontFace
    *    The used font face
    *  @param[in] viewportExtent
    *    Extend of the viewport (width, height) in px
    */
    void setTransform(const glm::vec2 & origin, float fontSize, const FontFace & fontFace, const glm::uvec2 & viewportExtent);

    /**
    *  @brief
    *    Set transformation matrix for rendering text in 3D world space
    *
    *  @param[in] origin
    *    Point of origin (in world coordinates)
    *  @param[in] fontSize
    *    Font size (in pt)
    *  @param[in] fontFace
    *    The used font face
    *  @param[in] rotation
    *    Rotation matrix for the text
    */
    void setTransform(const glm::vec3 & origin, float fontSize, const FontFace & fontFace, const glm::mat4 & rotation);

    /**
    *  @brief
    *    Set transformation matrix for rendering text in 2D space (???) [TODO]
    *
    *  @param[in] origin
    *    Point of origin (normalized coordinates w.r.t. the given viewport)
    *  @param[in] fontSizeInWorld
    *    Font size in pt (???) [TODO]
    *  @param[in] fontFace
    *    The used font face
    *  @param[in] viewportExtent
    *    Extend of the viewport (width, height) in px
    *  @param[in] pixelPerInch
    *    Number of pixels per inch
    *  @param[in] margins
    *    Margins (top/right/bottom/left, in pt)
    */
    void setTransform(const glm::vec2 & origin, float fontSizeInWorld, const FontFace & fontFace, const glm::uvec2 & viewportExtent,
                      float pixelPerInch, const glm::vec4 & margins = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));


protected:
    std::u32string m_string;    ///< Text that is rendered
    bool           m_wordWrap;  ///< Wrap words at the end of a line?
    float          m_lineWidth; ///< Width of a line (in pt)
    Alignment      m_alignment; ///< Horizontal text alignment
    LineAnchor     m_anchor;    ///< Vertical line anchor
    glm::mat4      m_transform; ///< Transformation for the glyph sequence
    glm::vec4      m_fontColor; ///< Font color (rgba)
};


} // namespace openll
