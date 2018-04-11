
#pragma once


#include <string>

#include <glm/fwd.hpp>

#include <openll/GlyphVertexCloud.h>


namespace openll
{


enum class Alignment : unsigned char;
class GlyphSequence;
class FontFace;
class Glyph;


/**
*  @brief
*    Typesetter that layouts text
*
*    The typesetter is responsible for layouting text on the screen or
*    in a virtual space. It takes a glyph sequence, which defines where
*    it wants to appear (see GlyphSequence), and a font face that is used
*    to display the text, and computes the actual position for each glyph.
*    Its output is a vertex array, which describes the glyphs position and
*    appearance on the screen/in the scene and which can be rendered
*    using a GlyphRenderer.
*/
class OPENLL_API Typesetter
{
public:
    Typesetter() = delete;
    ~Typesetter() = delete;

    /**
    *  @brief
    *    Get line feed character
    *
    *  @return
    *    Character that marks the end of a line
    */
    static const char32_t & lineFeed();

    /**
    *  @brief
    *    Get the extent of the text when layouted with a given font size
    *
    *  @param[in] sequence
    *    Text to display
    *  @param[in] fontFace
    *    Font face to use
    *  @param[in] fontSize
    *    Font size (in pt)
    *
    *  @return
    *    Extent (width, height) of the text
    *
    *  @remarks
    *    This function internally performs the typesetting of the
    *    text, but only returns its extent, disregarding the resulting
    *    vertex array. Therefore, if possible, typeset should be called
    *    instead of first calling extent and then typeset, to avoid
    *    layouting the text more than once.
    */
    static glm::vec2 extent(const GlyphSequence & sequence, const FontFace & fontFace, float fontSize);

    /**
    *  @brief
    *    Typeset (layout) the given text
    *
    *  @param[in] sequence
    *    Text to display
    *  @param[in] fontFace
    *    Font face to use
    *  @param[in] begin
    *    ???
    *  @param[in] dryrun
    *    ???
    *
    *  @return
    *    Extent (width, height) of the text ... in scaled space [TODO]
    */
    static glm::vec2 typeset(const GlyphSequence & sequence, const FontFace & fontFace, const std::vector<GlyphVertexCloud::Vertex>::iterator & begin, bool dryrun = false);


private:
    static bool typeset_wordwrap(
        const GlyphSequence & sequence
    ,   const FontFace & fontFace
    ,   const glm::vec2 & pen
    ,   const Glyph & glyph
    ,   const std::u32string::const_iterator & index
    ,   std::u32string::const_iterator & safe_forward);

    static std::u32string::const_iterator typeset_forward(
        const GlyphSequence & sequence
    ,   const FontFace & fontFace
    ,   const std::u32string::const_iterator & begin
    ,   float & width);

    static void typeset_glyph(
        const FontFace & fontFace
    ,   const glm::vec2 & pen
    ,   const Glyph & glyph
    ,   const std::vector<GlyphVertexCloud::Vertex>::iterator & vertex);

    static void typeset_extent(
        const FontFace & fontFace
    ,   std::u32string::const_iterator index
    ,   const std::u32string::const_iterator & begin
    ,   glm::vec2 & pen
    ,   glm::vec2 & extent);

    static void typeset_align(
        const glm::vec2 & pen
    ,   const Alignment alignment
    ,   const std::vector<GlyphVertexCloud::Vertex>::iterator & begin
    ,   const std::vector<GlyphVertexCloud::Vertex>::iterator & end);

    static void anchor_transform(
        const GlyphSequence & sequence
    ,   const FontFace & fontFace
    ,   const std::vector<GlyphVertexCloud::Vertex>::iterator & begin
    ,   const std::vector<GlyphVertexCloud::Vertex>::iterator & end);

    static void vertex_transform(
        const glm::mat4 & sequence
    ,   const glm::vec4 & fontColor
    ,   const std::vector<GlyphVertexCloud::Vertex>::iterator & begin
    ,   const std::vector<GlyphVertexCloud::Vertex>::iterator & end);

    static glm::vec2 extent_transform(
        const GlyphSequence & sequence
    ,   const glm::vec2 & extent);
};


} // namespace openll
