
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
    *
    *  @return
    *    Extent of the label (in output space)
    *
    *  @remarks
    *    This function internally performs the typesetting of the
    *    text, but only returns its extent, disregarding the resulting
    *    vertex array.
    *
    *  @notes
    *    - Before calling this function, a valid font face has to be set on the label.
    */
    static glm::vec2 extent(const Label & label);

    /**
    *  @brief
    *    Typeset (layout) the given text
    *
    *  @param[in,out] vertexCloud
    *    Vertex cloud that is constructed
    *  @param[in] label
    *    Label to display
    *  @param[in] optimize
    *    Optimize vertex cloud for rendering performance? (slow for large texts!)
    *  @param[in] dryrun
    *    Do not create output, just compute the extent?
    *
    *  @return
    *    Extent of the label (in output space)
    *
    *  @remarks
    *    Keep in mind that optimizing the vertex array is a slow process
    *    itself and also needs a lot of memory, as the vertex array has
    *    to be sorted. Therefore, optimization might not be advisable for
    *    large texts.
    *
    *  @notes
    *    - Before calling this function, a valid font face has to be set on the label.
    */
    static glm::vec2 typeset(GlyphVertexCloud & vertexCloud, const Label & label, bool optimize = false, bool dryrun = false);

    /**
    *  @brief
    *    Typeset (layout) the given text
    *
    *  @param[in,out] vertexCloud
    *    Vertex cloud that is constructed
    *  @param[in] labels
    *    List of labels to display
    *  @param[in] optimize
    *    Optimize vertex cloud for rendering performance? (slow for large texts!)
    *  @param[in] dryrun
    *    Do not create output, just compute the extent?
    *
    *  @return
    *    Extent of the label (in output space)
    *
    *  @remarks
    *    Keep in mind that optimizing the vertex array is a slow process
    *    itself and also needs a lot of memory, as the vertex array has
    *    to be sorted. Therefore, optimization might not be advisable for
    *    large texts.
    *
    *  @notes
    *    - Before calling this function, a valid font face has to be set on the label.
    *    - Each label has to use the same font face, as the resulting vertex cloud can
    *      only bind a single texture before rendering all glyphs. If this requirement
    *      is not met, the vertex cloud will use the first font face found, and an
    *      assertion will be thrown.
    */
    static glm::vec2 typeset(GlyphVertexCloud & vertexCloud, const std::vector<Label> & labels, bool optimize = false, bool dryrun = false);

    /**
    *  @brief
    *    Typeset (layout) the given text
    *
    *  @param[in,out] vertexCloud
    *    Vertex cloud that is constructed
    *  @param[in] labels
    *    List of labels to display
    *  @param[in] optimize
    *    Optimize vertex cloud for rendering performance? (slow for large texts!)
    *  @param[in] dryrun
    *    Do not create output, just compute the extent?
    *
    *  @return
    *    Extent of the label (in output space)
    *
    *  @remarks
    *    Keep in mind that optimizing the vertex array is a slow process
    *    itself and also needs a lot of memory, as the vertex array has
    *    to be sorted. Therefore, optimization might not be advisable for
    *    large texts.
    *
    *  @notes
    *    - Before calling this function, a valid font face has to be set on the label.
    *    - Each label has to use the same font face, as the resulting vertex cloud can
    *      only bind a single texture before rendering all glyphs. If this requirement
    *      is not met, the vertex cloud will use the first font face found, and an
    *      assertion will be thrown.
    */
    static glm::vec2 typeset(GlyphVertexCloud & vertexCloud, const std::vector<const Label *> & labels, bool optimize = false, bool dryrun = false);


