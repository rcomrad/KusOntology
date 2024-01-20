#ifndef NODE_HPP
#define NODE_HPP

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

namespace core
{
class Node
{
public:
    enum class Type
    {
        Nun,
        Concept,
        Function,
        Method,
        Type,
        Int,
        LL,
        Double,
        Float,
        Char,
        Bool,
        Container,        
        Variable,
        ContainerVariable,
        Condition,
        Cicle,
        Expression,
        FunctionCall,
        Last
    };

    Type mType;
    std::unordered_map<std::string, std::set<std::string>> mLeaves;

    bool operator==(const Node& aOther) const noexcept;
};
} // namespace core

#endif // !NODE_HPP
