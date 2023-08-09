#include "path.hpp"

#include <filesystem>

#include "domain/log.hpp"

#include "file.hpp"
#include "parser.hpp"
#include "variable_storage.hpp"

//--------------------------------------------------------------------------------

#if defined(BILL_WINDOWS)
#    include <windows.h>
#elif defined(LINUS_LINUX)
#    include <limits.h>
#    include <unistd.h>
#endif

//--------------------------------------------------------------------------------

file::Path::Path() noexcept
{
    reset();
}

file::Path&
file::Path::getInstance() noexcept
{
    static Path instance;
    return instance;
}

void
file::Path::reset() noexcept
{
    setPath("exe", getExecutablePath());
    setPath("main", calculateMainPath(mPaths["exe"]));
    // TODO: add all folders from project?
    // setPath("config", mPaths["main"] + "config/");
    auto temp = getAllFoldersPathMap(getPath("main").value());
    mPaths.insert(temp.begin(), temp.end());

    auto& var     = file::VariableStorage::getInstance();
    auto def_path = var.getWord("default_path");
    if (def_path.has_value())
    {
        setDefault(def_path.value());
    }

    auto pathFile = getPath("config").value() + "path.conf";
    auto paths    = file::Parser::getVariablesFromFile(pathFile);
    for (auto& var : paths)
    {
        if (var.value.getType() != file::Value::Type::String)
        {
            dom::writeError("'", var.name, "' from ", pathFile, " isn't path");
            continue;
        }

        mPaths[var.name] = var.value;
    }

    if (mPaths.empty())
    {
        dom::writeError("No paths file detected");
    }
}

//--------------------------------------------------------------------------------

boost::optional<const std::string&>
file::Path::getPath(const std::string& aName) noexcept
{
    boost::optional<const std::string&> result;

    auto it = mPaths.find(aName);
    if (it == mPaths.end())
    {
        it = mPaths.find("default");
        if (it != mPaths.end())
        {
            it = mPaths.insert({aName, it->second + aName + "/"}).first;
        }
    }
    if (it != mPaths.end()) result = it->second;

    return result;
}

std::optional<std::string>
file::Path::getPath(const std::string& aFolder,
                    const std::string& aName) noexcept
{
    std::optional<std::string> result;

    auto folder = getPath(aFolder);
    if (folder.has_value())
    {
        result = folder.value() + aName;
    }
    else
    {
        dom::writeWarning("No such folder (", aFolder, ")");
    }

    return result;
}

std::string
file::Path::getPathUnsafe(const std::string& aFolder,
                          const std::string& aName) noexcept
{
    std::string result;

    auto folder = getPath(aFolder);
    if (folder.has_value())
    {
        result = folder.value() + aName;
    }
    else
    {
        dom::writeError("No such folder (", aFolder, ")");
    }

    return result;
}

std::string
file::Path::generateConfigFolderPath() noexcept
{
    return calculateMainPath(getExecutablePath()) + "config/";
}

void
file::Path::setPath(const std::string& aName, const std::string& aPath) noexcept
{
    mPaths[aName] = aPath;
}

void
file::Path::setDefault(const std::string& aPath) noexcept
{
    setPath("default", aPath);
}

//--------------------------------------------------------------------------------

std::vector<std::string>
file::Path::getAllContentPaths(const std::string& aPath) noexcept
{
    std::vector<std::string> result;
    for (const auto& entry : std::filesystem::directory_iterator(aPath))
        result.emplace_back(entry.path().string());
    return result;
}

std::unordered_map<std::string, std::string>
file::Path::getAllContentPathsMap(const std::string& aPath) noexcept
{
    std::unordered_map<std::string, std::string> result;
    auto paths = getAllContentPaths(aPath);
    for (auto& i : paths)
    {
        int num = i.size();
        while (num >= 0 && i[num] != '/') num--;
        result[i.substr(num + 1, i.size())] = i;
    }
    return result;
}

// TODO: merge with upper
std::unordered_map<std::string, std::string>
file::Path::getAllFoldersPathMap(const std::string& aPath) noexcept
{
    std::unordered_map<std::string, std::string> result;
    for (const auto& i : std::filesystem::recursive_directory_iterator(aPath))
    {
        if (i.is_directory())
        {
            auto path = i.path().string();
            if (path.find("\\.") != std::string::npos ||
                path.find("git") != std::string::npos ||
                path.find("build") != std::string::npos)
                continue;

            int num = path.size();
            while (num >= 0 && path[num] != '/') num--;
            auto name = path.substr(num + 1, path.size());

            // if (name[0] == '.' || name == "git" || name == "build") continue;
            result[name] = path + "\\";
        }
    }
    return result;
}

//--------------------------------------------------------------------------------

std::string
file::Path::calculateMainPath(const std::string& aPath) noexcept
{
    std::string result = aPath;
    do result.pop_back();
    while (result.back() != '/');
    return result;
}

std::string
file::Path::getExecutablePath() noexcept
{
#if defined(BILL_WINDOWS)
    CHAR buffer[MAX_PATH] = {0};
    uint8_t size          = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    for (int i = 0; i < 1; ++i)
        while (buffer[--size] != L'\\')
            ;
    buffer[size + 1] = 0;

    std::string result(buffer);
    for (auto& i : result)
        if (i == '\\') i = '/';
    return result;
#elif defined(LINUS_LINUX)
    char buf[PATH_MAX + 1];
    if (readlink("/proc/self/exe", buf, sizeof(buf) - 1) == -1)
        dom::writeError("readlink() failed");
    std::string str(buf);
    int i = str.size() - 1;
    for (int j = 0; j < 1; --i)
    {
        if (str[i] == '/') ++j;
    }
    return str.substr(0, i + 2);
    // return str.substr(0, str.rfind('/'));
#else
    return "";
#endif
}

//--------------------------------------------------------------------------------
