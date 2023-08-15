#include "web.hpp"

#include <algorithm>
#include <iostream>

#include "domain/log.hpp"

#include "file_data/file.hpp"
#include "file_data/parser.hpp"
#include "file_data/path.hpp"
#include "file_data/variable_storage.hpp"

core::Web::Web(const std::string& aFileName) noexcept
    : mCicleNumber(0), mIfNumber(-1), mBlockNumber(0), mExpressionNumber(0)
{
    auto code = file::File::getAllData(
        file::Path::getPathUnsafe("resources", aFileName));
    int num = 0;
    for (auto& i : {"if", "else", "for", "while"})
    {
        while (true)
        {
            num = code.find(i, num);
            if (num != std::string::npos)
            {
                int cnt = 0;
                while (!(code[num] == ')' && cnt == 1))
                {
                    if (code[num] == '(') ++cnt;
                    if (code[num] == ')') --cnt;
                    ++num;
                }
                ++num;
                while (std::isspace(code[num])) ++num;
                if (code[num] != '{')
                {
                    code[num - 1]                 = '{';
                    code[code.find(';', num) + 1] = '}';
                }
            }
            else
            {
                break;
            }
        }
    }
    process(code);
}

std::map<std::string, core::Node>
core::Web::processFile(const std::string& aFileName) noexcept
{
    Web w(aFileName);
    return w.mWeb;
}

void
core::Web::print() const noexcept
{
    std::string data;

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

    mWeb[aCommand].mType = Node::Type::Type;
    for (auto& cur : parts)
    {
        auto temp = file::Parser::slice(cur, " ()=");
        auto name = temp[0];

        if (cur.find('=') != std::string::npos /*||
            cur.find('(') != std::string::npos*/)
        {
            // auto temp = expressionHandler("", cur);
            auto value = process(cur);
            result.insert(value.begin(), value.end());
        }

        createEdge(name, aCommand, "is"s, Node::Type::Variable);
        mVariables.insert(name);
        result.insert(std::move(name));
    }

    return result;
}

std::unordered_set<std::string>
core::Web::funcHandler(const std::string& aCommand,
                       const std::string& aArgs) noexcept
{
    auto parts = file::Parser::slice(aArgs, "(,", &core::Web::skipAssaigment);
    std::string funcType = aCommand;
    std::string funcName = parts[0];

    createEdge(funcName, "function", "child"s, Node::Type::Function);
    createEdge(funcType, funcName, "associated"s, Node::Type::Type);

    return {funcName};
}

std::unordered_set<std::string>
core::Web::cicleHandler(const std::string& aCommand,
                        const std::string& aArgs) noexcept
{
    std::string curName = "cicle_" + std::to_string(mCicleNumber++);

    createEdge(curName, "cicle", "is"s, Node::Type::Cicle);

    auto insides = getInsides(aArgs);
    auto num     = insides.find(';');
    if (num != std::string::npos)
    {
        insides.insert(num + 1, "if (");
        insides.insert(insides.find(';', num + 1), ")");
    }

    auto blocks = process(insides);
    for (auto& i : blocks)
    {
        createEdge(curName, i, "contain");
    }

    return {curName};
}

std::unordered_set<std::string>
core::Web::conditionHandler(const std::string& aCommand,
                            const std::string& aArgs) noexcept
{
    if (aCommand == "if")
    {
        mBlockNumber = 0;
        ++mIfNumber;
    }
    else
    {
        ++mBlockNumber;
    }

    std::string ifName    = "if_" + std::to_string(mIfNumber);
    std::string blockName = "cond_block_" + std::to_string(mIfNumber) + "_" +
                            std::to_string(mBlockNumber);

    createEdge(ifName, "condition", "is", Node::Type::Condition);
    createEdge(blockName, ifName, "located", Node::Type::ConditionBlock);

    std::string data = getInsides(aArgs);
    auto var         = process(getInsides(aArgs));
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
    std::unordered_set<std::string> result;

    static std::unordered_map<std::string, std::string> methods =
        file::File::getWordsMap(
            file::Path::getPathUnsafe("resources", "method.txt"));

    std::string expNumStr = std::to_string(mExpressionNumber);
    while (expNumStr.size() < 3) expNumStr = "0" + expNumStr;
    std::string blockName = "expr_block_" + expNumStr;
    dom::writeInfo(blockName, "<->", aCommand, aArgs);

    // TODO: ++ -- as asigment
    bool flag  = false;
    auto parts = file::Parser::slice(aArgs, " \t\n*/=<>");

    if (parts[0].find("++") != std::string::npos ||
        parts[0].find("--") != std::string::npos ||
        aArgs.find(" = ") != std::string::npos && !parts.empty())
    {
        createEdge(blockName, file::Parser::slice(parts[0], "+-")[0], "sets");
        parts[0].clear();
        flag = true;
    }
    // for (auto& i : parts)
    // {
    //     bool
    //     if ()
    //     {
    //         i.erase(i.find("+"), 2);
    //     }
    //     if (i.find("--") != std::string::npos)
    //     {
    //         i.erase(i.find("-"), 2);
    //     }
    // }

    std::string type = "use";
    if (parts[0] == "cin") type = "input";
    if (parts[0] == "cout") type = "output";

    for (auto& i : parts)
    {
        auto temp = file::Parser::slice(i, "[]");
        for (int j = 0; j < temp.size(); ++j)
        {
            flag |= usePart(mVariables, blockName, j ? "use" : type, temp[j]);
            flag |= usePart(methods, blockName, "use", temp[j]);
        }
    }

    if (flag)
    {
        mWeb[blockName].mType = Node::Type::Expression;
        result                = {blockName};
        ++mExpressionNumber;
    }

    return result;
}

