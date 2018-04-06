
#include <openll/openll.h>

#include <cpplocate/cpplocate.h>


namespace
{

std::string determineDataPath()
{
    std::string path = cpplocate::locatePath("data/openll", "share/openll", reinterpret_cast<void *>(&openll::dataPath));
    if (path.empty()) path = "./data";
    else              path = path + "/data";

    return path;
}

} // namespace


namespace openll
{


const std::string & dataPath()
{
    static const auto path = determineDataPath();

    return path;
}


} // namespace openll
