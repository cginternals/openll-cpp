
#pragma once


#include <iosfwd>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>

#include <openll/openll_api.h>


namespace openll
{


class FontFace;


/**
* @brief
*   The FontLoader provides interfaces to load font face descriptions from files.
*
*   It can be registered at a ResourceManager as a generic loader for font faces.
*/
class OPENLL_API FontLoader
{
public:
    /**
    *  @brief
    *    Key-Value pairs of the description file
    */
    using StringPairs = std::map<std::string, std::string>;


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
    *    Check if this loader can load files with given extension.
    *
    *  @param[in] ext
    *    The extension to check
    *
    *  @return
    *    'true' if this loader can handle files of given extension, else 'false'
    */
    bool canLoad(const std::string & ext) const;

    /**
    *  @brief
    *    Get file dialog filter for all loadable file types.
    *
    *  @return
    *    File dialog filter for all loadable file types.
    */
    std::vector<std::string> loadingTypes() const;

    /**
    *  @brief
    *    Get file dialog filter for all loadable file types as one string.
    *
    *  @return
    *    File dialog filter for all loadable file types.
    */
    std::string allLoadingTypes() const;

    /**
    *  @brief
    *    Load a font face description file and create a new FontFace.
    *
    *  @param[in] filename
    *    The path to the font face description file
    *  @param[in] progress
    *    A callback to propagate progress.
    *    The int parameters of the callback denote current and
    *    maximum required steps for loading.
    *
    *  @return
    *    A configured and initialized FontFace on success, else 'nullptr'
    */
    FontFace * load(const std::string & filename, std::function<void(int, int)> progress  = nullptr) const;


protected:
    /**
    *  @brief
    *    Handle common info block of font face description file
    *
    *  @param[in]    stream
    *    The stream to read info from
    *  @param[inout] fontFace
    *    The font face to construct
    *  @param[out]   fontSize
    *    The retrieved font size of the font face
    */
    void handleInfo(std::stringstream & stream, FontFace & fontFace, float & fontSize) const;

    /**
    *  @brief
    *    Handle common block of font face description file
    *
    *  @param[in]    stream
    *    The stream to read info from
    *  @param[inout] fontFace
    *    The font face to construct
    *  @param[in]   fontSize
    *    The font size to correctly determine other metrics
    */
    void handleCommon(std::stringstream & stream, FontFace & fontFace, float fontSize) const;

    /**
    *  @brief
    *    Handle font face page block of font face description file
    *
    *  @param[in]    stream
    *    The stream to read info from
    *  @param[inout] fontFace
    *    The font face to construct
    *  @param[out]   filename
    *    The file name of the description file to derivate glyph texture atlas file paths
    */
    void handlePage(std::stringstream & stream, FontFace & fontFace, const std::string & filename) const;

    /**
    *  @brief
    *    Handle font face character block of font face description file
    *
    *  @param[in]    stream
    *    The stream to read info from
    *  @param[inout] fontFace
    *    The font face to construct
    */
    void handleChar(std::stringstream & stream, FontFace & fontFace) const;

    /**
    *  @brief
    *    Handle font face kerning block of font face description file
    *
    *  @param[in]    stream
    *    The stream to read info from
    *  @param[inout] fontFace
    *    The font face to construct
    */
    void handleKerning(std::stringstream & stream, FontFace & fontFace) const;

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
    static StringPairs readKeyValuePairs(std::stringstream & stream, const std::initializer_list<const char *> & mandatoryKeys);
};


} // namespace openll
