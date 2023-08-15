#include "core.hpp"

#include <iostream>

#include "domain/log.hpp"

#include "file_data/file.hpp"
#include "file_data/path.hpp"
#include "file_data/variable_storage.hpp"

#include "web.hpp"

core::Core::Core() noexcept
{
}

void
core::Core::run() noexcept
{
    Web(file::VariableStorage::getInstance().getWordUnsafe("source0")).print();

    // auto w1 = Web::processFile(
    //     file::VariableStorage::getInstance().getWordUnsafe("source0"));
    // auto w2 = Web::processFile(
    //     file::VariableStorage::getInstance().getWordUnsafe("source1"));

    // dom::writeInfo(w1 == w2);
}
