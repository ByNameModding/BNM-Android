#pragma once

//! Будёт удалён в релизе 2.0!!!!!
//! Will be removed in 2.0 release!!!!!

namespace BNM {
    struct Class;
    namespace Utils {
        struct DataIterator;
    }
    typedef Class LoadClass;
    typedef Utils::DataIterator DataIterator;
}
#define InitResolveFunc(x, y) BNM::InitFunc(x, BNM::GetExternMethod(y))