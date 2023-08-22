#ifndef LANGUAGE_HPP
#define LANGUAGE_HPP

#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "node.hpp"

namespace core
{
class Language
{
public:
    static std::unordered_map<std::string, Node::Type> getNodeList() noexcept;
    static std::unordered_map<std::string, std::string>
    makeRelationMap() noexcept;
    static std::unordered_map<std::string, long long> getNameToNum() noexcept;

private:
    static void loadNamesFromFile(
        std::unordered_map<std::string, core::Node::Type>& aArray,
        core::Node::Type aType,
        const std::string& aFileName) noexcept;

    static void addNamesToList(
        std::unordered_map<std::string, core::Node::Type>& aArray,
        core::Node::Type aType,
        std::vector<std::string>&& aNames) noexcept;

    static std::vector<long long> getPrime() noexcept;
};
} // namespace core

#endif // !LANGUAGE_HPP
