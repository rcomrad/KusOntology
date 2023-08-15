#include "path.hpp"

#include <filesystem>

#include "domain/log.hpp"

#include "file.hpp"
#include "parser.hpp"
// #include "variable_storage.hpp"

//--------------------------------------------------------------------------------

#if defined(BILL_WINDOWS)
#    include <windows.h>
#elif defined(LINUS_LINUX)
#    include <limits.h>
#    include <unistd.h>
#endif

//--------------------------------------------------------------------------------

std::unordered_set<std::string> file::Path::mForbiddenFolders = {
    "/.", "/build", "/cmake_scripts", "/devops"};

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
    mPaths["main"] =
        std::filesystem::current_path().parent_path().string() + "/";
    std::replace(mPaths["main"].begin(), mPaths["main"].end(), '\\', '/');
    addAllFolders(mPaths["main"]);

    auto pathFile = mPaths["config"] + "path.conf";
    auto paths    = file::Parser::getVariablesFromFile(pathFile);
    if (paths.empty())
    {
        dom::writeWarning("Path file doesn't exist or empty");
    }
    for (auto& var : paths)
    {
        if (var.value.getType() != file::Value::Type::String)
        {
            dom::writeError("'", var.name, "' from ", pathFile, " isn't path");
            continue;
        }

        mPaths[var.name] = var.value;
    }
}

//--------------------------------------------------------------------------------

boost::optional<const std::string&>
file::Path::getPath(const std::string& aName) noexcept
{
    boost::optional<const std::string&> result;

    auto& storage = getInstance().mPaths;

    auto it = storage.find(aName);
    if (it != storage.end()) result = it->second;
    else
    {
        dom::writeWarning("No such path (", aName, ")");
    }

    return result;
}

std::optional<std::string>
file::Path::getPath(const std::string& aFolder,
                    const std::string& aName,
                    bool aIsCritical) noexcept
{
    std::optional<std::string> result;

    auto folder = getPath(aFolder);
    if (folder.has_value())
    {
        result = folder.value() + aName;
    }
    else
    {
        if (aIsCritical)
        {
            dom::writeError("No such folder (", aFolder, ")");
        }
        else
        {
            dom::writeWarning("No such folder (", aFolder, ")");
        }
    }

    return result;
}

std::string
file::Path::getPathUnsafe(const std::string& aFolder,
                          const std::string& aName) noexcept
{
    std::string result;

    auto temp = getPath(aFolder, aName, true);
    if (temp.has_value())
    {
        result = temp.value();
    }

    return result;
}

std::optional<std::string>
file::Path::openFolder(const std::string& aName) noexcept
{
    std::optional<std::string> result;

    auto& ins = getInstance();

    auto temp = ins.getPath(aName);
    if (!temp.has_value())
    {
        dom::writeWarning("No such path (", aName, ")");

        std::string path = ins.mPaths["main"] + aName + "/";
        if (std::filesystem::create_directories(path))
        {
            result = ins.mPaths[aName] = path;
            dom::writeInfo("Creating path", aName, " (", path, ")");
        }
        else
        {
            dom::writeError("Can't creating path", aName, " (", path, ")");
        }
    }
    else
    {
        result = temp.value();
    }

    return result;
}

void
file::Path::addFoldersFrom(const std::string& aPath) noexcept
{
    getInstance().addAllFolders(aPath);
}

//--------------------------------------------------------------------------------

std::unordered_map<std::string, std::string>
file::Path::getContentMap(const std::string& aPath,
                          FileType aFIleType,
                          LevelType aLevelType) noexcept
{
    std::unordered_map<std::string, std::string> result;

    auto paths = getContent(aPath, aFIleType, aLevelType);
    for (auto&& i : paths)
    {
        bool flag = i.back() == '/';
        if (flag) i.pop_back();

        int num = i.size();
        while (num >= 0 && i[num] != '/') num--;
        auto name = i.substr(num + 1, i.size());
        if (flag) i.push_back('/');

        result[name] = std::move(i);
    }

    return result;
}

void
file::Path::addAllFolders(const std::string& aPath) noexcept
{
    auto temp = getContentMap(aPath, FileType::Folder, LevelType::Recursive);
    mPaths.insert(temp.begin(), temp.end());
}

std::vector<std::string>
file::Path::getContent(const std::string& aPath,
                       FileType aFIleType,
                       LevelType aLevelType) noexcept
{
    std::vector<std::string> result;
    if (aLevelType == LevelType::Current)
    {
        result =
            getContent<std::filesystem::directory_iterator>(aPath, aFIleType);
    }
    else
    {
        result = getContent<std::filesystem::recursive_directory_iterator>(
            aPath, aFIleType);
    }
    return result;
}
