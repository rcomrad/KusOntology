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
        Variable,
        Type,
        Concept,
        Function,
        Cicle,
        Condition,
        ConditionBlock,
        Expression,
        FunctionCall
    };

    Type mType;
    std::unordered_map<std::string, std::set<std::string>> mLeaves;
};
} // namespace core

#endif // !NODE_HPP
