#ifndef WEB_HPP
#define WEB_HPP

#include <set>

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
    // std::set<std::string> mTypes;
    std::unordered_map<std::string, Node> mWeb;

    // std::vector<std::string> process(const std::string& aStr) noexcept;
    std::vector<std::string> process(const char*& aStr) noexcept;

    std::vector<std::string> typeHandler(const std::string& aCommand,
                                         const std::string& aArgs) noexcept;
    std::vector<std::string> declarationHandler(
        const std::string& aCommand, const std::string& aArgs) noexcept;
    std::vector<std::string> funcHandler(const std::string& aCommand,
                                         const std::string& aArgs) noexcept;
    std::vector<std::string> cicleHandler(const std::string& aCommand,
                                          const std::string& aArgs) noexcept;
    void createNode(const std::string& aName,
                    Node::Type aType,
                    const std::pair<std::string, std::string>& aLeave) noexcept;

    void createTree(
        const std::string& aName,
        Node::Type aType,
        const std::unordered_map<std::string, std::string>& aLeave) noexcept;

    static void skipDefault(const char*& c) noexcept;
    static void skipAssaigment(const char*& c) noexcept;
    static void skipCicle(const char*& c) noexcept;
};
} // namespace core

#endif // !WEB_HPP
