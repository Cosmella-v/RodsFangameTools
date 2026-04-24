#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

inline bool isCompressedLevel(const std::string& s) {
    return s.rfind("H4sI", 0) == 0;
}
