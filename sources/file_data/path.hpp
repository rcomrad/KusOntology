#ifndef KUS_PATHS_HPP
#define KUS_PATHS_HPP

#include <boost/optional.hpp>

#include <string>
#include <unordered_map>

#include "domain/holy_trinity.hpp"

namespace file
{
class Path
{
public:
    HOLY_TRINITY_SINGLE(Path);
    static Path& getInstance() noexcept;
    void reset() noexcept;

    boost::optional<const std::string&> getPath(
        const std::string& aName) noexcept;
    static std::string generateConfigFolderPath() noexcept;

    void setPath(const std::string& aName, const std::string& aPath) noexcept;
    void setDefault(const std::string& aPath) noexcept;

    static std::vector<std::string> getAllContentPaths(
        const std::string& aPath) noexcept;
    static std::unordered_map<std::string, std::string> getAllContentPathsMap(
        const std::string& aPath) noexcept;

private:
    Path() noexcept;

    static std::string calculateMainPath(const std::string& aPath) noexcept;
    static std::string getExecutablePath() noexcept;

    std::unordered_map<std::string, std::string> mPaths;
};
} // namespace file

#endif // !KUS_PATHS_HPP
