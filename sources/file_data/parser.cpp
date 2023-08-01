#include "parser.hpp"

#include <iostream>

#include "file.hpp"

std::optional<file::Variable>
file::Parser::makeVariable(const std::string& aStr) noexcept
{
    std::optional<Variable> result;

    int num = aStr.find('=');
    if (num != std::string::npos)
    {
        Variable temp;

        for (size_t i = 0; i < num; ++i)
        {
            if (std::islower(aStr[i]) || std::isupper(aStr[i]) ||
                aStr[i] == '_')
            {
                temp.name += aStr[i];
            }
        }

        temp.value.str = aStr.substr(num + 2, aStr.size());
        normalize(temp.name, Type::Lower);
        result = std::move(temp);
    }

    return result;
}

std::vector<file::Variable>
file::Parser::getVariablesFromFile(const std::string aFilename,
                                   bool aIsCritical) noexcept
{
    std::vector<Variable> result;

    auto lines = file::File::getLines(aFilename, aIsCritical);
    for (auto& str : lines)
    {
        auto temp = file::Parser::makeVariable(str);
        if (temp.has_value())
        {
            result.emplace_back(std::move(temp.value()));
        }
        else
        {
            std::cout << "ERROR: '" << str << "' from " << aFilename
                      << " isn't variable!" << std::endl;
            continue;
        }
    }

    return result;
}

std::vector<std::string>
file::Parser::slice(const std::string& aStr, char aDelimiters) noexcept
{
    std::vector<std::string> result(1);

    for (auto& i : aStr)
    {
        if (std::isalpha(i) || i == '_' || i == '~')
        {
            result.back().push_back(i);
        }
        if (i == ',')
        {
            result.emplace_back();
        }
    }

    return result;
}

std::vector<std::string>
file::Parser::slice(const std::string& aStr,
                    const std::string& aDelimiters,
                    const std::string& aErase) noexcept
{
    std::vector<std::string> result(1);

    for (auto i : aStr)
    {
        if (aDelimiters.find(i) == std::string::npos)
        {
            if (aErase.find(i) == std::string::npos)
            {
                result.back().push_back(i);
            }
        }
        else if (!result.back().empty())
        {
            result.emplace_back();
        }
    }

    return result;
}

std::vector<std::string>
file::Parser::slice(const std::string& aStr,
                    const std::string& aDelimiters,
                    const std::function<void(const char*& c)>& aSkiper) noexcept
{
    std::vector<std::string> result(1);

    auto c = aStr.c_str();
    while (*c != '\0')
    {
        auto temp = c;
        aSkiper(c);
        while (temp != c) result.back().push_back(*temp++);

        if (aDelimiters.find(*c) == std::string::npos)
        {
            if (!result.back().empty() || !std::isspace(*c))
            {
                result.back().push_back(*c);
            }
        }
        else if (!result.back().empty())
        {
            result.emplace_back();
        }
        ++c;
    }

    while (result.back().empty()) result.pop_back();

    return result;
}

void
file::Parser::normalize(std::string& aStr, Type aType) noexcept
{
    if (aType == Type::Upper)
    {
        for (auto& i : aStr)
            if (std::islower(i)) i = std::toupper(i);
    }
    else if (aType == Type::Lower)
    {
        for (auto& i : aStr)
            if (std::isupper(i)) i = std::tolower(i);
    }
}

std::string
file::Parser::normalize(const std::string& aStr, Type aType) noexcept
{
    std::string result = aStr;
    normalize(result, aType);
    return result;
}
