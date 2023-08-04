#include "web.hpp"

#include <algorithm>

#include "file_data/file.hpp"
#include "file_data/parser.hpp"
#include "file_data/path.hpp"

core::Web::Web() noexcept
{
    create();
}

void
core::Web::create() noexcept
{
    auto code = file::File::getAllData(
        file::Path::getInstance().getPath("resources").value() + "source.cpp");
    // auto lines = file::Parser::slice(code, ";{}", &core::Web::skipDefault);

    auto tempPtr = code.c_str();
    process(tempPtr);
    // for (auto& line : lines)
    // {
    //     process(line);
    // }
}

void
core::Web::print() const noexcept
{
    std::string data;
    for (auto& i : mWeb)
    {
        data += i.first + "\n";
        for (auto& j : i.second.mLeaves)
        {
            data += "\t" + j.first + "\n";
            for (auto& k : j.second)
            {
                data += "\t\t" + k + "\n";
            }
        }
    }
    // data += "\t" + j.second + " " + j.first + "\n";

    file::File::writeData("resources", "out", data);
    ;
}

// std::vector<std::string>
// core::Web::process(const std::string& aStr) noexcept
// {
//     std::vector<std::string> result;

// static std::unordered_map<std::string, decltype(&core::Web::typeHandler)>
//     router = {
//         {"int",    &core::Web::typeHandler },
//         {"float",  &core::Web::typeHandler },
//         {"double", &core::Web::typeHandler },
//         {"for",    &core::Web::cicleHandler},
//         {"while",  &core::Web::cicleHandler},
// };

// int num = 0;
// while (std::isspace(aStr[num])) ++num;

// for (int i = 1; i < aStr.size() - num; ++i)
// {
//     auto name = aStr.substr(num, i);
//     auto it   = router.find(name);
//     if (it != router.end())
//     {
//         result = (this->*it->second)(name, aStr.substr(i + 1,
//         aStr.size())); break;
//     }
// }

//     return result;
// }

std::unordered_set<std::string>
core::Web::typeHandler(const std::string& aCommand,
                       const std::string& aArgs) noexcept
{
    std::unordered_set<std::string> result;

    if (aArgs.find('=') == std::string::npos &&
        aArgs.find('(') != std::string::npos)
    {
        result = funcHandler(aCommand, aArgs);
    }
    else
    {
        result = declarationHandler(aCommand, aArgs);
    }

    return result;
}

std::unordered_set<std::string>
core::Web::declarationHandler(const std::string& aCommand,
                              const std::string& aArgs) noexcept
{
    std::unordered_set<std::string> result;

    auto parts = file::Parser::slice(aArgs, ",", &core::Web::skipAssaigment);

    auto& type = mWeb[aCommand];
    type.mType = Node::Type::Type;
    for (auto& cur : parts)
    {
        auto name = file::Parser::slice(cur, " =");
        type.mLeaves["child"].insert(name[0]);
        createNode(name[0], Node::Type::Variable, {"is"s, aCommand});
        result.insert(std::move(name[0]));
    }

    return result;
}

std::unordered_set<std::string>
core::Web::funcHandler(const std::string& aCommand,
                       const std::string& aArgs) noexcept
{
    auto parts = file::Parser::slice(aArgs, "(,", &core::Web::skipAssaigment);

    std::string funcName = parts[0];
    std::string funcType = aCommand;

    createNode("function", Node::Type::Concept, {"child"s, funcName});
    createTree(funcName, Node::Type::Function,
               {
                   {"is"s,     "function"},
                   {"return"s, funcType  }
    });
    createNode(funcType, Node::Type::Type, {"associated"s, funcName});

    return {funcName};
}

std::unordered_set<std::string>
core::Web::cicleHandler(const std::string& aCommand,
                        const std::string& aArgs) noexcept
{
    static int cicleNumber = 0;
    std::string curName    = "cicle_" + std::to_string(cicleNumber++);

    createNode(aCommand, Node::Type::Concept, {"child"s, curName});
    createNode(curName, Node::Type::Cicle, {"is"s, aCommand});

    auto& cur   = mWeb[curName];
    auto blocks = file::Parser::slice(getInsides(aArgs), ";", "", false);
    if (!blocks[1].empty()) blocks[1] = "if " + blocks[1];

    for (auto& i : blocks)
    {
        auto ptr  = i.c_str();
        auto temp = process(ptr);
        for (auto& j : temp)
        {
            cur.mLeaves["contain"].insert(j);
            mWeb[j].mLeaves["located"].insert(curName);
        }
    }
    // auto tempPtr = blocks[0].c_str();
    // auto var     = process(tempPtr);
    // for (auto& i : var)
    // {
    //     cur.mLeaves["contain"].insert(i);
    //     mWeb[i].mLeaves["located"].insert(curName);
    // }

    return {curName};
}

