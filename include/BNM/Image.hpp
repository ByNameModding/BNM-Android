#pragma once

#include <string_view>
#include <vector>

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "DebugMessages.hpp"

// NOLINTBEGIN
namespace BNM {
    struct Class;

    /**
        @brief Class for working with il2cpp images.

        This class allows to get classes from image.
    */
    struct Image {
        /**
            @brief Create empty image.
        */
        inline constexpr Image() = default;

        /**
            @brief Create image from il2cpp image.
            @param image Il2cpp image
        */
        inline Image(const BNM::IL2CPP::Il2CppImage *image) : _data((BNM::IL2CPP::Il2CppImage *) image) {}

        /**
            @brief Create image from name.

            Tries to find image by its name.

            @param name Image name
        */
        Image(const std::string_view &name);

        /**
            @brief Create image from il2cpp assembly.
            @param assembly Il2cpp assembly
        */
        Image(const BNM::IL2CPP::Il2CppAssembly *assembly);

        /**
             @brief Get all classes of target image.

             Gets all classes and inner classes of target image.

             @param includeInner Should include inner classes

             @return Vector of classes if anyone of them exists.
         */
        [[nodiscard]] std::vector<BNM::Class> GetClasses(bool includeInner = false) const;

        /**
             @brief Get classes count.
             @return Classes count if image is valid, otherwise zero.
         */
        [[nodiscard]] inline uint32_t GetClassesCount() const { return _data ? _data->typeCount : -1; }

        /**
            @brief Get Il2CppImage.
            @return Il2CppImage
        */
        [[nodiscard]] inline BNM::IL2CPP::Il2CppImage *GetInfo() const { return _data; }
        inline operator BNM::IL2CPP::Il2CppImage *() const { return GetInfo(); };

        /**
            @brief Check if image is valid.
            @return State of image
        */
        [[nodiscard]] inline bool IsValid() const { return _data != nullptr; }

        inline operator bool() const { return IsValid(); }

#ifdef BNM_ALLOW_STR_METHODS

        /**
            @brief Get image name.
            @return Image name or "Dead image"
        */
        [[nodiscard]] inline std::string_view str() const {
            return _data ? _data->name : BNM_OBFUSCATE(DBG_BNM_MSG_Image_str_nullptr);
        }
#endif

        /**
             @brief Get all images.
             @return Vector of all images
         */
        static std::vector<BNM::Image> GetImages();

        BNM::IL2CPP::Il2CppImage *_data{};
    };
}
// NOLINTEND