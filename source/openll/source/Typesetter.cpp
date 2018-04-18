
#include <openll/Typesetter.h>

#include <set>

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


namespace
{


// Setup common delimiters for wordwrapping
// Note: u32string::find outperforms set::count here (tested)
static const auto delimiters = std::u32string({ '\x0A', ' ', ',', '.', '-', '/', '(', ')', '[', ']', '<', '>', '.' });
static const auto delimiterSet = std::set<char32_t>(delimiters.begin(), delimiters.end());


} // namespace


namespace openll
{


glm::vec2 Typesetter::extent(const Label & label)
{
    assert(label.fontFace() != nullptr);

    // Abort operation if no font face is set
    if (!label.fontFace())
    {
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
    if (!label.fontFace())
    {
        return glm::vec2();
    }

    // Clear vertex cloud
    vertexCloud.vertices().clear();

    // Setup map for optimizing vertex array
    std::map<size_t, std::vector<size_t>> buckets;

    // Typeset single label
    auto extent = typeset_label(vertexCloud.vertices(), buckets, label, optimize, dryrun);

    // Optimize vertex cloud
    if (optimize)
    {
        optimize_vertices(vertexCloud.vertices(), buckets);
    }

    // Update vertex array
    vertexCloud.update();

    // Set font texture
    vertexCloud.setTexture(label.fontFace()->glyphTexture());

    // Give back extent
    return extent;
}

glm::vec2 Typesetter::typeset(GlyphVertexCloud & vertexCloud, const std::vector<Label> & labels, bool optimize, bool dryrun)
{
    const FontFace * fontFace = nullptr;

    // Clear vertex cloud
    vertexCloud.vertices().clear();

    // Setup map for optimizing vertex array
    std::map<size_t, std::vector<size_t>> buckets;

    // Typeset labels
    glm::vec2 extent(0.0f, 0.0f);
    for (const auto & label : labels)
    {
        // Check that font face is valid and the same font face is used for all labels
        assert(label.fontFace() != nullptr);
        assert(label.fontFace() == fontFace || fontFace == nullptr);

        // Remember font face from first label
        if (!fontFace)
        {
            fontFace = label.fontFace();
        }

        // Abort operation if no font face is set
        if (label.fontFace())
        {
            // Typeset label
            auto currenExtent = typeset_label(vertexCloud.vertices(), buckets, label, optimize, dryrun);
            extent = glm::vec2(glm::max(extent.x, currenExtent.x), glm::max(extent.y, currenExtent.y));
        }
    }

    // Optimize vertex cloud
    if (optimize)
    {
        optimize_vertices(vertexCloud.vertices(), buckets);
    }

    // Update vertex array
    vertexCloud.update();

    // Set font texture
    vertexCloud.setTexture(fontFace->glyphTexture());

    // Give back extent
    return extent;
}

glm::vec2 Typesetter::typeset(GlyphVertexCloud & vertexCloud, const std::vector<const Label *> & labels, bool optimize, bool dryrun)
{
    const FontFace * fontFace = nullptr;

    // Clear vertex cloud
    vertexCloud.vertices().clear();

    // Setup map for optimizing vertex array
    std::map<size_t, std::vector<size_t>> buckets;

    // Typeset labels
    glm::vec2 extent(0.0f, 0.0f);
    for (const auto * label : labels)
    {
        // Abort if label is not valid
        assert(label);
        if (!label)
        {
            continue;
        }

        // Check that font face is valid and the same font face is used for all labels
        assert(label->fontFace() != nullptr);
        assert(fontFace == nullptr || label->fontFace() == fontFace);

        // Abort operation if no font face is set
        if (label->fontFace())
        {
            // Remember font face from first label
            if (!fontFace)
            {
                fontFace = label->fontFace();
            }

            // Typeset label
            auto currenExtent = typeset_label(vertexCloud.vertices(), buckets, *label, optimize, dryrun);
            extent = glm::vec2(glm::max(extent.x, currenExtent.x), glm::max(extent.y, currenExtent.y));
        }
    }

    // Optimize vertex cloud
    if (optimize)
    {
        optimize_vertices(vertexCloud.vertices(), buckets);
    }

    // Update vertex array
    vertexCloud.update();

    // Give back extent
    return extent;
}

glm::vec2 Typesetter::typeset_label(std::vector<GlyphVertexCloud::Vertex> & vertices, std::map<size_t, std::vector<size_t>> & buckets, const Label & label, bool optimize, bool dryrun)
{
    struct LineEndInformation
    {
        glm::vec2 pen;
        glm::vec2 lastDepictablePen;
        size_t startGlyphIndex;
        size_t endGlyphIndex;
    };

    // Get font face
    const auto & fontFace = *label.fontFace();

    // Append vertex cloud: the maximum number of visible glyphs is the size of the string
    if (!dryrun)
    {
        vertices.reserve(vertices.size() + label.text()->text().size());
    }

    size_t glyphCloudStart = vertices.size();

    auto extent = glm::vec2(0.f);

    const auto itBegin = label.text()->text().cbegin();
    const auto itEnd = label.text()->text().cend();

    LineEndInformation currentLine = { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glyphCloudStart, glyphCloudStart };
    LineEndInformation lineForward = { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glyphCloudStart, glyphCloudStart };

    const auto lineWidth = glm::max(label.lineWidth() * label.fontFace()->size() / label.fontSize(), 0.0f);

    auto index = glyphCloudStart;
    for (auto it = itBegin; it != itEnd; ++it)
    {
        const auto & glyph = fontFace.glyph(*it);

        // Handle line feeds as well as word wrap for next word
        // (or next glyph if word width exceeds the max line width)
        const auto feedLine = *it == label.text()->lineFeed() || (label.wordWrap() &&
            typeset_wordwrap(label, fontFace, lineWidth, lineForward.pen, glyph, it));

        if (feedLine)
        {
            assert(it != itBegin);

            extent.x = glm::max(currentLine.pen.x, extent.x);
            extent.y += fontFace.lineHeight();

            const auto lineHeight = fontFace.lineHeight();
            lineForward.pen.y -= lineHeight;

            // Handle newline and alignment
            auto newPenX = lineForward.pen.x - currentLine.pen.x;
            if (!dryrun)
            {
                typeset_align(currentLine.lastDepictablePen, label.alignment(), vertices, currentLine.startGlyphIndex, currentLine.endGlyphIndex);

                const auto xOffset = vertices[lineForward.startGlyphIndex].origin.x;

                for (auto j = lineForward.startGlyphIndex; j != lineForward.endGlyphIndex; ++j)
                {
                    auto & v = vertices[j];
                    v.origin.x -= xOffset;
                    v.origin.y -= lineHeight;
                }

                if (lineForward.startGlyphIndex >= index)
                {
                    newPenX = 0.0f;
                }
                else
                {
                    newPenX = lineForward.pen.x - xOffset;
                }
            }

            currentLine.pen = glm::vec2(newPenX, lineForward.pen.y);
            currentLine.startGlyphIndex = lineForward.startGlyphIndex;
            currentLine.endGlyphIndex = lineForward.endGlyphIndex;
            currentLine.lastDepictablePen = currentLine.pen;
            lineForward.startGlyphIndex = lineForward.endGlyphIndex;
            lineForward.pen = currentLine.pen;
            lineForward.lastDepictablePen = lineForward.pen;
        }
        else if (index > currentLine.startGlyphIndex)
        {   // Apply kerning
            lineForward.pen.x += fontFace.kerning(*(it - 1), *it);
        }

        // Typeset glyphs in vertex cloud (only if renderable)
        if (!dryrun && glyph.depictable())
        {
            vertices.push_back(GlyphVertexCloud::Vertex());
            typeset_glyph(vertices, buckets, index, fontFace, lineForward.pen, glyph, optimize && !dryrun);
            ++index;
            lineForward.endGlyphIndex = index;
        }

        lineForward.pen.x += glyph.advance();

        if (glyph.depictable())
        {
            lineForward.lastDepictablePen = lineForward.pen;
        }

        if (feedLine || delimiterSet.find(glyph.index()) != delimiterSet.end())
        {
            currentLine.lastDepictablePen = lineForward.lastDepictablePen;
            currentLine.endGlyphIndex = lineForward.endGlyphIndex;
            currentLine.pen = lineForward.lastDepictablePen;
            lineForward.startGlyphIndex = lineForward.endGlyphIndex;
        }
    }

    // Handle alignment (when last line of the label is processed)
    extent.x = glm::max(lineForward.lastDepictablePen.x, extent.x);
    extent.y += fontFace.lineHeight();

    if (!dryrun)
    {
        typeset_align(lineForward.lastDepictablePen, label.alignment(), vertices, currentLine.startGlyphIndex, lineForward.endGlyphIndex);
        anchor_transform(label, fontFace, vertices, glyphCloudStart, lineForward.endGlyphIndex);
        vertex_transform(label.transform(), label.textColor(), vertices, glyphCloudStart, lineForward.endGlyphIndex);
    }

    return extent_transform(label, extent);
}

inline bool Typesetter::typeset_wordwrap(
  const Label & label
, const FontFace & fontFace
, float lineWidth
, const glm::vec2 & pen
, const Glyph & glyph
, const std::u32string::const_iterator & index)
{
    assert(label.wordWrap());

    if (!glyph.depictable())
    {
        return false;
    }

    const auto pen_glyph =
        pen.x + glyph.advance() +
            (index != label.text()->text().cbegin() ? fontFace.kerning(*(index - 1), *index) : 0.f);

    return pen_glyph > lineWidth
            && (glyph.advance() <= lineWidth || pen.x > 0.f);
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

    const auto & extentScale = fontFace.inverseGlyphTextureExtent();
    const auto ll = glyph.subTextureOrigin()
        - glm::vec2(padding[3], padding[2]) * extentScale;
    const auto ur = glyph.subTextureOrigin() + glyph.subTextureExtent()
        + glm::vec2(padding[1], padding[0]) * extentScale;
    vertex.uvRect = glm::vec4(ll, ur);

    if (optimize)
    {
        auto glyphIndex = glyph.index();
        buckets[glyphIndex].push_back(index);
    }
}

inline void Typesetter::typeset_align(
  const glm::vec2 & pen
, const Alignment alignment
, std::vector<GlyphVertexCloud::Vertex> & vertices
, size_t begin
, size_t end)
{
    if (alignment == Alignment::LeftAligned)
    {
        return;
    }

    auto penOffset = -pen.x;

    if (alignment == Alignment::Centered)
    {
        penOffset *= 0.5f;
    }

    // Origin is expected to be in 'font face space' (not transformed)
    for (auto i = begin; i != end; ++i)
    {
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

    for (auto i = begin; i != end; ++i)
    {
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

        const auto ll = transform * glm::vec4(v.origin, 1.f);
        const auto lr = transform * glm::vec4(v.origin + v.vtan, 1.f);
        const auto ul = transform * glm::vec4(v.origin + v.vbitan, 1.f);

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
    const auto ll = label.transform() * glm::vec4(     0.f,      0.f, 0.f, 1.f);
    const auto lr = label.transform() * glm::vec4(extent.x,      0.f, 0.f, 1.f);
    const auto ul = label.transform() * glm::vec4(     0.f, extent.y, 0.f, 1.f);

    return glm::vec2(glm::distance(lr, ll), glm::distance(ul, ll));
}

inline void Typesetter::optimize_vertices(std::vector<GlyphVertexCloud::Vertex> & vertices, const std::map<size_t, std::vector<size_t>> & buckets)
{
    std::vector<GlyphVertexCloud::Vertex> sorted;

    sorted.reserve(vertices.size());

    for (const auto & it : buckets)
    {
        const auto & bucket = it.second;

        for (size_t i=0; i<bucket.size(); ++i)
        {
            sorted.emplace_back(vertices[bucket[i]]);
        }
    }

    std::swap(vertices, sorted);
}


} // namespace openll
