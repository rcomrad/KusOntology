#include "web.hpp"

#include <algorithm>
#include <iostream>

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

    // for (auto& v : {"function", "condition", "for", "while"})
    // {
    //     auto it = mWeb.find(v);
    //     if (it == mWeb.end()) continue;
    //     auto i = *it;

    //     data += i.first + "\n";
    //     for (auto& j : i.second.mLeaves)
    //     {
    //         data += "\t" + j.first + "\n";
    //         for (auto& k : j.second)
    //         {
    //             data += "\t\t" + k + "\n";
    //         }
    //     }

    //     mWeb.erase(it);
    // }

    // for (auto& v : {"int", "float", "for", "while", "if", "else if", "else",
    //                 "expression"})
    // {
    //     auto it = mWeb.find(v);
    //     if (it == mWeb.end()) continue;
    //     auto i = *it;

    //     data += i.first + "\n";
    //     for (auto& j : i.second.mLeaves)
    //     {
    //         data += "\t" + j.first + "\n";
    //         for (auto& k : j.second)
    //         {
    //             data += "\t\t" + k + "\n";
    //         }
    //     }

    //     mWeb.erase(it);
    // }

    // for (auto& v : mVariables)
    // {
    //     auto it = mWeb.find(v);
    //     auto i  = *it;

    //     data += i.first + "\n";
    //     for (auto& j : i.second.mLeaves)
    //     {
    //         data += "\t" + j.first + "\n";
    //         for (auto& k : j.second)
    //         {
    //             data += "\t\t" + k + "\n";
    //         }
    //     }

    //     mWeb.erase(it);
    // }

    // for (auto& v : mVariables)
    // {
    //     auto it = mWeb.find(v);
    //     auto i  = *it;

    //     data += i.first + "\n";
    //     for (auto& j : i.second.mLeaves)
    //     {
    //         data += "\t" + j.first + "\n";
    //         for (auto& k : j.second)
    //         {
    //             data += "\t\t" + k + "\n";
    //         }
    //     }

    //     mWeb.erase(it);
    // }

    // for (auto& i : mWeb)
    // {
    //     data += i.first + "\n";
    //     for (auto& j : i.second.mLeaves)
    //     {
    //         data += "\t" + j.first + "\n";
    //         for (auto& k : j.second)
    //         {
    //             data += "\t\t" + k + "\n";
    //         }
    //     }
    // }
    // data += "\t" + j.second + " " + j.first + "\n";
    for (int type = int(Node::Type::Nun); type < int(Node::Type::Last); ++type)
    {
        for (auto& i : mWeb)
        {
            if (i.second.mType != Node::Type(type)) continue;

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
    }

    file::File::writeData("resources", "out", data);
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
        auto temp = file::Parser::slice(cur, " =");
        auto name = temp[0];
        if (cur.find('=') != std::string::npos)
        {
            auto temp = expressionHandler("", cur);
            result.insert(temp.begin(), temp.end());
        }

        mVariables.insert(name);
        type.mLeaves["child"].insert(name);
        createNode(name, Node::Type::Variable, {"is"s, aCommand});
        result.insert(std::move(name));
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

    // auto& cur = mWeb[curName];
    auto blocks = process(getInsides(aArgs));

    for (auto& i : blocks)
    {
        // auto ptr  = i.c_str();
        // auto temp = process(ptr);
        // for (auto& j : temp)
        // {
        createEdge(curName, i, "contain");
        // }
    }

    // if (!blocks[1].empty()) blocks[1] = "if " + blocks[1];

    // for (auto& i : blocks)
    // {
    //     auto ptr  = i.c_str();
    //     auto temp = process(ptr);
    //     for (auto& j : temp)
    //     {
    //         cur.mLeaves["contain"].insert(j);
    //         mWeb[j].mLeaves["located"].insert(curName);
    //     }
    // }
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
        createEdge(blockName, i, "use");
    }

    return {blockName};
}

