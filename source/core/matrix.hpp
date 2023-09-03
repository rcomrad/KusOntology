#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <string>
#include <unordered_map>

#include "domain/pair.hpp"

namespace core
{
class Matrix
{
public:
    void add_node(long long aNum, std::vector<long long>&& aValues) noexcept;
    // void pushForLastNode(long long aNum) noexcept;
    void collapse() noexcept;

    float intersect(const Matrix& aOther,
                    bool aWithoutConcept  = true,
                    bool aNotFullEquality = true) const noexcept;

    void print(const std::string& aName) const noexcept;

private:
    std::vector<long long> mNodes;
    std::vector<std::vector<long long>> mTable;
    std::unordered_map<long long,
                       std::vector<std::vector<dom::Pair<long long>>>>
        mAttributes;

    static float compare(
        const std::vector<std::vector<dom::Pair<long long>>>& aFirst,
        const std::vector<std::vector<dom::Pair<long long>>>& aSecond,
        bool aNotFullEquality = true) noexcept;

    static float distance(
        const std::vector<dom::Pair<long long>>& aFrom,
        const std::vector<dom::Pair<long long>>& aTo) noexcept;
};
} // namespace core

#endif // !VERTIX_HPP
