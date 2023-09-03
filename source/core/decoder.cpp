#include "decoder.hpp"

#include "file_data/file.hpp"
#include "file_data/parser.hpp"
#include "file_data/path.hpp"

#include "language.hpp"

core::Web
core::Decoder::processFile(const std::string& aFileName) noexcept
{
    Decoder d(aFileName);
    Web result = std::move(d.mWeb);
    return result;
}

core::Decoder::Decoder(const std::string& aFileName) noexcept
    : mCicleNumber(0),
      mConditionBlockNumber(0),
      mExpressionNumber(0),
      mWeb(aFileName.substr(aFileName.rfind('/') + 1, aFileName.size()))
{
    auto mFileName =
        aFileName.substr(aFileName.rfind('/') + 1, aFileName.size());
    auto code = file::File::getAllData(aFileName);
    preprocess(code);
    process(code);
}

//--------------------------------------------------------------------------------

void
core::Decoder::preprocess(std::string& aData) noexcept
{
    static std::unordered_set<std::string> names = {"if", "else", "for",
                                                    "while"};

    for (auto& i : names)
    {
        int num = 0;
        while (true)
        {
            num = aData.find(i, num);
            if (num != std::string::npos)
            {
                int cnt = 0;
                if (i != "else")
                {
                    while (!(aData[num] == ')' && cnt == 1))
                    {
                        if (aData[num] == '(') ++cnt;
                        if (aData[num] == ')') --cnt;
                        ++num;
                    }
                }
                ++num;
                while (std::isspace(aData[num])) ++num;
                if (aData[num] == 'i' && aData[num + 1] == 'f') continue;
                if (aData[num] != '{')
                {
                    aData[num - 1]                  = '{';
                    aData[aData.find(';', num) + 1] = '}';
                }
            }
            else
            {
                break;
            }
        }
    }
}

std::unordered_set<std::string>
core::Decoder::process(const char*& aStr) noexcept
{
    std::unordered_set<std::string> result;

    static std::unordered_map<std::string,
                              decltype(&core::Decoder::typeHandler)>
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
                    mWeb.createEdge(curStr, name, "contain");
                }
            }

            break;
        }
    }

    if (*aStr == '}') ++aStr;

    // auto temp = std::move(result);
    // for (auto& i : temp)
    // {
    //     if (i.find("cond_block") == std::string::npos)
    //     {
    //         result.insert(i);
    //     }
    //     else
    //     {
    //         result.insert(*mWeb[i].mLeaves.find("located")->second.begin());
    //     }
    // }

    return result;
}

std::unordered_set<std::string>
core::Decoder::process(const std::string& aStr) noexcept
{
    auto ptr = aStr.c_str();
    return process(ptr);
}

//--------------------------------------------------------------------------------

std::unordered_map<std::string, decltype(&core::Decoder::typeHandler)>
core::Decoder::getRouter() noexcept
{
    std::unordered_map<std::string, decltype(&core::Decoder::typeHandler)>
        result;

    auto temp = Language::getNodeList();
    std::unordered_map<Node::Type, decltype(&core::Decoder::typeHandler)>
        router = {
            {Node::Type::Function,   &core::Decoder::typeHandler      },
            {Node::Type::Type,       &core::Decoder::typeHandler      },
            {Node::Type::Container,  &core::Decoder::containerHandler },
            {Node::Type::Condition,  &core::Decoder::conditionHandler },
            {Node::Type::Cicle,      &core::Decoder::cicleHandler     },
            {Node::Type::Expression, &core::Decoder::expressionHandler},
    };

    for (auto& i : temp)
    {
        auto it = router.find(i.second);
        if (it != router.end())
        {
            result[i.first] = it->second;
        }
    }

    return result;
}

//--------------------------------------------------------------------------------