std::unordered_set<std::string>
core::Web::expressionHandler(const std::string& aCommand,
                             const std::string& aArgs) noexcept
{
    static std::unordered_map<std::string, std::string> methods =
        file::File::getWordsMap(file::Path::getInstance().getPathUnsafe(
            "resources", "methods.txt"));

    std::unordered_set<std::string> result;

    int cnt = 0;

    static int expressionNumber = 0;
    std::string blockName = "expr_block_" + std::to_string(expressionNumber++);
    std::cout << blockName << " <-> " << aCommand << " " << aArgs << "\n";

    // TODO: ++ -- as asigment
    auto parts = file::Parser::slice(aArgs, " \t\n-+*/=<>");
    if (aArgs.find('=') != std::string::npos && !parts.empty())
    {
        createEdge(blockName, parts[0], "sets");
        parts[0].clear();
        ++cnt;
    }

    if (!parts.empty())
    {
        for (auto& i : parts)
        {
            bool flag = false;
            for (auto& j : mVariables)
            {
                if (i == j)
                {
                    createEdge(blockName, j, "use");
                    ++cnt;
                    flag = true;
                    break;
                }
            }

            if (!flag)
            {
                for (auto& j : methods)
                {
                    if (i == j.first)
                    {
                        createEdge(blockName, j.second, "use");
                        ++cnt;
                        break;
                    }
                }
            }
        }

        result = {blockName};
    }

    if (cnt == 0)
    {
        result.clear();
        --expressionNumber;
    }
    else
    {
        mWeb[blockName].mType = Node::Type::Expression;
    }

    // if (assignment.size() > 1)
    // {
    // }
    // else
    // {

    //     for (auto&& i : blocks)
    //     {
    //         if (mWeb.count(i))
    //         {
    //             result.insert(std::move(i));
    //         }
    //     }

    //     if (aCommand == "expression" && !result.empty())
    //     {
    //         static int expressionNumber = 0;
    //         std::string blockName =
    //             "expr_block_" + std::to_string(expressionNumber++);

    //         auto temp = std::move(result);
    //         for (auto& i : temp)
    //         {
    //             createNode(blockName, Node::Type::Expression, {"use"s, i});
    //             createNode(i, Node::Type::Variable,
    //                        {"participate"s, blockName});
    //         }
    //         result = {blockName};
    //     }
    // }

    return result;
}

// std::unordered_set<std::string>
// core::Web::assignmentHandler(const std::string& aCommand,
//                              const std::string& aArgs) noexcept
// {
//     std::unordered_set<std::string> result;

//     static int expressionNumber = 0;
//     std::string blockName = "expr_block_" +
//     std::to_string(expressionNumber++);

//     createEdge(blockName, aCommand, "sets");

//     auto parts = file::Parser::slice(aArgs, " \t\n-+*/");

//     for (auto& i : parts)
//     {
//         for (auto& j : mVariables)
//         {
//             if (i == j)
//             {
//                 createEdge(blockName, j, "use");
//                 break;
//             }
//         }
//     }

//     result = {blockName};
//     return result;
// }

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

std::unordered_map<std::string, std::string>
foo()
{
    std::unordered_map<std::string, std::string> result = {
        {"use",     "participate"},
        {"sets",    "assigned"   },
        {"is",      "child"      },
        {"contain", "located"    },
        {"return",  "associated" }
    };

    std::unordered_map<std::string, std::string> temp;
    for (auto& i : result)
    {
        temp.insert({i.second, i.first});
    }
    result.insert(temp.begin(), temp.end());

    return result;
}

void
core::Web::createEdge(const std::string& aFrom,
                      const std::string& aTo,
                      const std::string& aFromRelation,
                      Node::Type aFromType) noexcept
{
    static std::unordered_map<std::string, std::string> relationSwitch = foo();

    mWeb[aFrom].mLeaves[aFromRelation].insert(aTo);
    mWeb[aTo].mLeaves[relationSwitch[aFromRelation]].insert(aFrom);

    if (aFromType != Node::Type::Nun)
    {
        mWeb[aFrom].mType = aFromType;
    }
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
core::Web::process(const std::string& aStr) noexcept
{
    auto ptr = aStr.c_str();
    return process(ptr);
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
                    createEdge(curStr, name, "contain");
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
