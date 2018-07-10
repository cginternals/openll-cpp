
#include <string>
#include <algorithm>

#include <cpplocate/cpplocate.h>


namespace common
{

    std::string retrieveDataPath(const std::string & module, const std::string & key)
    {
        const auto moduleInfo = cpplocate::findModule(module);

        auto dataPath = moduleInfo.value(key);
        dataPath = normalizePath(dataPath);

        if (dataPath.empty())
            dataPath = "data/";
        else
            dataPath += "/";

        return dataPath;
    }

}
