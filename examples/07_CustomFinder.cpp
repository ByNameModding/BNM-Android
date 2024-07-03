#include <BNM/Loading.hpp>

// The example requires KittyMemory to work. After adding it, uncomment code.
// https://github.com/MJx0/KittyMemory
//! #include "KittyMemory.h"

// An example of replacing a method for searching for methods using KittyMemory.

/**
ElfScanner g_il2cppELF{};

void *KittyMemoryFinder(const char *name, void *data) {
    auto &scanner = *(ElfScanner *) data;
    // auto &scanner = g_il2cppELF;
    return (void *) scanner.findSymbol(name);
}
**/

void Example_07() {
    // Tells BNM to use KittyMemoryFinder in place of BNM_dlsym.
    //! BNM::Loading::SetMethodFinder(KittyMemoryFinder, (void *) &g_il2cppELF);

    // What if load happens later.
    BNM::Loading::AllowedLateInitHook();

    // Set g_il2cppELF in any way.
    // g_il2cppELF = ...;

    // Try load BNM
    auto result = BNM::Loading::TryLoadByUsersFinder();
    BNM_LOG_DEBUG("07: %d", (int) result);
}