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
    Web() noexcept {};
    Web(const std::string& aName) noexcept;
    HOLY_TRINITY_NOCOPY(Web);

    void print() const noexcept;

    void makeMatrix() noexcept;

    float compare(const Web& other) const noexcept;

    void addNode(const std::string& aName, Node::Type aType) noexcept;
    void createEdge(const std::string& aFrom,
                    const std::string& aTo,
                    const std::string& aFromRelation,
                    Node::Type aFromType = Node::Type::Nun) noexcept;

private:
    std::string mName;

    std::map<std::string, Node> mWeb;

    static std::unordered_map<std::string, decltype(&core::Web::typeHandler)>
    getRouter() noexcept;

    static std::unordered_map<std::string, Node::Type> getNodeList() noexcept;

    static std::unordered_map<std::string, int> getCodes() noexcept;

    std::vector<long long> recurrentSearch(
        std::vector<std::vector<int>> aTable,
        std::vector<long long> attributes) noexcept;

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

    void typeAutomaticDetection(decltype(*mWeb.begin())& aNode) noexcept;

    static bool check(const char*& aStr) noexcept;

    static std::string getInsides(const std::string& aStr) noexcept;

    int mCicleNumber;
    int mIfNumber;
    int mBlockNumber;
    int mExpressionNumber;

    std::string mExprInfo;

    std::unordered_map<int, std::vector<std::vector<long long>>> mMarix;
};
} // namespace core

#endif // !WEB_HPP
