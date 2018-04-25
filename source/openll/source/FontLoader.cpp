
#include <openll/FontLoader.h>

#include <fstream>
#include <sstream>
#include <string>
#include <functional>
#include <set>
#include <map>

#include <cppassist/memory/make_unique.h>
#include <cppassist/logging/logging.h>
#include <cppassist/string/conversion.h>
#include <cppassist/string/manipulation.h>
#include <cppassist/fs//FilePath.h>
#include <cppassist/fs/RawFile.h>

#include <glbinding/gl/enum.h>

#include <openll/FontFace.h>


namespace openll
{


std::unique_ptr<FontFace> FontLoader::load(const std::string & filename)
{
    // Open file
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in)
    {
        return nullptr;
    }

    // Create font face
    auto fontFace = cppassist::make_unique<FontFace>();

    // Initialize font info
    auto identifier = std::string();
    auto fontSize   = 0.0f;

    // Read file line by line
    auto line = std::string();
    while (std::getline(in, line))
    {
        // Read next line
        std::stringstream ss;
        ss << line;

        // Read line identifier
        if (!std::getline(ss, identifier, ' '))
        {
            assert(false);
            continue;
        }

        // Parse line
        if (identifier == "info")
        {
            parseInfo(ss, *fontFace, fontSize);
        }
        else if (identifier == "common")
        {
            parseCommon(ss, *fontFace, fontSize);
        }
        else if (identifier == "page")
        {
            parsePage(ss, *fontFace, filename);
        }
        else if (identifier == "char")
        {
            parseChar(ss, *fontFace);
        }
        else if (identifier == "kerning")
        {
            parseKerning(ss, *fontFace);
        }
    }

    // Check if font has been loaded successfully
    if (fontFace->glyphTexture())
    {
        return fontFace;
    }

    // Otherwise delete font face and return error
    return nullptr;
}

void FontLoader::parseInfo(std::stringstream & stream, FontFace & fontFace, float & fontSize)
{
    auto pairs = readKeyValuePairs(stream, { "size", "padding" });

    fontSize = cppassist::string::fromString<float>(pairs.at("size"));

    auto values = cppassist::string::split(pairs.at("padding"), ',');
    assert(values.size() == 4);

    auto padding = glm::vec4();
    padding[0] = cppassist::string::fromString<float>(values[2]); // top
    padding[1] = cppassist::string::fromString<float>(values[1]); // right
    padding[2] = cppassist::string::fromString<float>(values[3]); // bottom
    padding[3] = cppassist::string::fromString<float>(values[0]); // left

    fontFace.setGlyphTexturePadding(padding);
}

void FontLoader::parseCommon(std::stringstream & stream, FontFace & fontFace, const float fontSize)
{
    auto pairs = readKeyValuePairs(stream, { "lineHeight", "base", "scaleW", "scaleH" });

    fontFace.setAscent(cppassist::string::fromString<float>(pairs.at("base")));
    fontFace.setDescent(fontFace.ascent() - fontSize);

    assert(fontFace.size() > 0.f);
    fontFace.setLineHeight(cppassist::string::fromString<float>(pairs.at("lineHeight")));

    fontFace.setGlyphTextureExtent({
        cppassist::string::fromString<float>(pairs.at("scaleW")),
        cppassist::string::fromString<float>(pairs.at("scaleH"))
    });
}

void FontLoader::parsePage(std::stringstream & stream, FontFace & fontFace, const std::string & filename)
{
    auto pairs = readKeyValuePairs(stream, { "file" });

    const auto path = cppassist::FilePath(filename).directoryPath();
    const auto file = cppassist::string::stripped(pairs.at("file"), { '"', '\r' });

    if (cppassist::string::hasSuffix(file, ".raw"))
    {
        auto texture = new globjects::Texture(gl::GL_TEXTURE_2D);
        auto raw = cppassist::RawFile();
        raw.load(path + "/" + file);

        if (!raw.isValid())
        {
            assert(false);
            return;
        }

        texture->image2D(0, gl::GL_R8, fontFace.glyphTextureExtent(), 0
            , gl::GL_RED, gl::GL_UNSIGNED_BYTE, raw.data());

        fontFace.setGlyphTexture(std::unique_ptr<globjects::Texture>(texture));
    }

    fontFace.glyphTexture()->setParameter(gl::GL_TEXTURE_MIN_FILTER, gl::GL_LINEAR);
    fontFace.glyphTexture()->setParameter(gl::GL_TEXTURE_MAG_FILTER, gl::GL_LINEAR);
    fontFace.glyphTexture()->setParameter(gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);
    fontFace.glyphTexture()->setParameter(gl::GL_TEXTURE_WRAP_T, gl::GL_CLAMP_TO_EDGE);
}

void FontLoader::parseChar(std::stringstream & stream, FontFace & fontFace)
{
    auto pairs = readKeyValuePairs(stream, { "id", "x", "y", "width", "height", "xoffset", "yoffset", "xadvance" });

    auto index = cppassist::string::fromString<size_t>(pairs.at("id"));
    assert(index > 0);

    auto glyph = Glyph(nullptr);

    glyph.setIndex(index);

    const auto extentScale = 1.f / glm::vec2(fontFace.glyphTextureExtent());
    const auto extent = glm::vec2(
        cppassist::string::fromString<float>(pairs.at("width")),
        cppassist::string::fromString<float>(pairs.at("height"))
    );

    glyph.setSubTextureOrigin({
        cppassist::string::fromString<float>(pairs.at("x")) * extentScale.x,
        1.f - (cppassist::string::fromString<float>(pairs.at("y")) + extent.y) * extentScale.y
    });

    glyph.setExtent(extent);
    glyph.setSubTextureExtent(extent * extentScale);

    glyph.setBearing(fontFace.ascent(),
        cppassist::string::fromString<float>(pairs.at("xoffset")),
        cppassist::string::fromString<float>(pairs.at("yoffset"))
    );

    glyph.setAdvance(cppassist::string::fromString<float>(pairs.at("xadvance")));

    fontFace.addGlyph(glyph);
}

void FontLoader::parseKerning(std::stringstream & stream, FontFace & fontFace)
{
    auto pairs = readKeyValuePairs(stream, { "first", "second", "amount" });

    auto first = cppassist::string::fromString<size_t>(pairs.at("first"));
    assert(first > 0);

    auto second = cppassist::string::fromString<size_t>(pairs.at("second"));
    assert(second > 0);

    auto kerning = cppassist::string::fromString<float>(pairs.at("amount"));

    fontFace.setKerning(first, second, kerning);
}

std::map<std::string, std::string> FontLoader::readKeyValuePairs(std::stringstream & stream, const std::initializer_list<const char *> & mandatoryKeys)
{
    auto key = std::string();
    auto value = std::string();

    auto pairs = std::map<std::string, std::string>();

    while (stream)
    {
        if (!std::getline(stream, key, '='))
        {
            continue;
        }

        if (!std::getline(stream, value, ' '))
        {
            continue;
        }

        pairs.insert(std::pair<std::string, std::string>(key, value));
    }

    // Check if all required keys are provided
    auto valid = true;
    for (const auto mandatoryKey : mandatoryKeys)
    {
        valid |= (pairs.find(mandatoryKey) != pairs.cend());
    }

    if (!valid)
    {
        return std::map<std::string, std::string>();
    }

    return pairs;
}


} // namespace openll