std::unordered_set<std::string>
core::Web::conditionHandler(const std::string& aCommand,
                            const std::string& aArgs) noexcept
{
    static int ifNumber    = -1;
    static int blockNumber = 0;
    if (aCommand == "if")
    {
        blockNumber = 0;
        ++ifNumber;
    }
    else
    {
        ++blockNumber;
    }

    std::string ifName    = "if_" + std::to_string(ifNumber);
    std::string blockName = "cond_block_" + std::to_string(ifNumber) + "_" +
                            std::to_string(blockNumber);

    createNode("condition", Node::Type::Concept, {"child"s, ifName});
    createNode(ifName, Node::Type::Condition, {"is"s, "condition"});

    createNode(blockName, Node::Type::ConditionBlock, {"located"s, ifName});
    createNode(ifName, Node::Type::Condition, {"contain"s, blockName});

    std::string data = getInsides(aArgs);

    auto var = expressionHandler("", getInsides(aArgs));
    for (auto& i : var)
    {
        createNode(blockName, Node::Type::ConditionBlock, {"use"s, i});
        createNode(i, Node::Type::Condition, {"participate"s, blockName});
    }

    return {blockName};
}

std::unordered_set<std::string>
core::Web::expressionHandler(const std::string& aCommand,
                             const std::string& aArgs) noexcept
{
    std::unordered_set<std::string> result;
    auto blocks = file::Parser::slice(aArgs, " \n\t+-");

    for (auto&& i : blocks)
    {
        if (mWeb.count(i))
        {
            result.insert(std::move(i));
        }
    }

    if (aCommand == "expression" && !result.empty())
    {
        static int expressionNumber = 0;
        std::string blockName =
            "expr_block_" + std::to_string(expressionNumber++);

        auto temp = std::move(result);
        for (auto& i : temp)
        {
            createNode(blockName, Node::Type::Expression, {"use"s, i});
            createNode(i, Node::Type::Variable, {"participate"s, blockName});
        }
        result = {blockName};
    }

    return result;
}

std::string
core::Web::getInsides(const std::string& aStr) noexcept
{
    // TODO: erase
    int start          = aStr.find('(') + 1;
    int end            = aStr.find(')');
    std::string result = aStr.substr(start, end - start);

    return result;
}

void
core::Web::createNode(
    const std::string& aName,
    Node::Type aType,
    const std::pair<std::string, std::string>& aLeave) noexcept
{
    createTree(aName, aType, {aLeave});
}

void
core::Web::createTree(
    const std::string& aName,
    Node::Type aType,
    const std::unordered_map<std::string, std::string>& aLeave) noexcept
{
    auto& cur = mWeb[aName];
    cur.mType = aType;
    for (auto& i : aLeave)
    {
        cur.mLeaves[i.first].insert(i.second);
    }
    // cur.mLeaves.insert(aLeave.begin(), aLeave.end());
}

void
core::Web::skipDefault(const char*& c) noexcept
{
    skipAssaigment(c);
    skipCicle(c);
    while (*c != '\0' && (*c == '\n' || *c == '\t'))
    {
        ++c;
    }
}

void
core::Web::skipAssaigment(const char*& c) noexcept
{
    if (*c == '=')
    {
        while (true)
        {
            if (*c == '\0') break;
            if (*c == ',' || *c == ';') break;
            ++c;
        }
    }
}

void
core::Web::skipCicle(const char*& c) noexcept
{
    if (*c == '(')
    {
        while (*c != '\0' && *c != ')')
        {
            ++c;
        }
    }
}

std::unordered_set<std::string>
core::Web::process(const char*& aStr) noexcept
{
    std::unordered_set<std::string> result;

    static std::unordered_map<std::string, decltype(&core::Web::typeHandler)>
        router = {
            {"int",        &core::Web::typeHandler      },
            {"float",      &core::Web::typeHandler      },
            {"double",     &core::Web::typeHandler      },
            {"for",        &core::Web::cicleHandler     },
            {"while",      &core::Web::cicleHandler     },
            {"if",         &core::Web::conditionHandler },
            {"else if",    &core::Web::conditionHandler },
            {"else",       &core::Web::conditionHandler },
            {"expression", &core::Web::expressionHandler}
    };

    while (*aStr != '\0' && *aStr != '}')
    {
        std::string curStr;

        if (std::isspace(*aStr))
        {
            ++aStr;
            continue;
        }

        while (*aStr != '\0')
        {
            curStr.push_back(*aStr++);

            if (check(aStr)) continue;

            auto it = router.end();
            std::string args;
            while (!curStr.empty() && it == router.end())
            {
                args.push_back(curStr.back());
                curStr.pop_back();

                it = router.find(curStr);
            }
            std::reverse(args.begin(), args.end());

            if (it == router.end())
            {
                curStr = "expression";
                it     = router.find("expression");
            }
            auto temp = (this->*it->second)(curStr, args);
            if (!temp.empty()) curStr = *temp.begin();
            for (auto&& i : temp)
            {
                result.insert(std::move(i));
            }

            if (*aStr != '\0' && *aStr++ == '{')
            {
                auto temp = process(aStr);

                for (auto& name : temp)
                {
                    createNode(curStr, Node::Type::Variable,
                               {"contain"s, name});
                    createNode(name, Node::Type::Variable,
                               {"located"s, curStr});
                }
            }

            break;
        }
    }

    if (*aStr == '}') ++aStr;

    auto temp = std::move(result);
    for (auto& i : temp)
    {
        if (i.find("cond_block") == std::string::npos)
        {
            result.insert(i);
        }
        else
        {
            result.insert(*mWeb[i].mLeaves.find("located")->second.begin());
        }
    }

    return result;
}

bool
core::Web::check(const char*& aStr) noexcept
{
    static int n = 0;
    if (*aStr == '(') ++n;
    if (*aStr == ')') --n;
    return n != 0 || *aStr != '{' && *aStr != ';' && *aStr != '\0';
}
