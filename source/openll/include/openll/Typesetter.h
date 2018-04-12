
#pragma once


#include <string>
#include <vector>
#include <map>

#include <glm/fwd.hpp>

#include <openll/GlyphVertexCloud.h>


namespace openll
{


enum class Alignment : unsigned char;
class Label;
class FontFace;
class Glyph;


/**
*  @brief
*    Typesetter that layouts text
*
*    The typesetter is responsible for layouting text on the screen or
*    in a virtual space. It takes a label, which defines where it wants
*    to appear (see Label), and a font face that is used to display the
*    text and computes the actual position for each glyph.
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
    *    Get the extent of the text when layouted with a given font size
    *
    *  @param[in] label
    *    Label to display
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
    static glm::vec2 extent(const Label & label, const FontFace & fontFace, float fontSize);

    /**
    *  @brief
    *    Typeset (layout) the given text
    *
    *  @param[in,out] vertexCloud
    *    Vertex cloud that is constructed
    *  @param[in] label
    *    Label to display
    *  @param[in] fontFace
    *    Font face to use
    *  @param[in] optimize
    *    Optimize vertex cloud for rendering performance? (slow for large texts!)
    *  @param[in] dryrun
    *    Do not create output, just compute the extent?
    *
    *  @return
    *    Extent (width, height) of the text ... in scaled space [TODO]
    *
    *  @remarks
    *    Keep in mind that optimizing the vertex array is a slow process
    *    itself, as each character of the text has to be processed and
    *    for each character it has to be determined, whether the glyph
    *    is visible or not. So the entire text has to be processed at
    *    least once. Therefore, optimization might not be advisable for
    *    large texts.
    */
    static glm::vec2 typeset(
        GlyphVertexCloud & vertexCloud
    ,   const Label & label
    ,   const FontFace & fontFace
    ,   bool optimize = false
    ,   bool dryrun = false);

    /**
    *  @brief
    *    Typeset (layout) the given text
    *
    *  @param[in,out] vertexCloud
    *    Vertex cloud that is constructed
    *  @param[in] labels
    *    List of labels to display
    *  @param[in] fontFace
    *    Font face to use
    *  @param[in] optimize
    *    Optimize vertex cloud for rendering performance? (slow for large texts!)
    *  @param[in] dryrun
    *    Do not create output, just compute the extent?
    *
    *  @return
    *    Extent (width, height) of the text ... in scaled space [TODO]
    *
    *  @remarks
    *    Keep in mind that optimizing the vertex array is a slow process
    *    itself, as each character of the text has to be processed and
    *    for each character it has to be determined, whether the glyph
    *    is visible or not. So the entire text has to be processed at
    *    least once. Therefore, optimization might not be advisable for
    *    large texts.
    */
    static glm::vec2 typeset(
        GlyphVertexCloud & vertexCloud
    ,   const std::vector<Label> & labels
    ,   const FontFace & fontFace
    ,   bool optimize = false
    ,   bool dryrun = false);

    /**
    *  @brief
    *    Typeset (layout) the given text
    *
    *  @param[in,out] vertexCloud
    *    Vertex cloud that is constructed
    *  @param[in] labels
    *    List of labels to display
    *  @param[in] fontFace
    *    Font face to use
    *  @param[in] optimize
    *    Optimize vertex cloud for rendering performance? (slow for large texts!)
    *  @param[in] dryrun
    *    Do not create output, just compute the extent?
    *
    *  @return
    *    Extent (width, height) of the text ... in scaled space [TODO]
    *
    *  @remarks
    *    Keep in mind that optimizing the vertex array is a slow process
    *    itself, as each character of the text has to be processed and
    *    for each character it has to be determined, whether the glyph
    *    is visible or not. So the entire text has to be processed at
    *    least once. Therefore, optimization might not be advisable for
    *    large texts.
    */
    static glm::vec2 typeset(
        GlyphVertexCloud & vertexCloud
    ,   const std::vector<const Label *> & labels
    ,   const FontFace & fontFace
    ,   bool optimize = false
    ,   bool dryrun = false);


private:
    static glm::vec2 typeset_label(
        std::vector<GlyphVertexCloud::Vertex> & vertices
    ,   std::map<size_t, std::vector<size_t>> & buckets
    ,   const Label & label
    ,   const FontFace & fontFace
    ,   bool optimize = false
    ,   bool dryrun = false);

    static bool typeset_wordwrap(
        const Label & label
    ,   const FontFace & fontFace
    ,   const glm::vec2 & pen
    ,   const Glyph & glyph
    ,   const std::u32string::const_iterator & index
    ,   std::u32string::const_iterator & safe_forward);

    static std::u32string::const_iterator typeset_forward(
        const Label & label
    ,   const FontFace & fontFace
    ,   const std::u32string::const_iterator & begin
    ,   float & width);

    static void typeset_glyph(
        const FontFace & fontFace
    ,   const glm::vec2 & pen
    ,   const Glyph & glyph
    ,   std::vector<GlyphVertexCloud::Vertex> & vertices
    ,   std::map<size_t, std::vector<size_t>> & buckets
    ,   size_t index);

    static void typeset_extent(
        const FontFace & fontFace
    ,   std::u32string::const_iterator index
    ,   const std::u32string::const_iterator & begin
    ,   glm::vec2 & pen
    ,   glm::vec2 & extent);

    static void typeset_align(
        const glm::vec2 & pen
    ,   const Alignment alignment
    ,   std::vector<GlyphVertexCloud::Vertex> & vertices
    ,   size_t begin
    ,   size_t end);

    static void anchor_transform(
        const Label & label
    ,   const FontFace & fontFace
    ,   std::vector<GlyphVertexCloud::Vertex> & vertices
    ,   size_t begin
    ,   size_t end);

    static void vertex_transform(
        const glm::mat4 & label
    ,   const glm::vec4 & textColor
    ,   std::vector<GlyphVertexCloud::Vertex> & vertices
    ,   size_t begin
    ,   size_t end);

    static glm::vec2 extent_transform(
        const Label & label
    ,   const glm::vec2 & extent);

    static void optimize_vertices(
        std::vector<GlyphVertexCloud::Vertex> & vertices
    ,   const std::map<size_t, std::vector<size_t>> & buckets);
};


} // namespace openll
