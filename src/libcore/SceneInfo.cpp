/*
 * Copyright (c) 2019 Jonas Deyson
 *
 * This software is released under the MIT License.
 *
 * You should have received a copy of the MIT License
 * along with this program. If not, see <https://opensource.org/licenses/MIT>
 */

#include "fbksd/core/SceneInfo.h"
#include <iostream>
using namespace fbksd;


#define SceneInfo_NEW_TYPE(type, map) \
    template<> \
    void SceneInfo::set(const std::string& name, const type& value) \
    { map[name] = value; } \
    template<> \
    type SceneInfo::get(const std::string &name) const { \
        auto item = map.find(name); \
        if(item != map.end()){ \
            return item->second; \
        } \
        return type(); \
    } \
    template<> \
    bool SceneInfo::has<type>(const std::string &name) const { \
        auto item = map.find(name); \
        return item != map.end(); \
    }

SceneInfo_NEW_TYPE(float, floatMap)
SceneInfo_NEW_TYPE(int64_t, int64Map)
SceneInfo_NEW_TYPE(bool, boolMap)
SceneInfo_NEW_TYPE(std::string, stringMap)


// This is a overload for the int64Map accepting a int
template<>
void SceneInfo::set(const std::string &name, const int& value)
{
    int64Map[name] = value;
}

SceneInfo SceneInfo::merged(const SceneInfo& scene) const
{
    SceneInfo result = *this;

    for(auto& item : scene.int64Map)
        result.int64Map[item.first] = item.second;
    for(auto& item : scene.floatMap)
        result.floatMap[item.first] = item.second;
    for(auto& item : scene.boolMap)
        result.boolMap[item.first] = item.second;
    for(auto& item : scene.stringMap)
        result.stringMap[item.first] = item.second;

    return result;
}
