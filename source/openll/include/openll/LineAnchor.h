
#pragma once


#include <functional>


namespace openll
{


/**
*  @brief
*    Vertical anchor point for text
*/
enum class LineAnchor : unsigned char
{
    Ascent,   ///< Anchor text to the beginning (top) of the glyph
    Center,   ///< Anchor text to the center of the glyph
    Baseline, ///< Anchor text to the baseline of the glyph
    Descent   ///< Anchor text to the end (bottom) of the glyph
};


} // namespace openll


namespace std
{


/**
*  @brief
*    Hash specialization for LineAnchor enum
*
*    Enables the use of LineAnchor as a key type of the unordered collection types.
*/
template<>
struct hash<openll::LineAnchor>
{
    std::hash<unsigned char>::result_type operator() (const openll::LineAnchor & arg) const
    {
        std::hash<unsigned char> hasher;
        return hasher(static_cast<unsigned char>(arg));
    }
};


} // namespace std
