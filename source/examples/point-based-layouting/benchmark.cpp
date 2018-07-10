#include "benchmark.h"

#include <algorithm>
#include <map>

#include <openll/layout/LabelArea.h>
#include <openll/Typesetter.h>

namespace
{

std::vector<openll::LabelArea> computeLabelAreas(const std::vector<openll::Label> & labels)
{
    std::vector<openll::LabelArea> areas;
    for (const auto& label : labels)
    {
        auto origin = label.pointLocation + label.placement.offset;
        auto extent = openll::Typesetter::extent(label.sequence);
        const auto position = label.placement.display ?
            openll::relativeLabelPosition(label.placement.offset, extent) :
            openll::RelativeLabelPosition::Hidden;
        areas.push_back({origin, extent, position});
    }
    return areas;
}

}

int labelOverlaps(const std::vector<openll::Label> & labels, const glm::vec2 & relativePadding)
{
    auto areas = computeLabelAreas(labels);
    int counter = 0;
    for (size_t i = 0; i < areas.size(); ++i) {
        for (size_t j = i + 1; j < areas.size(); ++j) {
            if (areas[i].paddedOverlaps(areas[j], relativePadding))
            {
                ++counter;
            }
        }
    }
    return counter;
}

float labelOverlapArea(const std::vector<openll::Label> & labels, const glm::vec2 & relativePadding)
{
    auto areas = computeLabelAreas(labels);
    float area = 0;
    for (size_t i = 0; i < areas.size(); ++i) {
        for (size_t j = i + 1; j < areas.size(); ++j) {
            area += areas[i].paddedOverlapArea(areas[j], relativePadding);
        }
    }
    return area;
}

int labelsHidden(const std::vector<openll::Label> & labels)
{
    return std::count_if(labels.begin(), labels.end(), [](const openll::Label& label)
    {
        return !label.placement.display;
    });
}

std::map<openll::RelativeLabelPosition, unsigned int> labelPositionDesirability(const std::vector<openll::Label> & labels)
{
    std::map<openll::RelativeLabelPosition, unsigned int> result
    {
        {openll::RelativeLabelPosition::UpperRight, 0},
        {openll::RelativeLabelPosition::UpperLeft, 0},
        {openll::RelativeLabelPosition::LowerLeft, 0},
        {openll::RelativeLabelPosition::LowerRight, 0}
    };
    for (const auto & label : labels)
    {
        if (!label.placement.display) continue;
        const auto extent = openll::Typesetter::extent(label.sequence);
        const auto position = openll::relativeLabelPosition(label.placement.offset, extent);
        ++result[position];
    }
    return result;
}
