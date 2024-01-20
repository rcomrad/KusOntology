#include "decoder.hpp"

#include "domain/log.hpp"
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
    file::File::writeData("output", mFileName + "_preprocess", code);
    process(code);
}

//--------------------------------------------------------------------------------

#include <regex>
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

    int curNum = 0;
    while ((curNum = aData.find(">", curNum))!= std::string::npos)
    {
        int last = curNum;

        do {++curNum;}
        while (std::isspace(aData[curNum]));

        std::string temp;
        do {
            temp.push_back(aData[curNum]);
            ++curNum;
        }
        while (std::isalpha(aData[curNum]));

        while (std::isspace(aData[curNum])) ++curNum;;

        temp = ";\n" + temp + ".resize";
        if (aData[curNum] == '(')
        {
            aData.insert(curNum, temp);
        }

        curNum = last + 1;
    }

    //std::replace(aData.begin(), aData.end(), "[0]", ".front()");
    aData = std::regex_replace(aData, std::regex("\\[0\\]"), ".front()");
    aData = std::regex_replace(aData, std::regex("unordered_map"), "map");
    aData = std::regex_replace(aData, std::regex("unordered_set"), "set");

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
                                  const std::string& aArgs,
                                  Node::Type aType) noexcept
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

        mWeb.createEdge(name, aCommand, "is"s, aType);
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

    mWeb.createEdge(funcName, "function", "is"s, Node::Type::Function);
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
    dom::writeInfo(blockName, "<->", aCommand, aArgs);
    // mExprInfo += blockName + " <-> " + aCommand + " " + aArgs + "\n";

    // TODO: ++ -- as asigment
    bool flag  = false;
    auto parts = file::Parser::slice(aArgs, " \t\n*/=<>.()[]!");

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

    std::string lastContainer;
    std::string curContainer;
    for (auto& i : parts)
    {
        auto temp = file::Parser::slice(i, "[]");
        auto tempCont = mWeb.getContainer(i);
        if (!tempCont.empty())
        {
            lastContainer = tempCont;
            int phe = lastContainer.find('_');
            curContainer = 
                lastContainer.substr(0, lastContainer.find('_'));
            lastContainer = 
                lastContainer.substr(phe + 1, lastContainer.size());       
        }

        for (int j = 0; j < temp.size(); ++j)
        {
            flag |= usePart(mVariables, blockName, j ? "use" : type, temp[j]);
            if (usePart(methods, blockName, "call", curContainer + "_" + temp[j]))
            {
                flag = true;
                mWeb.createEdge(curContainer, curContainer + "_" + temp[j],
                                        "has_method");
                curContainer = 
                    lastContainer.substr(0, lastContainer.find('_'));
                int phe = lastContainer.find('_');
                lastContainer = 
                    lastContainer.substr(phe + 1, lastContainer.size());                  
            }
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
    std::vector<std::string> types = {aCommand};
    std::string vars = aArgs.substr(aArgs.rfind('>') + 1, aArgs.size());
    std::string subtype = aArgs.substr(1, aArgs.rfind('>') - 1);

    int gg = 0;
    for(int i = 0; i < subtype.size(); ++i)
    {
        if (std::isspace(subtype[i])) ++gg;
        else subtype[i - gg] = subtype[i];
    }
    subtype.resize(subtype.size() - gg);

    while (subtype.find('<') != std::string::npos)
    {
        types.emplace_back(subtype.substr(0, subtype.find('<')));
        subtype = subtype.substr(subtype.find('<') + 1, subtype.rfind('>') - 2);
    }

    std::string command = types[0];
    for(int j = 1; j < types.size(); ++j)
    {
        auto& i = types[j];
        std::string oldComm = command;
        command.push_back('_');
        command += i;
        mWeb.createEdge(command, oldComm, "base"s, Node::Type::Concept);
        mWeb.createEdge(command, i, "contains"s, Node::Type::Concept);
    }
    
    return declarationHandler(command, vars, Node::Type::ContainerVariable);
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
    int end            = aStr.rfind(')');
    std::string result = aStr.substr(start, end - start);

    return result;
}
