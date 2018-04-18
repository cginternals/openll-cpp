
#pragma once


#include <cstdint>
#include <map>
#include <unordered_map>

#include <glm/vec2.hpp>

#include <openll/openll_api.h>


namespace openll
{

/**
*  @brief
*    Description of a glyph
*
*    Most of the glyph data (except the advance) refers to the font
*    face's glyph texture.
*
*  @remarks
*    This class does not provide dpi awareness. This has to be
*    handled outside of this class, e.g., during layouting and rendering.
*/
class OPENLL_API Glyph
{
public:
    /**
    *  @brief
    *    Constructor
    */
    Glyph();

    /**
    *  @brief
    *    Destructor
    */
    ~Glyph();

    /**
    *  @brief
    *    Get index in associated FontFace
    *
    *  @return
    *    Index of the glyph in FontFace
    */
    size_t index() const;

    /**
    *  @brief
    *    Set index in associated FontFace
    *
    *  @param[in] index
    *    Index of the glyph in the associated FontFace
    */
    void setIndex(size_t index);

    /**
    *  @brief
    *    Get upper left position of the glyph's sub-texture
    *
    *    The upper left position refers to the glyph texture that is
    *    specified by a font face (see FontFace). It contains the
    *    (u,v)-coordinate pointing to the glyphs sub-texture within
    *    the glyph texture. The coordinates are normalized in [0..1].
    *
    *  @return
    *    (u,v)-coordinate of the upper left texel of the glyph's sub-texture
    */
    const glm::vec2 & subTextureOrigin() const;

    /**
    *  @brief
    *    Set upper left position of the glyph's sub-texture
    *
    *    The upper left position refers to the glyph texture that is
    *    specified by a font face (see FontFace). It contains the
    *    (u,v)-coordinate pointing to the glyphs sub-texture within
    *    the glyph texture. The coordinates are normalized in [0..1].
    *
    *  @param[in] origin
    *    (u,v)-coordinate of the upper left texel of the glyph's sub-texture
    */
    void setSubTextureOrigin(const glm::vec2 & origin);

    /**
    *  @brief
    *    Get width and height of the glyph's sub-texture
    *
    *    In combination with the sub-texture offset (subTextureOffset),
    *    the sub-texture rectangle is implicitly specified in
    *    normalized texture coordinates [0..1].
    *
    *  @return
    *    Normalized width and height of the glyph's sub-texture
    *
    *  @remarks
    *    The extent comprises the font face's padding
    */
    const glm::vec2 & subTextureExtent() const;

    /**
    *  @brief
    *    Set width and height of the glyph's sub-texture
    *
    *    In combination with the sub-texture offset (subTextureOffset),
    *    the sub-texture rectangle is implicitly specified in
    *    normalized texture coordinates [0..1].
    *
    *  @param[in] extent
    *    Normalized width and height of the glyph's sub-texture
    *
    *  @remarks
    *    The extent comprises the font face's padding
    */
    void setSubTextureExtent(const glm::vec2 & extent);

    /**
    *  @brief
    *    Check if a glyph is depictable/renderable
    *
    *    If the glyph's subtexture vertical or horizontal extent is
    *    zero, the glyph does not need to be depicted/rendered.
    *    For example, spaces, line feeds, other control sequences as well
    *    as unknown glyphs do not need to be processed for rendering.
    *
    *  @return
    *    'true' if the glyph needs to be depicted/rendered, else 'false'
    */
    bool depictable() const;

    /**
    *  @brief
    *    Get the x and y offsets w.r.t. to the pen-position on the baseline
    *
    *    The horizontal bearing does not comprise the glyph-texture's
    *    padding provided by the owning font face (see FontFace).
    *    The vertical bearing also does not comprise the glyph-
    *    texture's padding and is the measured w.r.t. baseline.
    *
    *  @return
    *    The horizontal and vertical bearing based on the glyphs origin or
    *    the pen-position placed on the baseline (in pt)
    */
    const glm::vec2 & bearing() const;

    /**
    *  @brief
    *    Set the x and y offsets w.r.t. to the pen-position on the baseline
    *
    *    The horizontal bearing does not comprise the glyph-texture's
    *    padding provided by the owning font face (see FontFace).
    *    The vertical bearing also does not comprise the glyph-
    *    texture's padding and is the measured w.r.t. baseline.
    *
    *  @param[in] bearing
    *    The horizontal and vertical bearing based on the glyphs origin or
    *    the pen-position placed on the baseline (in pt)
    */
    void setBearing(const glm::vec2 & bearing);

    /**
    *  @brief
    *    Set the x and y bearings
    *
    *    The horizontal bearing does not comprise the glyph-texture's
    *    padding provided by the owning font face (see FontFace).
    *    The vertical bearing also does not comprise the glyph-
    *    texture's padding and is the measured w.r.t. baseline.
    *
    *    The vertical bearing is computed as follows:
    *        bearingY = fontAscent - (yOffset - top_padding)
    *    The horizontal bearing equals the xOffset:
    *        bearingX = xOffset - left_padding:
    *
    *  @param[in] fontAscent
    *    The font face's ascent (in pt)
    *  @param[in] xOffset
    *    The glyphs horizontal offset without left padding (in pt)
    *  @param[in] yOffset
    *    The glyphs vertical offset w.r.t. the font's topmost
    *    descendends, without the font's top padding (in pt)
    */
    void setBearing(float fontAscent, float xOffset, float yOffset);

    /**
    *  @brief
    *    Get width and height of the glyph (in pt)
    *
    *  @return
    *    The glyph's extent (width and height) in pt
    */
    const glm::vec2 & extent() const;

    /**
    *  @brief
    *    Set width and height of the glyph (in pt)
    *
    *  @param[in] extent
    *    The glyph's extent (width and height) in pt
    */
    void setExtent(const glm::vec2 & extent);

    /**
    *  @brief
    *    Get the glyph's horizontal overall advance (in pt)
    *
    *    The horizontal advance comprises the font face's left and
    *    right padding, the glyphs (inner) width as well as the
    *    horizontal bearing (and often a glyph specific gap).
    *
    *    For example:
    *        advance = subtextureExtent_width + xOffset (+ gap)
    *    or alternatively:
    *        advance = xOffset + padding_left + glyph_width + padding_right (+ gap)
    *
    *  @return
    *    The glyphs horizontal advance along the baseline (in pt)
    */
    float advance() const;

    /**
    *  @brief
    *    Set the glyph's horizontal overall advance in pt.
    *
    *    The horizontal advance comprises the font face's left and
    *    right padding, the glyphs (inner) width as well as the
    *    horizontal bearing (and often a glyph specific gap).
    *
    *    For example:
    *        advance = subtextureExtent_width + xOffset (+ gap)
    *    or alternatively:
    *        advance = xOffset + padding_left + glyph_width + padding_right (+ gap)
    *
    *  @param[in] advance
    *    The glyphs horizontal advance along the baseline (in pt)
    */
    void setAdvance(float advance);


protected:
    size_t    m_index;            ///< Index of the glyph in the associated FontFace
    glm::vec2 m_subtextureOrigin; ///< Upper left position of the glyph's sub-texture
    glm::vec2 m_subtextureExtent; ///< Width and height of the glyph's sub-texture
    glm::vec2 m_bearing;          ///< x and y offsets w.r.t. to the pen-position on the baseline
    glm::vec2 m_extent;           ///< Width and height of the glyph in pt
    float     m_advance;          ///< Glyph's horizontal overall advance in pt
    bool      m_depictable;       ///< Flag if the glyph is depictable/renderable
};


} // namespace openll
