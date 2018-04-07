
#pragma once


#include <functional>


namespace openll
{


/**
*  @brief
*    Horizontal text alignment for glyphs in an annotation space
*/
enum class Alignment : unsigned char
{
    LeftAligned, ///< Left align text at the origin
    Centered,    ///< Center align text at the origin
    RightAligned ///< Right align text at the origin
};


} // namespace openll


namespace std
{


/**
*  @brief
*    Hash specialization for Alignment enum
*
*    Enables the use of Alignment as a key type of the unordered collection types.
*/
template<>
struct hash<openll::Alignment>
{
    std::hash<unsigned char>::result_type operator()(const openll::Alignment & arg) const
    {
        std::hash<unsigned char> hasher;
        return hasher(static_cast<unsigned char>(arg));
    }
};


} // namespace std
