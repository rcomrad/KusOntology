#ifndef DECODER_HPP
#define DECODER_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "web.hpp"

namespace core
{
class Decoder
{
public:
    // Web() noexcept {};
    // Web(const std::string& aFileName) noexcept;
    // HOLY_TRINITY_NOCOPY(Web);

    static std::map<std::string, Node> processFile(
        const std::string& aFileName) noexcept;

private:
    Web mWeb;

    std::unordered_set<std::string> mVariables;

    int mCicleNumber;
    int mIfNumber;
    int mBlockNumber;
    int mExpressionNumber;

    static void preprocess(std::string& aData) noexcept;
    std::unordered_set<std::string> process(const std::string& aStr) noexcept;
    std::unordered_set<std::string> process(const char*& aStr) noexcept;

    //--------------------------------------------------------------------------------

    std::unordered_set<std::string> typeHandler(
        const std::string& aCommand, const std::string& aArgs) noexcept;
    std::unordered_set<std::string> declarationHandler(
        const std::string& aCommand, const std::string& aArgs) noexcept;
    std::unordered_set<std::string> funcHandler(
        const std::string& aCommand, const std::string& aArgs) noexcept;
    std::unordered_set<std::string> cicleHandler(
        const std::string& aCommand, const std::string& aArgs) noexcept;
    std::unordered_set<std::string> conditionHandler(
        const std::string& aCommand, const std::string& aArgs) noexcept;
    std::unordered_set<std::string> expressionHandler(
        const std::string& aCommand, const std::string& aArgs) noexcept;
    std::unordered_set<std::string> containerHandler(
        const std::string& aCommand, const std::string& aArgs) noexcept;

    //--------------------------------------------------------------------------------

    static std::unordered_map<std::string,
                              decltype(&core::Decoder::typeHandler)>
    getRouter() noexcept;

    //--------------------------------------------------------------------------------

    static std::string getName(decltype(mVariables.begin()) aPtr) noexcept;
    static std::string getName(
        std::unordered_map<std::string, std::string>::const_iterator
            aPtr) noexcept;
    template <typename T>
    bool usePart(const T& aCont,
                 const std::string& aBlockName,
                 const std::string& aType,
                 const std::string& aName) noexcept
    {
        bool result = false;
        auto it     = aCont.find(aName);
        if (it != aCont.end())
        {
            createEdge(aBlockName, getName(it), aType);
            result = true;
        }
        return result;
    }

    //--------------------------------------------------------------------------------

    static bool check(const char*& aStr) noexcept;
    static std::string getInsides(const std::string& aStr) noexcept;

    // std::string mExprInfo;
};
} // namespace core

#endif // !DECODER_HPP
