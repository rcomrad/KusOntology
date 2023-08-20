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

private:
    static void loadNamesFromFile(
        std::unordered_map<std::string, core::Node::Type>& aArray,
        core::Node::Type aType,
        const std::string& aFileName) noexcept;

    static std::unordered_map<std::string, core::Node::Type> addNamesToList(
        std::unordered_map<std::string, core::Node::Type>& aArray,
        core::Node::Type aType,
        std::vector<std::string>&& aNames) noexcept;
};
} // namespace core

#endif // !LANGUAGE_HPP
