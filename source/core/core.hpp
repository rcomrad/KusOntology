#ifndef CORE_HPP
#define CORE_HPP

#include <string>

#include "domain/holy_trinity.hpp"

namespace core
{
class Core
{
public:
    Core() noexcept;
    HOLY_TRINITY_SINGLE(Core);

    void run() noexcept;
    void process(const std::string& aName) noexcept;
};
} // namespace core

#endif // !CORE_HPP
