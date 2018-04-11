
#include <openll/Text.h>


namespace openll
{


char32_t Text::defaultLineFeed()
{
    static const auto LF = static_cast<char32_t>('\x0A');
    return LF;
}

Text::Text()
: m_linefeed(Text::defaultLineFeed())
{
}

Text::~Text()
{
}

const std::u32string & Text::text() const
{
    return m_text;
}

void Text::setText(const std::u32string & text)
{
    m_text = text;
}

char32_t Text::lineFeed() const
{
    return m_linefeed;
}

void Text::setLineFeed(char32_t linefeed)
{
    m_linefeed = linefeed;
}


} // namespace openll
