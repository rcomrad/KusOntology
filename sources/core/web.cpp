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
            data += "\t" + j.second + " " + j.first + "\n";
        }
    }

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

std::vector<std::string>
core::Web::typeHandler(const std::string& aCommand,
                       const std::string& aArgs) noexcept
{
    std::vector<std::string> result;

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

std::vector<std::string>
core::Web::declarationHandler(const std::string& aCommand,
                              const std::string& aArgs) noexcept
{
    std::vector<std::string> result;

    auto parts = file::Parser::slice(aArgs, ",", &core::Web::skipAssaigment);

    auto& type = mWeb[aCommand];
    type.mType = Node::Type::Type;
    for (auto& cur : parts)
    {
        auto name = file::Parser::slice(cur, " =");
        result.push_back(name[0]);

        type.mLeaves[name[0]] = "child";
        createNode(name[0], Node::Type::Variable, {aCommand, "is"s});
    }

    return result;
}

std::vector<std::string>
core::Web::funcHandler(const std::string& aCommand,
                       const std::string& aArgs) noexcept
{
    auto parts = file::Parser::slice(aArgs, "(,", &core::Web::skipAssaigment);

    std::string funcName = parts[0];
    std::string funcType = aCommand;

    createNode("function", Node::Type::Concept, {funcName, "child"s});
    createTree(funcName, Node::Type::Function,
               {
                   {"function", "is"s    },
                   {funcType,   "return"s}
    });
    createNode(funcType, Node::Type::Type, {funcName, "associated"s});

    return {funcName};
}

std::vector<std::string>
core::Web::cicleHandler(const std::string& aCommand,
                        const std::string& aArgs) noexcept
{
    static int cicleNumber = 0;
    std::string curName    = aCommand + "_" + std::to_string(cicleNumber);

    createNode(aCommand, Node::Type::Concept, {curName, "child"s});
    createNode(curName, Node::Type::Cicle, {aCommand, "is"s});

    // TODO: erase
    int start        = aArgs.find('(') + 1;
    int end          = aArgs.find(')');
    std::string data = aArgs.substr(start, end - start);

    auto& cur    = mWeb[curName];
    auto blocks  = file::Parser::slice(data, ";");
    auto tempPtr = blocks[0].c_str();
    auto var     = process(tempPtr);
    for (auto& i : var)
    {
        cur.mLeaves[i]           = "contain";
        mWeb[i].mLeaves[curName] = "located";
    }

    return {curName};
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
    cur.mLeaves.insert(aLeave.begin(), aLeave.end());
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

std::vector<std::string>
core::Web::process(const char*& aStr) noexcept
{
    std::vector<std::string> result;

    static std::unordered_map<std::string, decltype(&core::Web::typeHandler)>
        router = {
            {"int",    &core::Web::typeHandler },
            {"float",  &core::Web::typeHandler },
            {"double", &core::Web::typeHandler },
            {"for",    &core::Web::cicleHandler},
            {"while",  &core::Web::cicleHandler},
    };

    while (*aStr != '\0' && *aStr != '}')
    {
        std::string curStr;

        while (*aStr != '\0')
        {
            if (std::isspace(*aStr)) ++aStr;
            else break;
        }

        while (*aStr != '\0')
        {
            curStr.push_back(*aStr++);

            if (*aStr != '{' && *aStr != ';' && *aStr != '\0') continue;

            auto it = router.end();
            std::string args;
            while (!curStr.empty() && it == router.end())
            {
                args.push_back(curStr.back());
                curStr.pop_back();

                it = router.find(curStr);
            }
            std::reverse(args.begin(), args.end());

            if (it != router.end())
            {
                auto temp = (this->*it->second)(curStr, args);
                curStr    = temp[0];
                for (auto&& i : temp)
                {
                    result.emplace_back(std::move(i));
                }
            }

            if (*aStr != '\0' && *aStr++ == '{')
            {
                auto temp = process(aStr);
                for (auto& name : temp)
                {
                    createNode(curStr, Node::Type::Variable,
                               {name, "contain"s});
                    createNode(name, Node::Type::Variable,
                               {curStr, "located"s});
                }
            }

            break;
        }
    }

    return result;
}
