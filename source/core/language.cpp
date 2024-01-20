#include "language.hpp"

#include "domain/metaprogramming.hpp"

#include "file_data/file.hpp"
#include "file_data/path.hpp"

std::unordered_map<std::string, core::Node::Type>
core::Language::getNodeList() noexcept
{
    std::unordered_map<std::string, Node::Type> result;

    loadNamesFromFile(result, Node::Type::Concept, "concept.txt");
    loadNamesFromFile(result, Node::Type::Container, "containers.txt");
    loadNamesFromFile(result, Node::Type::Type, "type.txt");
    loadNamesFromFile(result, Node::Type::Method, "method2.txt");
    addNamesToList(result, Node::Type::Cicle, {"for", "while"});
    addNamesToList(result, Node::Type::Condition, {"if"s, "else if"s, "else"s});
    addNamesToList(result, Node::Type::Expression, {"expression"});

    // result["int"]       = Node::Type::Int;
    // result["long long"] = Node::Type::LL;
    // result["float"]     = Node::Type::Float;
    // result["double"]    = Node::Type::Double;
    // result["char"]      = Node::Type::Char;
    // result["bool"]      = Node::Type::Bool;

    return result;
}

std::unordered_map<std::string, std::string>
core::Language::makeRelationMap() noexcept
{
    std::unordered_map<std::string, std::string> result =
        file::File::getWordsMap(
            file::Path::getPathUnsafe("language", "relation.txt"));

    std::unordered_map<std::string, std::string> temp;
    for (auto& i : result)
    {
        temp.insert({i.second, i.first});
    }
    result.insert(temp.begin(), temp.end());

    return result;
}

std::unordered_map<std::string, long long>
core::Language::getNameToNum() noexcept
{
    std::unordered_map<std::string, long long> result;

    auto primes = getPrime();

    for (int i = int(Node::Type::Nun) + 1; i < int(Node::Type::Last); ++i)
    {
        result[std::to_string(i)] = primes.back();
        primes.pop_back();
    }

    auto relations = file::File::getWords(
        file::Path::getPathUnsafe("language", "relation.txt"));
    for (auto& i : relations)
    {
        for (auto& j : i)
        {
            result[j] = primes.back();
            primes.pop_back();
        }
    }

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

void
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

std::vector<long long>
core::Language::getPrime() noexcept
{
    std::vector<long long> result;

    std::vector<char> pr(1e7, true);
    for (long long i = 2; i < long long(pr.size()); ++i)
    {
        if (pr[i])
        {
            for (long long j = i * i; j < long long(pr.size()); j += i)
            {
                pr[j] = false;
            }
            result.push_back(i);
        }
    }

    std::reverse(result.begin(), result.end());

    return result;
}
