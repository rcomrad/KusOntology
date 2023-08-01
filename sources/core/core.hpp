#ifndef CORE_HPP
#define CORE_HPP

#include "domain/holy_trinity.hpp"

namespace core
{
class Core
{
public:
    Core() noexcept;
    HOLY_TRINITY_SINGLE(Core);

    void run() noexcept;
};
} // namespace core

#endif // !CORE_HPP
