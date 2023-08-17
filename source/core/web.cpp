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
    mFileName = aFileName.substr(aFileName.rfind('/'), aFileName.size());

    auto code = file::File::getAllData(aFileName);
    int num   = 0;
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

std::vector<int>
getPrime()
{
    std::vector<int> result;

    std::vector<char> pr(1e7, true);
    for (long long i = 2; i < long long(pr.size()); ++i)
    {
        if (pr[i])
        {
            for (long long j = i * i; j < long long(pr.size()); j += i)
            {
                pr[j] = false;
            }
            result.push_back(i);
        }
    }

    std::reverse(result.begin(), result.end());

    return result;
}

std::unordered_map<std::string, int>
core::Web::getCodes() noexcept
{
    std::unordered_map<std::string, int> result;
    auto primes = getPrime();

    for (int type = int(Node::Type::Nun); type < int(Node::Type::Last); ++type)
    {
        result[std::to_string(type)] = primes.back();
        primes.pop_back();
    }

    std::unordered_map<std::string, std::string> relationSwitch =
        makeRelationMap();
    for (auto& i : relationSwitch)
    {
        // auto it = result.find(relationSwitch[i.first]);
        // if (it != result.end())
        // {
        //     result[i.first] = it->second;
        // }
        // else
        {
            result[i.first] = primes.back();
            primes.pop_back();
        }
    }

    return result;
}

void
core::Web::makeMatrix() const noexcept
{
    static std::unordered_map<std::string, int> codes = getCodes();

    std::vector<long long> attributes;
    std::unordered_map<std::string, int> nums;
    for (int type = int(Node::Type::Nun); type < int(Node::Type::Last); ++type)
    {
        for (auto& i : mWeb)
        {
            if (i.second.mType != Node::Type(type)) continue;
            nums[i.first] = nums.size();
            attributes.push_back(codes[std::to_string(int(i.second.mType))]);
        }
    }

    std::vector<std::vector<int>> table(nums.size(),
                                        std::vector<int>(nums.size()));

    for (auto& i : mWeb)
    {
        for (auto& j : i.second.mLeaves)
        {
            for (auto& k : j.second)
            {
                auto& x = nums[i.first];
                auto& y = nums[k];
                int d   = codes[j.first];

                table[x][y] = d;
            }
        }
    }

    auto rec = recurrentSearch(table, attributes);

    std::string data;
    for (int i = 0; i < table.size(); ++i)
    {
        for (auto& j : table[i])
        {
            data += std::to_string(j) + " ";
        }
        data += " = " + std::to_string(attributes[i]) + "\n";
    }
    data += "\n";
    for (auto& i : rec)
    {
        data += std::to_string(i) + " ";
    }
    file::File::writeData("output", mFileName + "_matrix", data);
}

std::vector<long long>
core::Web::recurrentSearch(std::vector<std::vector<int>> aTable,
                           std::vector<long long> attributes) const noexcept
{
    std::vector<std::vector<long long>> characteristics;
    for (auto& i : aTable)
    {
        auto& temp = characteristics.emplace_back();
        for (int j = 0; j < i.size(); ++j)
        {
            if (i[j])
            {
                temp.emplace_back(i[j] * attributes[j]);
            }
        }
        std::sort(temp.begin(), temp.end());
    }

    std::vector<long long> result;
    std::unordered_map<std::string, long long> hash;
    for (auto& i : characteristics)
    {
        std::string s;
        for (auto& j : i)
        {
            s += std::to_string(j) + " ";
        }

        auto& temp = hash[s];
        if (temp == 0)
        {
            temp = hash.size();
        }
        result.emplace_back(temp);
    }

    std::string data;
    for (auto& i : characteristics)
    {

        for (auto& j : i)
        {
            data += std::to_string(j) + " ";
        }
        data += "\n";
    }
    file::File::writeData("output", mFileName + "_char", data);

    return result;
}

void
core::Web::print() const noexcept
{
    std::string data;

    int cnt = 0;
    for (int type = int(Node::Type::Nun); type < int(Node::Type::Last); ++type)
    {
        for (auto& i : mWeb)
        {
            if (i.second.mType != Node::Type(type)) continue;

            data += std::to_string(++cnt) + ") " + i.first + "\n";
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

    file::File::writeData("output", mFileName + "_print", data);
    file::File::writeData("output", mFileName + "_expr", mExprInfo);
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

    auto parts = file::Parser::slice(aArgs, ",");

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
    auto parts           = file::Parser::slice(aArgs, "(,");
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
            file::Path::getPathUnsafe("language", "method.txt"));

    std::string expNumStr = std::to_string(mExpressionNumber);
    while (expNumStr.size() < 3) expNumStr = "0" + expNumStr;
    std::string blockName = "expr_block_" + expNumStr;
    // dom::writeInfo(blockName, "<->", aCommand, aArgs);
    mExprInfo += blockName + " <-> " + aCommand + " " + aArgs + "\n";

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
            file::Path::getPathUnsafe("language", "relation.txt"));

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
                 file::Path::getPathUnsafe("language", "concept.txt")),
             Node::Type::Concept},
            {file::File::getWordsSet(
                 file::Path::getPathUnsafe("language", "type.txt")),
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

    auto types =
        file::File::getLines(file::Path::getPathUnsafe("language", "type.txt"));
    for (auto& i : types)
    {
        result[i] = &core::Web::typeHandler;
    }

    auto containers = file::File::getLines(
        file::Path::getPathUnsafe("language", "containers.txt"));
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
