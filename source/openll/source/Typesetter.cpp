
#include <openll/Typesetter.h>

#include <set>
#include <algorithm>
#include <vector>
#include <mutex>

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
auto delimiters = std::u32string({ '\x0A', ' ', ',', '.', '-', '/', '(', ')', '[', ']', '<', '>', '.' });
const auto delimiterSet = std::set<char32_t>(delimiters.begin(), delimiters.end());
std::once_flag onceFlag;

inline bool isDelimiter(const char32_t character)
{
    // Set test
    // return delimiterSet.find(character) != delimiterSet.end();

    // Vector search
    // return std::find(delimiters.begin(), delimiters.end(), character) != delimiters.end();

    // Sorted vector binary search
    std::call_once( onceFlag, []{ std::sort(delimiters.begin(), delimiters.end()); });

    return std::binary_search(delimiters.begin(), delimiters.end(), character);
}


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

glm::vec2 Typesetter::typeset(GlyphVertexCloud & vertexCloud, const std::vector<Label> & labels, bool optimize, bool dryrun, std::vector<std::pair<std::uint32_t, std::uint32_t>> * positions)
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
            const auto startIndex = std::uint32_t(vertexCloud.vertices().size());
            const auto currentExtent = typeset_label(vertexCloud.vertices(), buckets, label, optimize, dryrun);
            extent = glm::max(extent, currentExtent);

            if (positions != nullptr)
            {
                const auto endIndex = std::uint32_t(vertexCloud.vertices().size());
                positions->emplace_back(startIndex, endIndex);
            }
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
    if (fontFace != nullptr)
    {
        vertexCloud.setTexture(fontFace->glyphTexture());
    }

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

inline glm::vec2 Typesetter::typeset_label(std::vector<GlyphVertexCloud::Vertex> & vertices, std::map<size_t, std::vector<size_t>> & buckets, const Label & label, bool optimize, bool dryrun)
{
    struct SegmentInformation
    {
        glm::vec2 firstDepictablePen;
        glm::vec2 lastDepictablePen;
        size_t startGlyphIndex;
    };

    // Get font face
    const auto & fontFace = *label.fontFace();

    // Append vertex cloud: the maximum number of visible glyphs is the size of the string
    if (!dryrun)
    {
        vertices.reserve(vertices.size() + label.text()->text().size());
    }

    size_t glyphCloudStart = vertices.size();

    auto extent = glm::vec2(0.0f, 0.0f);

    const auto itBegin = label.text()->text().cbegin();
    const auto itEnd = label.text()->text().cend();

    auto currentPen = glm::vec2(0.0f, label.lineAnchorOffset());
    SegmentInformation currentLine = { currentPen, currentPen, glyphCloudStart };
    SegmentInformation lineForward = { currentPen, currentPen, glyphCloudStart };

    const auto lineWidth = glm::max(label.lineWidth() * label.fontFace()->size() / label.fontSize(), 0.0f);

    auto index = glyphCloudStart;
    bool firstDepictablePenInvalid = true;
    for (auto it = itBegin; it != itEnd; ++it)
    {
        const auto & glyph = fontFace.glyph(*it);

        if (firstDepictablePenInvalid && glyph.depictable())
        {
            currentLine.firstDepictablePen = currentPen;
            firstDepictablePenInvalid = false;
        }

        // Handle line feeds as well as word wrap for next word
        // (or next glyph if word width exceeds the max line width)
        const auto kerning = (it != itBegin ? fontFace.kerning(*(it - 1), *it) : 0.f);
        const auto feedLine = *it == label.text()->lineFeed() || (label.wordWrap() &&
            typeset_wordwrap(label, lineWidth, currentPen, glyph, kerning));

        if (feedLine)
        {
            assert(it != itBegin);

            extent.x = glm::max(currentLine.lastDepictablePen.x, extent.x);
            extent.y += fontFace.lineHeight();

            const auto lineHeight = fontFace.lineHeight();

            currentPen.y -= lineHeight;

            // Handle newline and alignment
            if (!dryrun)
            {
                typeset_align(currentLine.lastDepictablePen, label.alignment(), vertices, currentLine.startGlyphIndex, lineForward.startGlyphIndex);

                // Omit relayouting
                const auto xOffset = currentLine.firstDepictablePen.x;

                for (auto j = lineForward.startGlyphIndex; j != index; ++j)
                {
                    auto & v = vertices[j];
                    v.origin.x -= xOffset;

                    v.origin.y -= lineHeight;
                }

                currentPen.x = std::max(lineForward.startGlyphIndex >= index ? 0.0f : currentPen.x - xOffset, 0.0f);
                currentLine.startGlyphIndex = lineForward.startGlyphIndex;
                lineForward.startGlyphIndex = index;
            }
            else
            {
                currentPen.x = lineForward.lastDepictablePen.x - currentLine.firstDepictablePen.x;
            }

            currentLine.lastDepictablePen = currentPen;
            lineForward.firstDepictablePen = glm::vec2(0.0f, currentPen.y);
            lineForward.lastDepictablePen = currentPen;
        }
        else
        {   // Apply kerning if no line feed precedes
            currentPen.x += kerning;
        }

        // Typeset glyphs in vertex cloud (only if renderable)
        if (!dryrun && glyph.depictable())
        {
            vertices.push_back(GlyphVertexCloud::Vertex());
            typeset_glyph(vertices, buckets, index, currentPen, glyph, optimize);
            ++index;
        }

        currentPen.x += glyph.advance();

        if (glyph.depictable())
        {
            lineForward.lastDepictablePen = currentPen;
        }

        if (feedLine || isDelimiter(glyph.index()))
        {
            currentLine.lastDepictablePen = lineForward.lastDepictablePen;
            firstDepictablePenInvalid = true;

            if (!dryrun)
            {
                lineForward.startGlyphIndex = index;
            }
        }
    }

    // Handle alignment (when last line of the label is processed)
    extent.x = glm::max(lineForward.lastDepictablePen.x, extent.x);
    extent.y += fontFace.lineHeight();

    if (!dryrun)
    {
        typeset_align(lineForward.lastDepictablePen, label.alignment(), vertices, currentLine.startGlyphIndex, index);
        vertex_transform(label.transform(), label.textColor(), vertices, glyphCloudStart, index);
    }

    return extent_transform(label, extent);
}

inline bool Typesetter::typeset_wordwrap(
  const Label & label
, float lineWidth
, const glm::vec2 & pen
, const Glyph & glyph
, const float kerning)
{
    assert(label.wordWrap());

    if (!glyph.depictable() || (glyph.advance() > lineWidth && pen.x <= 0.0f))
    {
        return false;
    }

    return pen.x + glyph.advance() + kerning > lineWidth;
}

inline void Typesetter::typeset_glyph(
  std::vector<GlyphVertexCloud::Vertex> & vertices
, std::map<size_t, std::vector<size_t>> & buckets
, size_t index
, const glm::vec2 & pen
, const Glyph & glyph
, bool optimize)
{
    assert(pen.x >= 0.0f);

    auto & vertex = vertices[index];

    vertex.origin = glm::vec3(pen + glyph.penOrigin(), 0.0f);
    vertex.vtan   = glm::vec3(glyph.penTangent(), 0.f);
    vertex.vbitan = glm::vec3(glyph.penBitangent(), 0.f);
    vertex.uvRect = glyph.subtextureRectangle();

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

        assert(v.origin.z == 0.0f);

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
