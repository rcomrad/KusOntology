#include "web.hpp"

#include <algorithm>
#include <iostream>

#include "domain/log.hpp"

#include "file_data/file.hpp"
#include "file_data/parser.hpp"
#include "file_data/path.hpp"
#include "file_data/variable_storage.hpp"

#include "language.hpp"

core::Web::Web() noexcept : Web("NUN")
{
}

core::Web::Web(const std::string& aName) noexcept : mName(aName)
{
}

void
core::Web::addNode(const std::string& aName, Node::Type aType) noexcept
{
    mWeb[aName].mType = aType;
}

std::string
core::Web::getName() const noexcept
{
    return mName;
}

void
core::Web::createEdge(const std::string& aFrom,
                      const std::string& aTo,
                      const std::string& aFromRelation,
                      Node::Type aFromType) noexcept
{
    static std::unordered_map<std::string, std::string> relationSwitch =
        Language::makeRelationMap();

    mWeb[aFrom];
    mWeb[aTo];

    auto& from = *mWeb.find(aFrom);
    auto& to   = *mWeb.find(aTo);

    from.second.mLeaves[aFromRelation].insert(aTo);
    to.second.mLeaves[relationSwitch[aFromRelation]].insert(aFrom);

    if (from.second.mType == Node::Type::Nun) from.second.mType = aFromType;

    typeAutomaticDetection(from);
    typeAutomaticDetection(to);
}

void
core::Web::makeMatrix() noexcept
{
    static auto codes = Language::getNameToNum();

    std::unordered_map<std::string, int> nums;
    for (auto& i : mWeb)
    {
        nums[i.first] = nums.size();
    }

    for (auto& i : mWeb)
    {
        // mMarix.add_node(codes[std::to_string(int(i.second.mType))]);
        std::vector<long long> temp(nums.size());
        for (auto& j : i.second.mLeaves)
        {
            for (auto& k : j.second)
            {
                temp[nums[k]] = codes[j.first];
            }
        }
        mMarix.add_node(codes[std::to_string(int(i.second.mType))],
                        std::move(temp));
    }

    mMarix.collapse();
}

float
core::Web::compare(const Web& other,
                   bool aWithoutConcept,
                   bool aNotFullEquality) const noexcept
{
    return mMarix.intersect(other.mMarix, aWithoutConcept, aNotFullEquality);
}

void
core::Web::print() const noexcept
{
    std::string data;

    int cnt = 0;
    for (int type = int(Node::Type::Nun); type < int(Node::Type::Last); ++type)
    {
        for (auto& i : mWeb)
        {
            if (i.second.mType != Node::Type(type)) continue;

            data += std::to_string(++cnt) + ") " + i.first + "\n";
            for (auto& j : i.second.mLeaves)
            {
                data += "\t" + j.first + "\n";
                for (auto& k : j.second)
                {
                    data += "\t\t" + k + "\n";
                }
            }
        }
    }

    file::File::writeData("output", mName + "_print", data);

    mMarix.print(mName);
}

void
core::Web::typeAutomaticDetection(decltype(*mWeb.begin())& aNode) noexcept
{
    if (aNode.second.mType != Node::Type::Nun) return;

    static auto dict = Language::getNodeList();
    auto it          = dict.find(aNode.first);
    if (it != dict.end())
    {
        aNode.second.mType = it->second;
    }
}

// bool
// core::Web::isContainer(const std::string& aName) const noexcept
// {
//     bool result = false;

//     auto it = mWeb.find(aName);
//     if (it != mWeb.end() && it->second.mType == Node::Type::ContainerVariable)
//     {
//         result = true;
//     }

//     return result;
// }

// std::string
// core::Web::getBase(const std::string& aName) const noexcept
// {
//     std::string result = "";

//     auto it1 = mWeb.find(aName);
//     if (it1 != mWeb.end())
//     {
//         auto it2 = it1->second.mLeaves.find("base");
//         if (it2 !=  it1->second.mLeaves.end())
//         {
//             result = it2->second;
//         }
//         else
//         {
//             result=aName;
//         }
//     }
//     else
//     {
//         //TODO
//     }

//     return result;
// }

// std::string
// core::Web::getContainer(const std::string& aName) const noexcept
// {
//     std::string result = "";

//     auto it1 = mWeb.find(aName);
//     if (it1 != mWeb.end())
//     {
//         auto it2 = it1->second.mLeaves.find("is");
//         if (it2 !=  it1->second.mLeaves.end())
//         {
//             if (containers.contains(*it2->second.begin()))
//             {
//                 result = *it2->second.begin();
//             }
//         }
//         else
//         {
//             //TODO
//         }
//     }
//     else
//     {
//         //TODO
//     }

//     return result;
// }

std::string
core::Web::getContainer(const std::string& aName) const noexcept
{
    std::string result = "";

    auto it = mWeb.find(aName);
    if (it != mWeb.end() && it->second.mType == Node::Type::ContainerVariable)
    {
        result = *it->second.mLeaves.find("is"s)->second.begin();
    }

    return result;
}
