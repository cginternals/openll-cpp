
#pragma once


#include <string>

#include <openll/openll_api.h>


namespace openll
{


/**
*  @brief
*    Text buffer
*
*    Represents a text buffer which can be shared between glyph sequences
*    to display the same text at different locations or with different styles
*/
class OPENLL_API Text
{
public:
    /**
    *  @brief
    *    Get default line feed character
    *
    *  @return
    *    Character that marks the end of a line
    */
    static char32_t defaultLineFeed();


public:
    /**
    *  @brief
    *    Constructor
    */
    Text();

    /**
    *  @brief
    *    Destructor
    */
    virtual ~Text();

    /**
    *  @brief
    *    Get text (32 bit unicode string)
    *
    *  @return
    *    Text (32 bit unicode string)
    */
    const std::u32string & text() const;

    /**
    *  @brief
    *    Set text (32 bit unicode string)
    *
    *  @param[in] text
    *    Text (32 bit unicode string)
    */
    void setText(const std::u32string & text);

    /**
    *  @brief
    *    Get linefeed character
    *
    *  @return
    *    Character that marks the end of a line
    */
    char32_t lineFeed() const;

    /**
    *  @brief
    *    Set linefeed character
    *
    *  @param[in] linefeed
    *    Character that marks the end of a line
    */
    void setLineFeed(char32_t linefeed);


protected:
    std::u32string m_text;     ///< Text that is rendered
    char32_t       m_linefeed; ///< Character that marks the end of a line
};


} // namespace openll
