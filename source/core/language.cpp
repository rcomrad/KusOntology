#include "language.hpp"

#include "file_data/file.hpp"
#include "file_data/path.hpp"

std::unordered_map<std::string, core::Node::Type>
core::Language::getNodeList() noexcept
{
    std::unordered_map<std::string, Node::Type> result;

    loadNamesFromFile(result, Node::Type::Concept, "concept.txt");
    loadNamesFromFile(result, Node::Type::Type, "containers.txt");
    loadNamesFromFile(result, Node::Type::Type, "type.txt");
    loadNamesFromFile(result, Node::Type::Cicle, {"for", "while"});
    loadNamesFromFile(result, Node::Type::Condition, {"if", "else if", "else"});
    loadNamesFromFile(result, Node::Type::Expression, {"expression"});

    return result;
}

void
core::Language::loadNamesFromFile(
    std::unordered_map<std::string, core::Node::Type>& aArray,
    core::Node::Type aType,
    const std::string& aFileName) noexcept
{
    auto names =
        file::File::getLines(file::Path::getPathUnsafe("language", aFileName));
    addNamesToList(aArray, aType, std::move(names));
}

std::unordered_map<std::string, core::Node::Type>
core::Language::addNamesToList(
    std::unordered_map<std::string, core::Node::Type>& aArray,
    core::Node::Type aType,
    std::vector<std::string>&& aNames) noexcept
{
    for (auto&& i : aNames)
    {
        aArray.insert({std::move(i), aType});
    }
}