std::unordered_set<std::string>
core::Decoder::typeHandler(const std::string& aCommand,
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
core::Decoder::declarationHandler(const std::string& aCommand,
                                  const std::string& aArgs) noexcept
{
    std::unordered_set<std::string> result;

    auto parts = file::Parser::slice(aArgs, ",");

    mWeb.addNode(aCommand, Node::Type::Type);
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

        mWeb.createEdge(name, aCommand, "is"s, Node::Type::Variable);
        mVariables.insert(name);
        result.insert(std::move(name));
    }

    return result;
}

std::unordered_set<std::string>
core::Decoder::funcHandler(const std::string& aCommand,
                           const std::string& aArgs) noexcept
{
    auto parts           = file::Parser::slice(aArgs, "(,");
    std::string funcType = aCommand;
    std::string funcName = parts[0];

    mWeb.createEdge(funcName, "function", "child"s, Node::Type::Function);
    mWeb.createEdge(funcType, funcName, "associated"s, Node::Type::Type);

    return {funcName};
}

std::unordered_set<std::string>
core::Decoder::cicleHandler(const std::string& aCommand,
                            const std::string& aArgs) noexcept
{
    std::string curName = "cicle_" + std::to_string(mCicleNumber++);

    mWeb.createEdge(curName, "cicle", "is"s, Node::Type::Cicle);

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
        mWeb.createEdge(curName, i, "contain");
    }

    return {curName};
}

std::unordered_set<std::string>
core::Decoder::conditionHandler(const std::string& aCommand,
                                const std::string& aArgs) noexcept
{
    std::string blockName =
        "cond_block_" + std::to_string(mConditionBlockNumber);

    mWeb.createEdge(blockName, "condition", "is", Node::Type::Condition);
    if (aCommand != "if")
    {
        std::string prev =
            "cond_block_" + std::to_string(mConditionBlockNumber - 1);
        mWeb.createEdge(blockName, prev, "next", Node::Type::Condition);
    }

    std::string data = getInsides(aArgs);
    auto var         = process(getInsides(aArgs));
    for (auto& i : var)
    {
        mWeb.createEdge(blockName, i, "use");
    }

    ++mConditionBlockNumber;

    return {blockName};
}

std::unordered_set<std::string>
core::Decoder::expressionHandler(const std::string& aCommand,
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
    // mExprInfo += blockName + " <-> " + aCommand + " " + aArgs + "\n";

    // TODO: ++ -- as asigment
    bool flag  = false;
    auto parts = file::Parser::slice(aArgs, " \t\n*/=<>");

    if (parts[0].find("++") != std::string::npos ||
        parts[0].find("--") != std::string::npos ||
        aArgs.find(" = ") != std::string::npos && !parts.empty())
    {
        mWeb.createEdge(blockName, file::Parser::slice(parts[0], "+-")[0],
                        "sets");
        parts[0].clear();
        flag = true;
    }

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
        mWeb.addNode(blockName, Node::Type::Expression);
        result = {blockName};
        ++mExpressionNumber;
    }

    return result;
}

std::unordered_set<std::string>
core::Decoder::containerHandler(const std::string& aCommand,
                                const std::string& aArgs) noexcept
{
    std::string temp = aArgs.substr(aArgs.rfind('>') + 1, aArgs.size());
    return declarationHandler(aCommand, temp);
}

//--------------------------------------------------------------------------------

std::string
core::Decoder::getName(
    decltype(core::Decoder::mVariables.begin()) aPtr) noexcept
{
    return *aPtr;
}

std::string
core::Decoder::getName(
    std::unordered_map<std::string, std::string>::const_iterator aPtr) noexcept
{
    return aPtr->second;
}

//--------------------------------------------------------------------------------

bool
core::Decoder::check(const char*& aStr) noexcept
{
    static int n = 0;
    if (*aStr == '(') ++n;
    if (*aStr == ')') --n;
    return n != 0 || *aStr != '{' && *aStr != ';' && *aStr != '\0';
}

std::string
core::Decoder::getInsides(const std::string& aStr) noexcept
{
    // TODO: erase
    int start          = aStr.find('(') + 1;
    int end            = aStr.find(')');
    std::string result = aStr.substr(start, end - start);

    return result;
}
