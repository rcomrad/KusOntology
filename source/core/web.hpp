#ifndef WEB_HPP
#define WEB_HPP

#include <set>
#include <unordered_set>

#include "domain/holy_trinity.hpp"
#include "domain/metaprogramming.hpp"

#include "matrix.hpp"
#include "node.hpp"

namespace core
{
class Web
{
public:
    Web() noexcept;
    Web(const std::string& aName) noexcept;
    HOLY_TRINITY_NOCOPY(Web);

    std::string getName() const noexcept;

    void print() const noexcept;

    void makeMatrix() noexcept;

    float compare(const Web& other,
                  bool aWithoutConcept  = true,
                  bool aNotFullEquality = true) const noexcept;

    void addNode(const std::string& aName, Node::Type aType) noexcept;
    void createEdge(const std::string& aFrom,
                    const std::string& aTo,
                    const std::string& aFromRelation,
                    Node::Type aFromType = Node::Type::Nun) noexcept;

private:
    std::string mName;
    // std::string mExprInfo;

    std::map<std::string, Node> mWeb;
    Matrix mMarix;

    void typeAutomaticDetection(decltype(*mWeb.begin())& aNode) noexcept;
};
} // namespace core

#endif // !WEB_HPP
