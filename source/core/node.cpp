#include "node.hpp"

bool
core::Node::operator==(const Node& aOther) const noexcept
{
    return mType == aOther.mType && mLeaves == aOther.mLeaves;
}
