
#pragma once


#include <functional>


namespace openll
{


/**
*  @brief
*    Alignment enum class for glyphs in an annotation space.
*/
enum class Alignment : unsigned char
{
    LeftAligned, ///< Left align glyphs
    Centered,    ///< Center align glyphs
    RightAligned ///< Right align glyphs
};


} // namespace openll


namespace std
{


/**
*  @brief
*    Hash specialization for Alignment enum class.
*
*    Enables the use of Alignment as key type of the unordered collection types.
*/
template<>
struct hash<openll::Alignment>
{
    std::hash<unsigned char>::result_type operator()(
        const openll::Alignment & arg) const
    {
        std::hash<unsigned char> hasher;
        return hasher(static_cast<unsigned char>(arg));
    }
};


} // namespace std
