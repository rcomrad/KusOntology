#include "matrix.hpp"

#include <set>

#include "domain/log.hpp"

#include "file_data/file.hpp"

#include "node.hpp"

void
core::Matrix::add_node(long long aNum,
                       std::vector<long long>&& aValues) noexcept
{
    mNodes.emplace_back(aNum);
    mTable.emplace_back(std::move(aValues));
}

// void
// core::Matrix::pushForLastNode(long long aNum) noexcept
// {
//     mTable.back().emplace_back(aNum);
// }

void
core::Matrix::collapse() noexcept
{
    for (int i = 0; i < mTable.size(); ++i)
    {
        auto& temp = mAttributes[mNodes[i]].emplace_back();
        for (int j = 0; j < mTable[i].size(); ++j)
        {
            if (mTable[i][j])
            {
                temp.emplace_back(
                    dom::Pair<long long>{mNodes[j], mTable[i][j]});
            }
        }
    }
}

float
core::Matrix::intersect(const Matrix& aOther,
                        bool aWithoutConcept,
                        bool aNotFullEquality) const noexcept
{
    float result = 0;
    std::set<long long> allNodes;
    allNodes.insert(mNodes.begin(), mNodes.end());
    allNodes.insert(aOther.mNodes.begin(), aOther.mNodes.end());

    if (aWithoutConcept)
    {
        // std::set<long long> erase = {2};
        allNodes.erase(2);
    }

    for (auto& cur : allNodes)
    {
        auto it1 = mAttributes.find(cur);
        auto it2 = aOther.mAttributes.find(cur);

        if (it1 == mAttributes.end()) continue;
        if (it2 == aOther.mAttributes.end()) continue;

        result += compare(it1->second, it2->second, aNotFullEquality);
    }

    return result / allNodes.size();
}

void
core::Matrix::print(const std::string& aName) const noexcept
{
    std::string data;
    for (int i = 0; i < mTable.size(); ++i)
    {
        for (auto& j : mTable[i])
        {
            data += std::to_string(j) + " ";
        }
        data += " = " + std::to_string(mNodes[i]) + "\n";
    }
    file::File::writeData("output", aName + "_matrix", data);

    data.clear();
    for (auto& i : mAttributes)
    {
        for (auto& j : i.second)
        {
            for (auto& k : j)
            {
                data += std::to_string(k.first) + ":" +
                        std::to_string(k.second) + " ";
            }
            data += "\n";
        }
    }
    file::File::writeData("output", aName + "_char", data);
}

float
core::Matrix::compare(
    const std::vector<std::vector<dom::Pair<long long>>>& aFirst,
    const std::vector<std::vector<dom::Pair<long long>>>& aSecond,
    bool aNotFullEquality) noexcept
{
    float result = 0;

    std::vector<bool> usedFirst(aFirst.size(), false);
    std::vector<bool> usedSecond(aSecond.size(), false);

    for (int i = 0; i < usedFirst.size(); ++i)
    {
        if (usedFirst[i]) continue;
        for (int j = 0; j < usedSecond.size(); ++j)
        {
            if (usedSecond[j]) continue;
            if (aFirst[i] == aSecond[j])
            {
                usedFirst[i] = usedSecond[j] = true;
                result += 1;
                break;
            }
        }
    }

    if (aNotFullEquality)
    {
        for (int i = 0; i < usedFirst.size(); ++i)
        {
            if (usedFirst[i]) continue;

            int num   = -1;
            float val = 0;

            for (int j = 0; j < usedSecond.size(); ++j)
            {
                if (usedSecond[j]) continue;
                float temp = distance(aFirst[i], aSecond[j]);
                if (temp > val)
                {
                    val = temp;
                    num = j;
                }
            }

            if (num > -1)
            {
                usedFirst[i] = usedSecond[num] = true;
                result += val;
            }
        }
    }

    result /= (aFirst.size() + aSecond.size()) / 2.;

    // dom::writeInfo(result);

    return result;
}

float
core::Matrix::distance(const std::vector<dom::Pair<long long>>& aFrom,
                       const std::vector<dom::Pair<long long>>& aTo) noexcept
{
    float result = 0;

    std::vector<bool> usedFrom(aFrom.size(), false);
    std::vector<bool> usedTo(aTo.size(), false);

    for (int i = 0; i < aFrom.size(); ++i)
    {
        if (usedFrom[i]) continue;
        for (int j = 0; j < aTo.size(); ++j)
        {
            if (usedTo[j]) continue;
            if (aFrom[i] == aTo[j])
            {
                usedFrom[i] = usedTo[j] = true;
                result += 2;
                break;
            }
        }
    }

    for (int i = 0; i < aFrom.size(); ++i)
    {
        if (usedFrom[i]) continue;
        for (int j = 0; j < aTo.size(); ++j)
        {
            if (usedTo[j]) continue;
            if (aFrom[i].x == aTo[j].x || aFrom[i].y == aTo[j].y)
            {
                usedFrom[i] = usedTo[j] = true;
                result += 1;
            }
        }
    }

    result /= aFrom.size() + aTo.size();
    // dom::writeInfo(result);
    return result;
}
