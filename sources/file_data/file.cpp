#include "file.hpp"

#include <fstream>

#include "domain/log.hpp"

#include "path.hpp"

std::string
file::File::getAllData(const std::string& aFileName, bool aIsCritical) noexcept
{
    std::ifstream ios(aFileName);
    if (!ios.is_open())
    {
        dom::writeError("No such file (", aFileName, ")");
        if (aIsCritical) exit(0);
    }
    else
    {
        dom::writeInfo("Extracting file (", aFileName, ")");
    }

    std::string result;
    std::string line;
    while (std::getline(ios, line, '\0'))
    {
        if (line.empty()) continue;
        result += line;
    }
    return result;
}

std::vector<std::string>
file::File::getLines(const std::string& aFileName, bool aIsCritical) noexcept
{
    std::string temp = getAllData(aFileName, aIsCritical);
    std::vector<std::string> result;
    int last = -1;
    for (int i = 0; i < temp.size() + 1; ++i)
    {
        if (temp[i] == '\n' || temp[i] == '\0')
        {
            if (i - last > 1)
            {
                result.emplace_back(temp.substr(last + 1, i - last - 1));
                // TODO: check or update?
                if (result.back().back() == '\r') result.back().pop_back();
            }
            // last = i + 1;
            last = i;
        }
    }
    return result;
}

std::vector<std::vector<std::string>>
file::File::getWords(const std::string& aFileName,
                     bool aIsCritical,
                     std::function<bool(char)> funk) noexcept
{
    auto lines = getLines(aFileName, aIsCritical);
    std::vector<std::vector<std::string>> result;
    for (auto& line : lines)
    {
        result.emplace_back();
        int indx = 0;
        while (indx < line.size())
        {
            while (funk(line[indx])) indx++;
            int from = indx;
            while (!funk(line[indx])) indx++;
            result.back().emplace_back(line.substr(from, indx - from));
            indx += 1;
        }
    }
    return result;
}

// TODO: merge with getWords
std::unordered_map<std::string, std::string>
file::File::getWordsMap(const std::string& aFileName,
                        bool aIsCritical,
                        std::function<bool(char)> funk) noexcept
{
    std::unordered_map<std::string, std::string> result;
    auto words = getWords(aFileName, aIsCritical, funk);

    for (auto& i : words)
    {
        result[i[0]] = i[1];
    }

    return result;
}

std::unordered_set<std::string>
file::File::getWordsSet(const std::string& aFileName,
                        bool aIsCritical,
                        std::function<bool(char)> funk) noexcept
{
    std::unordered_set<std::string> result;
    auto words = getWords(aFileName, aIsCritical, funk);
    for (auto&& i : words)
    {
        for (auto&& j : i)
        {
            result.insert(std::move(j));
        }
    }
    return result;
}

std::string
file::File::writeData(const std::string& aFolderName,
                      const std::string& aFileName,
                      const std::string& aData) noexcept
{
    std::string resultFileName = "";

    auto path = Path::getPath(aFolderName);
    if (!path)
    {
        dom::writeError("No such folder (", aFolderName, ")");
        path = Path::getPath("upload");
    }

    if (path)
    {
        std::string pathPrefix = path.value();
        std::string filePath;

        resultFileName = aFileName;
        filePath       = pathPrefix + resultFileName;

        std::ofstream out(filePath);
        out << aData;
        out.close();
    }
    else
    {
        dom::writeError("No upload folder");
    }

    return resultFileName;
}

bool
file::File::isSeparator(char c) noexcept
{
    return c == '\t' || c == ' ' || c == ';' || c == '\0';
}
