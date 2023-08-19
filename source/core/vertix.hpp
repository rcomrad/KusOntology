#ifndef VERTIX_HPP
#define VERTIX_HPP

#include <unordered_map>

namespace core
{
class Vertix
{
public:
    Vertix() noexcept;
    operator long long() const noexcept;
    float intersection(const Vertix& aOther) const noexcept;
    void add(long long aNumber) noexcept;

private:
    long long mValue;
    std::unordered_map<long long, int> mNums;
    int mSize;

    int getSameCount(
        const std::unordered_map<long long, int>& a,
        const std::unordered_map<long long, int>& b) const noexcept;
};
} // namespace core

#endif // !VERTIX_HPP
