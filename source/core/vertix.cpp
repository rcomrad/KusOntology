#include "vertix.hpp"

#include <cmath>

core::Vertix::Vertix() noexcept : mValue(1), mSize(0)
{
}

core::Vertix::operator long long() const noexcept
{
    return mValue;
}

float
core::Vertix::intersection(const Vertix& aOther) const noexcept
{
    float result = float(getSameCount(mNums, aOther.mNums) +
                         getSameCount(aOther.mNums, mNums)) /
                   (float(mSize + aOther.mSize) / 2.0);

    return result;
}

void
core::Vertix::add(long long aNumber) noexcept
{
    mValue *= aNumber;
    ++mNums[aNumber];
    ++mSize;
}

int
core::Vertix::getSameCount(
    const std::unordered_map<long long, int>& a,
    const std::unordered_map<long long, int>& b) const noexcept
{
    int result = 0;
    for (const auto& i : a)
    {
        auto it = b.find(i.first);
        if (it != b.end())
        {
            result += std::min(i.second, it->second);
        }
    }
    return result;
}
