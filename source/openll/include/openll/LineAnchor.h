
#pragma once


#include <functional>


namespace openll
{


enum class LineAnchor : unsigned char
{
    Ascent, Center, Baseline, Descent
};


} // namespace openll


namespace std
{


template<>
struct hash<openll::LineAnchor>
{
    std::hash<unsigned char>::result_type operator() (
        const openll::LineAnchor & arg) const
    {
        std::hash<unsigned char> hasher;
        return hasher(static_cast<unsigned char>(arg));
    }
};


} // namespace std
