#pragma once

#include <cstdint>

/** @brief Author nickname namespace. Similar to Boost library.**/
namespace gabe {
    namespace bcgen {
        /**
         * @brief Get the version of the BCGen lib.
         * @return BCGen version.
        **/
        const char* get_version();

        /**
         * @brief Get the MAJOR version of the BCGen lib.
         * @return BCGen MAJOR version.
        **/
        uint16_t get_version_major();

        /**
         * @brief Get the MINOR version of the BCGen lib.
         * @return BCGen MINOR version.
        **/
        uint16_t get_version_minor();

        /**
         * @brief Get the PATCH version of the BCGen lib.
         * @return BCGen PATCH version.
        **/
        uint16_t get_version_patch();

        /**
         * @brief Get the TWEAK version of the BCGen lib.
         * @return BCGen TWEAK version.
        **/
        uint16_t get_version_tweak();
    }
}
