#include "core.hpp"

#include <iostream>

#include "domain/log.hpp"

#include "file_data/file.hpp"
#include "file_data/path.hpp"
#include "file_data/variable_storage.hpp"

#include "decoder.hpp"

core::Core::Core() noexcept
{
}

void
core::Core::run() noexcept
{
    file::Path::clearFolder("output");
    fill();
    print();
    check();
}

void
core::Core::fill() noexcept
{
    auto names = file::Path::getContent(file::Path::getPathUnsafe("data", ""));
    for (auto& i : names)
    {
        auto temp   = Decoder::processFile(i);
        auto name   = temp.getName();
        mWebs[name] = std::move(temp);
    }
}

void
core::Core::print() noexcept
{
    for (auto& i : mWebs)
    {
        i.second.makeMatrix();
        i.second.print();
    }
}

void
core::Core::check() noexcept
{
    auto baseName = file::VariableStorage::getInstance().getWordUnsafe("base");
    auto& base    = mWebs[baseName];
    for (auto& name : {
             "source_base.cpp",
             "source_type.cpp",
             "source_if.cpp",
             "source_iftypy.cpp",
             "source_indx_search.cpp",
             "source_push_back.cpp",
             "source_push_indx.cpp",
             "source_othe_sol.cpp",
             "simple0.cpp",
             "odd.cpp",
             "square.cpp",
             "pif.cpp",
         })
    {
        auto& i = *mWebs.find(name);
        dom::writeInfo(
            baseName, "<->", i.first, "=", base.compare(i.second, false, false),
            base.compare(i.second, false, true),
            base.compare(i.second, true, false), base.compare(i.second));
    }
}
