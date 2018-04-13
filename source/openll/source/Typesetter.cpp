
#include <openll/Typesetter.h>

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <openll/Text.h>
#include <openll/Alignment.h>
#include <openll/FontFace.h>
#include <openll/Label.h>


namespace openll
{


glm::vec2 Typesetter::extent(const Label & label)
{
    assert(label.fontFace() != nullptr);

    // Abort operation if no font face is set
    if (!label.fontFace()) {
        return glm::vec2();
    }

    // Prepare empty vertex list for dry run
    std::vector<GlyphVertexCloud::Vertex> vertices;
    std::map<size_t, std::vector<size_t>> buckets;

    // Typeset text with default font size
    return typeset_label(vertices, buckets, label, false, true);
}

glm::vec2 Typesetter::typeset(GlyphVertexCloud & vertexCloud, const Label & label, bool optimize, bool dryrun)
{
    assert(label.fontFace() != nullptr);

    // Abort operation if no font face is set
    if (!label.fontFace()) {
        return glm::vec2();
    }

    // Clear vertex cloud
    vertexCloud.vertices().clear();

    // Setup map for optimizing vertex array
    std::map<size_t, std::vector<size_t>> buckets;

    // Typeset single label
    auto extent = typeset_label(vertexCloud.vertices(), buckets, label, optimize, dryrun);

    // Optimize vertex cloud
    if (optimize) {
        optimize_vertices(vertexCloud.vertices(), buckets);
    }

    // Update vertex array
    vertexCloud.update();

    // Give back extent
    return extent;
}

glm::vec2 Typesetter::typeset(GlyphVertexCloud & vertexCloud, const std::vector<Label> & labels, bool optimize, bool dryrun)
{
    // Clear vertex cloud
    vertexCloud.vertices().clear();

    // Setup map for optimizing vertex array
    std::map<size_t, std::vector<size_t>> buckets;

    // Typeset labels
    glm::vec2 extent(0.0f, 0.0f);
    for (const auto & label : labels) {
        assert(label.fontFace() != nullptr);

        // Abort operation if no font face is set
        if (label.fontFace()) {
            auto currenExtent = typeset_label(vertexCloud.vertices(), buckets, label, optimize, dryrun);
            extent = glm::vec2(glm::max(extent.x, currenExtent.x), glm::max(extent.y, currenExtent.y));
        }
    }

    // Optimize vertex cloud
    if (optimize) {
        optimize_vertices(vertexCloud.vertices(), buckets);
    }

    // Update vertex array
    vertexCloud.update();

    // Give back extent
    return extent;
}

glm::vec2 Typesetter::typeset(GlyphVertexCloud & vertexCloud, const std::vector<const Label *> & labels, bool optimize, bool dryrun)
{
    // Clear vertex cloud
    vertexCloud.vertices().clear();

    // Setup map for optimizing vertex array
    std::map<size_t, std::vector<size_t>> buckets;

    // Typeset labels
    glm::vec2 extent(0.0f, 0.0f);
    for (const auto * label : labels) {
        // Abort if label is not valid
        assert(label);
        if (!label) continue;

        // Abort operation if no font face is set
        assert(label->fontFace() != nullptr);
        if (label->fontFace()) {
            auto currenExtent = typeset_label(vertexCloud.vertices(), buckets, *label, optimize, dryrun);
            extent = glm::vec2(glm::max(extent.x, currenExtent.x), glm::max(extent.y, currenExtent.y));
        }
    }

    // Optimize vertex cloud
    if (optimize) {
        optimize_vertices(vertexCloud.vertices(), buckets);
    }

    // Update vertex array
    vertexCloud.update();

    // Give back extent
    return extent;
}

glm::vec2 Typesetter::typeset_label(std::vector<GlyphVertexCloud::Vertex> & vertices, std::map<size_t, std::vector<size_t>> & buckets, const Label & label, bool optimize, bool dryrun)
{
    // Get font face
    auto & fontFace = *label.fontFace();

    // Append vertex cloud: the maximum number of visible glyphs is the size of the string
    vertices.reserve(vertices.size() + label.text()->text().size());

    // Create first vertex
    if (!dryrun) {
        vertices.push_back(GlyphVertexCloud::Vertex());
    }
    size_t begin = vertices.size() - 1;

    auto pen = glm::vec2(0.f);
    auto vertex = begin;
    auto extent = glm::vec2(0.f);

    const auto iBegin = label.text()->text().cbegin();
    const auto iEnd = label.text()->text().cend();

    // Iterator used to reduce the number of wordwrap forward passes
    auto safe_forward = iBegin;

    auto feedLine = false;
    auto feedVertex = vertex;

    for (auto i = iBegin; i != iEnd; ++i)
    {
        const auto & glyph = fontFace.glyph(*i);

        // Handle line feeds as well as word wrap for next word
        // (or next glyph if word width exceeds the max line width)
        feedLine = *i == label.text()->lineFeed() || (label.wordWrap() &&
            typeset_wordwrap(label, fontFace, pen, glyph, i, safe_forward));

        if (feedLine)
        {
            assert(i != iBegin);
            typeset_extent(fontFace, i - 1, iBegin, pen, extent);

            // Handle alignment (when line feed occurs)
            if (!dryrun) {
                typeset_align(pen, label.alignment(), vertices, feedVertex, vertex);
            }

            pen.x = 0.f;
            pen.y -= fontFace.lineHeight();

            feedLine = false;
            feedVertex = vertex;
        }
        else if (i != iBegin)
        {   // Apply kerning
            pen.x += fontFace.kerning(*(i - 1), *i);
        }

        // Typeset glyphs in vertex cloud (only if renderable)
        if (!dryrun && glyph.depictable()) {
            vertices.push_back(GlyphVertexCloud::Vertex());
            typeset_glyph(vertices, buckets, vertex++, fontFace, pen, glyph, optimize && !dryrun);
        }

        pen.x += glyph.advance();

        if (i + 1 == iEnd)
        {
            // Handle alignment (when last line of the label is processed)
            typeset_extent(fontFace, i, iBegin, pen, extent);

            if (!dryrun) {
                typeset_align(pen, label.alignment(), vertices, feedVertex, vertex);
            }
        }
    }

    if (!dryrun) {
        anchor_transform(label, fontFace, vertices, begin, vertex);
        vertex_transform(label.transform(), label.textColor(), vertices, begin, vertex);
    }

    // Scale extent back to the label's font size (it has been computed with the font face's font size)
    return extent_transform(label, extent) * label.fontSize() / label.fontFace()->size();
}

inline bool Typesetter::typeset_wordwrap(
  const Label & label
, const FontFace & fontFace
, const glm::vec2 & pen
, const Glyph & glyph
, const std::u32string::const_iterator & index
, std::u32string::const_iterator & safe_forward)
{
    assert(label.wordWrap());

    const auto lineWidth = glm::max(label.lineWidth() * label.fontFace()->size() / label.fontSize(), 0.0f);
    auto width_forward = 0.f;

    const auto pen_glyph =
        pen.x + glyph.advance() +
            (index != label.text()->text().cbegin() ? fontFace.kerning(*(index - 1), *index) : 0.f);

    const auto wrap_glyph = glyph.depictable() && pen_glyph > lineWidth
        && (glyph.advance() <= lineWidth || pen.x > 0.f);

    auto wrap_forward = false;
    if (!wrap_glyph && index >= safe_forward)
    {
        safe_forward = typeset_forward(label, fontFace, index, width_forward);
        wrap_forward = width_forward <= lineWidth && (pen.x + width_forward) > lineWidth;
    }

    return wrap_forward || wrap_glyph;
}

inline std::u32string::const_iterator Typesetter::typeset_forward(
  const Label & label
, const FontFace & fontFace
, const std::u32string::const_iterator & index
, float & width)
{
    // Setup common delimiters
    // Note: u32string::find outperforms set::count here (tested)
    static const auto delimiters = std::u32string({ '\x0A', ' ', ',', '.', '-', '/', '(', ')', '[', ']', '<', '>' });

    const auto iBegin = label.text()->text().cbegin();
    const auto iEnd = label.text()->text().cend();

    width = 0.f; // reset the width

    // Accumulate glyph advances (including kerning) up to the next delimiter occurence
    auto i = index;
    while (i != iEnd && delimiters.find(*i) == delimiters.npos)
    {
        if (i != iBegin) {
            width += fontFace.kerning(*(i - 1), *i);
        }

        width += fontFace.glyph(*i++).advance();
    }

    return i;
}

inline void Typesetter::typeset_glyph(
  std::vector<GlyphVertexCloud::Vertex> & vertices
, std::map<size_t, std::vector<size_t>> & buckets
, size_t index
, const FontFace & fontFace
, const glm::vec2 & pen
, const Glyph & glyph
, bool optimize)
{
    auto & vertex = vertices[index];

    const auto & padding = fontFace.glyphTexturePadding();
    vertex.origin    = glm::vec3(pen, 0.f);
    vertex.origin.x += glyph.bearing().x - padding[3];
    vertex.origin.y += glyph.bearing().y - glyph.extent().y - padding[2];

    vertex.vtan   = glm::vec3(glyph.extent().x + padding[1] + padding[3], 0.f, 0.f);
    vertex.vbitan = glm::vec3(0.f, glyph.extent().y + padding[0] + padding[2], 0.f);

    const auto extentScale = 1.f / glm::vec2(fontFace.glyphTextureExtent());
    const auto ll = glyph.subTextureOrigin()
        - glm::vec2(padding[3], padding[2]) * extentScale;
    const auto ur = glyph.subTextureOrigin() + glyph.subTextureExtent()
        + glm::vec2(padding[1], padding[0]) * extentScale;
    vertex.uvRect = glm::vec4(ll, ur);

    if (optimize) {
        auto glyphIndex = glyph.index();
        buckets[glyphIndex].push_back(index);
    }
}

inline void Typesetter::typeset_extent(
  const FontFace & fontFace
, std::u32string::const_iterator index
, const std::u32string::const_iterator & begin
, glm::vec2 & pen
, glm::vec2 & extent)
{
    // On line feed, revert advance of preceding, not depictable glyphs
    while (index > begin)
    {
        auto precedingGlyph = fontFace.glyph(*index);
        if (precedingGlyph.depictable()) {
            break;
        }

        pen.x -= precedingGlyph.advance();
        --index;
    }

    extent.x = glm::max(pen.x, extent.x);
    extent.y += fontFace.lineHeight();
}

inline void Typesetter::typeset_align(
  const glm::vec2 & pen
, const Alignment alignment
, std::vector<GlyphVertexCloud::Vertex> & vertices
, size_t begin
, size_t end)
{
    if (alignment == Alignment::LeftAligned) {
        return;
    }

    auto penOffset = -pen.x;

    if (alignment == Alignment::Centered) {
        penOffset *= 0.5f;
    }

    // Origin is expected to be in 'font face space' (not transformed)
    for (auto i = begin; i != end; ++i) {
        auto & v = vertices[i];
        v.origin.x += penOffset;
    }
}

inline void Typesetter::anchor_transform(
  const Label & label
, const FontFace & fontFace
, std::vector<GlyphVertexCloud::Vertex> & vertices
, size_t begin
, size_t end)
{
    auto offset = 0.f;

    switch (label.lineAnchor())
    {
    case LineAnchor::Ascent:
        offset = fontFace.ascent();
        break;

    case LineAnchor::Center:
        offset = fontFace.size() * 0.5f + fontFace.descent();
        break;

    case LineAnchor::Descent:
        offset = fontFace.descent();
        break;

    case LineAnchor::Baseline:
    default:
        return;
    }

    for (auto i = begin; i != end; ++i) {
        auto & v = vertices[i];
        v.origin.y -= offset;
    }
}

inline void Typesetter::vertex_transform(
  const glm::mat4 & transform
, const glm::vec4 & textColor
, std::vector<GlyphVertexCloud::Vertex> & vertices
, size_t begin
, size_t end)
{
    for (auto i = begin; i != end; ++i)
    {
        auto & v = vertices[i];

        auto ll = transform * glm::vec4(v.origin, 1.f);
        auto lr = transform * glm::vec4(v.origin + v.vtan, 1.f);
        auto ul = transform * glm::vec4(v.origin + v.vbitan, 1.f);

        v.origin = glm::vec3(ll);
        v.vtan   = glm::vec3(lr - ll);
        v.vbitan = glm::vec3(ul - ll);
        v.textColor = textColor;
    }
}

inline glm::vec2 Typesetter::extent_transform(
  const Label & label
, const glm::vec2 & extent)
{
    auto ll = label.transform() * glm::vec4(     0.f,      0.f, 0.f, 1.f);
    auto lr = label.transform() * glm::vec4(extent.x,      0.f, 0.f, 1.f);
    auto ul = label.transform() * glm::vec4(     0.f, extent.y, 0.f, 1.f);

    return glm::vec2(glm::distance(lr, ll), glm::distance(ul, ll));
}

inline void Typesetter::optimize_vertices(std::vector<GlyphVertexCloud::Vertex> & vertices, const std::map<size_t, std::vector<size_t>> & buckets)
{
    std::vector<GlyphVertexCloud::Vertex> sorted;

    sorted.reserve(vertices.size());

    for (auto & it : buckets)
    {
        auto & bucket = it.second;

        for (size_t i=0; i<bucket.size(); i++)
        {
            sorted.emplace_back(vertices[bucket[i]]);
        }
    }

    std::swap(vertices, sorted);
}


} // namespace openll
