#pragma once

#include <string_view>
#include <vector>

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "DebugMessages.hpp"

namespace BNM {

#pragma pack(push, 1)

    struct Class;

    struct Image {
        inline constexpr Image() = default;
        inline Image(const BNM::IL2CPP::Il2CppImage *image) : _data((BNM::IL2CPP::Il2CppImage *) image) {}
        Image(const std::string_view &name);
        Image(const BNM::IL2CPP::Il2CppAssembly *assembly);

        std::vector<BNM::Class> GetClasses(bool includeInner = false) const;

        inline uint32_t GetClassesCount() const { return _data ? _data->typeCount : -1; }
        inline BNM::IL2CPP::Il2CppImage *GetInfo() const { return _data; }
        inline operator BNM::IL2CPP::Il2CppImage *() { return GetInfo(); };
        inline operator BNM::IL2CPP::Il2CppImage *() const { return GetInfo(); };


        inline operator bool() { return _data; }
        inline operator bool() const { return _data; }

#ifdef BNM_ALLOW_STR_METHODS
        inline std::string_view str() const {
            return _data ? _data->name : OBFUSCATE_BNM(DBG_BNM_MSG_Image_str_nullptr);
        } // Получить информацию о классе
#endif

        static std::vector<BNM::Image> GetImages();

        BNM::IL2CPP::Il2CppImage *_data{};
    };

#pragma pack(pop)

}