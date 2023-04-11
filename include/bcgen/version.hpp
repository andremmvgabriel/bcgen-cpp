#pragma once

#include <cstdint>

namespace gabe {
    namespace bcgen {
        const char* get_version();
        uint16_t get_version_major();
        uint16_t get_version_minor();
        uint16_t get_version_patch();
        uint16_t get_version_tweak();
    }
}
