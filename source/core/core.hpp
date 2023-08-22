#ifndef CORE_HPP
#define CORE_HPP

#include <string>
#include <unordered_map>

#include "domain/holy_trinity.hpp"

#include "web.hpp"

namespace core
{
class Core
{
public:
    Core() noexcept;
    HOLY_TRINITY_SINGLE(Core);

    void run() noexcept;
    void fill() noexcept;
    void print() noexcept;
    void check() noexcept;
    void process(const std::string& aName) noexcept;

    std::unordered_map<std::string, Web> mWebs;
};
} // namespace core

#endif // !CORE_HPP
