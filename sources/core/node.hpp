#ifndef NODE_HPP
#define NODE_HPP

#include <memory>
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
        Expression,
        FunctionCall
    };

    Type mType;
    std::unordered_map<std::string, std::string> mLeaves;
};
} // namespace core

#endif // !NODE_HPP
