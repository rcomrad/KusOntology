#ifndef KUS_PATHS_HPP
#define KUS_PATHS_HPP

#include <boost/optional.hpp>

#include <optional>
#include <string>
#include <unordered_map>

#include "domain/holy_trinity.hpp"

namespace file
{
class Path
{
public:
    HOLY_TRINITY_SINGLE(Path);

    static boost::optional<const std::string&> getPath(
        const std::string& aName) noexcept;
    static std::optional<std::string> getPath(
        const std::string& aFolder, const std::string& aName) noexcept;
    static std::string getPathUnsafe(const std::string& aFolder,
                                     const std::string& aName) noexcept;

    // void setPath(const std::string& aName, const std::string& aPath)
    // noexcept;
    static void addFolder(const std::string& aPath) noexcept;

    static std::vector<std::string> getAllContentPaths(
        const std::string& aPath) noexcept;
    static std::unordered_map<std::string, std::string> getAllContentPathsMap(
        const std::string& aPath) noexcept;

    static std::unordered_map<std::string, std::string> getAllFoldersPathMap(
        const std::string& aPath) noexcept;

private:
    Path() noexcept;
    static Path& getInstance() noexcept;

    void reset() noexcept;

    static std::string calculateMainPath(const std::string& aPath) noexcept;
    static std::string getExecutablePath() noexcept;

    std::unordered_map<std::string, std::string> mPaths;
};
} // namespace file

#endif // !KUS_PATHS_HPP