private:
    /**
    *  @brief
    *    Typeset label
    *
    *  @param[in,out] vertices
    *    Vertex array
    *  @param[in,out] buckets
    *    Buckets for sorting the vertices (only used for optimize)
    *  @param[in] label
    *    Label to layout
    *  @param[in] optimize
    *    Optimize vertex cloud for rendering performance? (slow for large texts!)
    *  @param[in] dryrun
    *    Do not create output, just compute the extent?
    *
    *  @return
    *    Extent of the label (in output space)
    */
    static glm::vec2 typeset_label(
        std::vector<GlyphVertexCloud::Vertex> & vertices
    ,   std::map<size_t, std::vector<size_t>> & buckets
    ,   const Label & label
    ,   bool optimize = false
    ,   bool dryrun = false);

    /**
    *  @brief
    *    Determine whether the next word needs to be wrapped
    *
    *  @param[in] label
    *    Label to layout
    *  @param[in] lineWidth
    *    The maximum line width
    *  @param[in] pen
    *    Current typesetting position
    *  @param[in] glyph
    *    Glyph that is rendered
    *  @param[in] kerning
    *    Kerning space to previous character
    *
    *  @return
    *    'true' if word need to be wrapped, else 'false'
    */
    static bool typeset_wordwrap(
        const Label & label
    ,   float lineWidth
    ,   const glm::vec2 & pen
    ,   const Glyph & glyph
    ,   float kerning);

    /**
    *  @brief
    *    Configure the vertex for a given glyph to render
    *
    *    If no vertex is given or the glyph is not depictable,
    *    this method immediately exits at the beginning.
    *
    *  @param[in,out] vertices
    *    Vertex array
    *  @param[in,out] buckets
    *    Buckets for sorting the vertices (only used for optimize)
    *  @param[in] index
    *    Index of the current vertex
    *  @param[in] fontFace
    *    The used font face
    *  @param[in] pen
    *    Current typesetting position
    *  @param[in] glyph
    *    Glyph that is rendered
    *  @param[in] optimize
    *    Optimize vertex cloud for rendering performance? (slow for large texts!)
    */
    static void typeset_glyph(
        std::vector<GlyphVertexCloud::Vertex> & vertices
    ,   std::map<size_t, std::vector<size_t>> & buckets
    ,   size_t index
    ,   const glm::vec2 & pen
    ,   const Glyph & glyph
    ,   bool optimize);

    /**
    *  @brief
    *    Align glyphs
    *
    *  @param[in] pen
    *    Current typesetting position
    *  @param[in] alignment
    *    Text alignment
    *  @param[in,out] vertices
    *    Vertex array
    *  @param[in] begin
    *    Index of first vertex
    *  @param[in] end
    *    Index of last vertex
    */
    static void typeset_align(
        const glm::vec2 & pen
    ,   const Alignment alignment
    ,   std::vector<GlyphVertexCloud::Vertex> & vertices
    ,   size_t begin
    ,   size_t end);

    /**
    *  @brief
    *    Calculate transformed positions for the current glyphs
    *
    *    This configures the final vertex information for each glyph.
    *
    *  @param[in] label
    *    Label to layout
    *  @param[in] textColor
    *    Text color (rgba)
    *  @param[in,out] vertices
    *    Vertex array
    *  @param[in] begin
    *    Index of first vertex
    *  @param[in] end
    *    Index of last vertex
    */
    static void vertex_transform(
        const glm::mat4 & label
    ,   const glm::vec4 & textColor
    ,   std::vector<GlyphVertexCloud::Vertex> & vertices
    ,   size_t begin
    ,   size_t end);

    /**
    *  @brief
    *    Transform extent from layouting space into output space
    *
    *  @param[in] label
    *    Label to layout
    *  @param[in] extent
    *    Extent of the label (in layouting space)
    *
    *  @return
    *    Extent of the label (in scaled output space)
    */
    static glm::vec2 extent_transform(
        const Label & label
    ,   const glm::vec2 & extent);

    /**
    *  @brief
    *    Apply vertex array optimization
    *
    *  @param[in,out] vertices
    *    Vertex array
    *  @param[in] buckets
    *    Buckets for sorting the vertices
    */
    static void optimize_vertices(
        std::vector<GlyphVertexCloud::Vertex> & vertices
    ,   const std::map<size_t, std::vector<size_t>> & buckets);
};


} // namespace openll
