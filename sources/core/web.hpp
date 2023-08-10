#ifndef WEB_HPP
#define WEB_HPP

#include <set>
#include <unordered_set>

#include "domain/holy_trinity.hpp"
#include "domain/metaprogramming.hpp"

#include "node.hpp"

namespace core
{
class Web
{
public:
    Web() noexcept;
    HOLY_TRINITY_SINGLE(Web);

    void create() noexcept;

    void print() const noexcept;

private:
    std::map<std::string, Node> mWeb;

    std::unordered_set<std::string> mVariables;

    std::unordered_set<std::string> process(const std::string& aStr) noexcept;
    std::unordered_set<std::string> process(const char*& aStr) noexcept;

    std::unordered_set<std::string> typeHandler(
        const std::string& aCommand, const std::string& aArgs) noexcept;
    std::unordered_set<std::string> declarationHandler(
        const std::string& aCommand, const std::string& aArgs) noexcept;
    std::unordered_set<std::string> funcHandler(
        const std::string& aCommand, const std::string& aArgs) noexcept;
    std::unordered_set<std::string> cicleHandler(
        const std::string& aCommand, const std::string& aArgs) noexcept;
    std::unordered_set<std::string> conditionHandler(
        const std::string& aCommand, const std::string& aArgs) noexcept;
    std::unordered_set<std::string> expressionHandler(
        const std::string& aCommand, const std::string& aArgs) noexcept;

    static std::unordered_map<std::string, std::string>
    makeRelationMap() noexcept;

    void createEdge(const std::string& aFrom,
                    const std::string& aTo,
                    const std::string& aFromRelation,
                    Node::Type aFromType = Node::Type::Nun) noexcept;

    void typeAutomaticDetection(decltype(*mWeb.begin())& aNode) noexcept;

    static void skipDefault(const char*& c) noexcept;
    static void skipAssaigment(const char*& c) noexcept;
    static void skipCicle(const char*& c) noexcept;

    static bool check(const char*& aStr) noexcept;

    static std::string getInsides(const std::string& aStr) noexcept;
};
} // namespace core

#endif // !WEB_HPP
