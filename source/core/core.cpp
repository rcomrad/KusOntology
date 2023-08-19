#include "core.hpp"

#include <iostream>

#include "domain/log.hpp"

#include "file_data/file.hpp"
#include "file_data/path.hpp"
#include "file_data/variable_storage.hpp"

core::Core::Core() noexcept
{
}

void
core::Core::run() noexcept
{
    file::Path::clearFolder("output");

    auto names = file::Path::getContent(file::Path::getPathUnsafe("data", ""));
    for (auto& i : names)
    {
        process(i);
    }

    dom::writeInfo(webs[2].compare(webs[2]));
    dom::writeInfo(webs[2].compare(webs[3]));
    dom::writeInfo(webs[2].compare(webs[4]));
    dom::writeInfo(webs[2].compare(webs[5]));
}

void
core::Core::process(const std::string& aName) noexcept
{
    static int num = 0;
    webs[num]      = Web(aName);
    webs[num].print();
    webs[num].makeMatrix();
    ++num;
}
