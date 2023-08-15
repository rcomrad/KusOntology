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
    Web(const std::string& aFileName) noexcept;
    HOLY_TRINITY_SINGLE(Web);

    static std::map<std::string, Node> processFile(
        const std::string& aFileName) noexcept;

    void print() const noexcept;

private:
    std::map<std::string, Node> mWeb;

    std::unordered_set<std::string> mVariables;

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
    std::unordered_set<std::string> containerHandler(
        const std::string& aCommand, const std::string& aArgs) noexcept;

    std::unordered_set<std::string> process(const std::string& aStr) noexcept;
    std::unordered_set<std::string> process(const char*& aStr) noexcept;
    static std::unordered_map<std::string, decltype(&core::Web::typeHandler)>
    getRouter() noexcept;

    static std::string getName(decltype(mVariables.begin()) aPtr) noexcept;
    static std::string getName(
        std::unordered_map<std::string, std::string>::const_iterator
            aPtr) noexcept;

    template <typename T>
    bool usePart(const T& aCont,
                 const std::string& aBlockName,
                 const std::string& aType,
                 const std::string& aName) noexcept
    {
        bool result = false;
        auto it     = aCont.find(aName);
        if (it != aCont.end())
        {
            createEdge(aBlockName, getName(it), aType);
            result = true;
        }
        return result;
    }

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

    int mCicleNumber;
    int mIfNumber;
    int mBlockNumber;
    int mExpressionNumber;
};
} // namespace core

#endif // !WEB_HPP
