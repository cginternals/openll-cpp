
#pragma once


#include <memory>
#include <iosfwd>
#include <string>
#include <map>

#include <openll/openll_api.h>


namespace openll
{


class FontFace;


/**
*  @brief
*    Loader that can load font faces from description files (.fnt)
*/
class OPENLL_API FontLoader
{
public:
    /**
    *  @brief
    *    Constructor
    */
    FontLoader();

    /**
    *  @brief
    *    Destructor
    */
    ~FontLoader();

    /**
    *  @brief
    *    Load a font face from a font description file (.fnt)
    *
    *  @param[in] filename
    *    Path to the font face description file (.fnt)
    *
    *  @return
    *    A configured and initialized FontFace on success, else 'nullptr'
    */
    std::unique_ptr<FontFace> load(const std::string & filename) const;


protected:
    /**
    *  @brief
    *    Parse common info block of font face description file
    *
    *  @param[in] stream
    *    The stream to read info from
    *  @param[in,out] fontFace
    *    The font face to construct
    *  @param[out] fontSize
    *    The retrieved font size of the font face
    */
    void parseInfo(std::stringstream & stream, FontFace & fontFace, float & fontSize) const;

    /**
    *  @brief
    *    Parse common block of font face description file
    *
    *  @param[in] stream
    *    The stream to read info from
    *  @param[in,out] fontFace
    *    The font face to construct
    *  @param[in] fontSize
    *    The font size to correctly determine other metrics
    */
    void parseCommon(std::stringstream & stream, FontFace & fontFace, float fontSize) const;

    /**
    *  @brief
    *    Parse font face page block of font face description file
    *
    *  @param[in] stream
    *    The stream to read info from
    *  @param[in,out] fontFace
    *    The font face to construct
    *  @param[out] filename
    *    The file name of the description file to derivate glyph texture atlas file paths
    */
    void parsePage(std::stringstream & stream, FontFace & fontFace, const std::string & filename) const;

    /**
    *  @brief
    *    Parse font face character block of font face description file
    *
    *  @param[in] stream
    *    The stream to read info from
    *  @param[in,out] fontFace
    *    The font face to construct
    */
    void parseChar(std::stringstream & stream, FontFace & fontFace) const;

    /**
    *  @brief
    *    Parse font face kerning block of font face description file
    *
    *  @param[in] stream
    *    The stream to read info from
    *  @param[in,out] fontFace
    *    The font face to construct
    */
    void parseKerning(std::stringstream & stream, FontFace & fontFace) const;

    /**
    *  @brief
    *    Extract all key-value pairs of one line in the description file
    *
    *  @param[in] stream
    *    The stream to read from
    *  @param[in] mandatoryKeys
    *    The list of mandatory keys required for successful extraction
    *
    *  @return
    *    The list of all extracted key-value pairs, empty if not all mandatory keys exist
    */
    std::map<std::string, std::string> readKeyValuePairs(std::stringstream & stream, const std::initializer_list<const char *> & mandatoryKeys) const;
};


} // namespace openll