std::string
core::Web::getName(decltype(core::Web::mVariables.begin()) aPtr) noexcept
{
    return *aPtr;
}
std::string
core::Web::getName(
    std::unordered_map<std::string, std::string>::const_iterator aPtr) noexcept
{
    return aPtr->second;
}

std::unordered_set<std::string>
core::Web::containerHandler(const std::string& aCommand,
                            const std::string& aArgs) noexcept
{
    std::string temp = aArgs.substr(aArgs.rfind('>') + 1, aArgs.size());
    return declarationHandler(aCommand, temp);
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

std::unordered_map<std::string, std::string>
core::Web::makeRelationMap() noexcept
{
    std::unordered_map<std::string, std::string> result =
        file::File::getWordsMap(
            file::Path::getPathUnsafe("resources", "relation.txt"));

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
    static std::unordered_map<std::string, std::string> relationSwitch =
        makeRelationMap();

    mWeb[aFrom];
    mWeb[aTo];

    auto& from = *mWeb.find(aFrom);
    auto& to   = *mWeb.find(aTo);

    from.second.mLeaves[aFromRelation].insert(aTo);
    to.second.mLeaves[relationSwitch[aFromRelation]].insert(aFrom);

    if (from.second.mType == Node::Type::Nun) from.second.mType = aFromType;

    if (from.second.mType == Node::Type::Nun) typeAutomaticDetection(from);
    if (to.second.mType == Node::Type::Nun) typeAutomaticDetection(to);
}

void
core::Web::typeAutomaticDetection(decltype(*mWeb.begin())& aNode) noexcept
{
    static std::vector<std::pair<std::unordered_set<std::string>, Node::Type>>
        dict = {
            {file::File::getWordsSet(
                 file::Path::getPathUnsafe("resources", "concept.txt")),
             Node::Type::Concept},
            {file::File::getWordsSet(
                 file::Path::getPathUnsafe("resources", "type.txt")),
             Node::Type::Type   }
    };

    for (auto& i : dict)
    {
        auto it = i.first.find(aNode.first);
        if (it != i.first.end())
        {
            aNode.second.mType = i.second;
            break;
        }
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

std::unordered_map<std::string, decltype(&core::Web::typeHandler)>
core::Web::getRouter() noexcept
{
    std::unordered_map<std::string, decltype(&core::Web::typeHandler)> result =
        {
            {"for",        &core::Web::cicleHandler     },
            {"while",      &core::Web::cicleHandler     },
            {"if",         &core::Web::conditionHandler },
            {"else if",    &core::Web::conditionHandler },
            {"else",       &core::Web::conditionHandler },
            {"expression", &core::Web::expressionHandler}
    };

    auto types = file::File::getLines(
        file::Path::getPathUnsafe("resources", "type.txt"));
    for (auto& i : types)
    {
        result[i] = &core::Web::typeHandler;
    }

    auto containers = file::File::getLines(
        file::Path::getPathUnsafe("resources", "containers.txt"));
    for (auto& i : containers)
    {
        result[i] = &core::Web::containerHandler;
    }

    return result;
}

std::unordered_set<std::string>
core::Web::process(const char*& aStr) noexcept
{
    std::unordered_set<std::string> result;

    static std::unordered_map<std::string, decltype(&core::Web::typeHandler)>
        router = getRouter();

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
                it     = router.find("expression");
                curStr = "expression";
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
