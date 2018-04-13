
#pragma once


#include <memory>
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


class Text;
class FontFace;


/**
*  @brief
*    Text label that is to be rendered with a given font face
*/
class OPENLL_API Label
{
public:
    /**
    *  @brief
    *    Constructor
    */
    Label();

    /**
    *  @brief
    *    Destructor
    */
    ~Label();

    /**
    *  @brief
    *    Get text
    *
    *  @return
    *    Shared pointer to Text
    */
    const std::shared_ptr<Text> & text() const;

    /**
    *  @brief
    *    Set text
    *
    *  @param[in] text
    *    Shared pointer to Text
    */
    void setText(const std::shared_ptr<Text> & text);

    /**
    *  @brief
    *    Set text directly from string (32 bit unicode string)
    *
    *  @param[in] text
    *    Text (32 bit unicode string)
    */
    void setText(const std::u32string & text);

    /**
    *  @brief
    *    Get font face
    *
    *  @return
    *    The used font face (can be nullptr!)
    */
    const FontFace * fontFace() const;

    /**
    *  @brief
    *    Set font face
    *
    *  @param[in] fontFace
    *    The used font face
    */
    void setFontFace(FontFace & fontFace);

    /**
    *  @brief
    *    Get font size
    *
    *  @return
    *    Font size for rendering (in pt)
    */
    float fontSize() const;

    /**
    *  @brief
    *    Set font size
    *
    *  @param[in] fontSize
    *    Font size for rendering (in pt)
    */
    void setFontSize(float fontSize);

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
    */
    float lineWidth() const;

    /**
    *  @brief
    *    Set line width (in pt)
    *
    *  @param[in] lineWidth
    *    Width of a single line (in pt)
    */
    void setLineWidth(float lineWidth);

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
    *    Get text color (rgba)
    *
    *  @return
    *    Text color (rgba)
    */
    const glm::vec4 & textColor() const;

    /**
    *  @brief
    *    Set text color (rgba)
    *
    *  @param[in] color
    *    Text color (rgba)
    */
    void setTextColor(const glm::vec4 & color);

    /**
    *  @brief
    *    Get transformation matrix for rendering the label
    *
    *  @return
    *    Transformation for the label
    */
    const glm::mat4 & transform() const;

    /**
    *  @brief
    *    Set transformation matrix for rendering the label
    *
    *  @param[in] transform
    *    Transformation for the label
    *
    *  @remarks
    *    Instead of setting the transformation matrix directly,
    *    use the other signatures of setTransform to calculate the
    *    transformation matrix for rendering text in 2D or 3D spaces.
    */
    void setTransform(const glm::mat4 & transform);

    /**
    *  @brief
    *    Set transformation matrix for rendering text in 2D space
    *
    *  @param[in] origin
    *    Point of origin (normalized device coordinates)
    *  @param[in] viewportExtent
    *    Extend of the viewport (width, height) in px
    *  @param[in] pixelPerInch
    *    Number of pixels per inch
    *  @param[in] margins
    *    Margins (top/right/bottom/left, in pt)
    */
    void setTransform2D(const glm::vec2 & origin, const glm::uvec2 & viewportExtent, float pixelPerInch = 72.0f, const glm::vec4 & margins = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

    /**
    *  @brief
    *    Set transformation matrix for rendering text in 3D world space
    *
    *  @param[in] origin
    *    Point of origin (in world coordinates)
    *  @param[in] transform
    *    Local coordinate system on which the transformation is applied
    */
    void setTransform3D(const glm::vec3 & origin, const glm::mat4 & transform);


protected:
    std::shared_ptr<Text> m_text;      ///< Text that is rendered
    FontFace            * m_fontFace;  ///< The used font face
    float                 m_fontSize;  ///< Font size for rendering (in pt)
    bool                  m_wordWrap;  ///< Wrap words at the end of a line?
    float                 m_lineWidth; ///< Width of a line (in pt)
    Alignment             m_alignment; ///< Horizontal text alignment
    LineAnchor            m_anchor;    ///< Vertical line anchor
    glm::mat4             m_transform; ///< Transformation for the label
    glm::vec4             m_textColor; ///< Text color (rgba)
};


} // namespace openll
