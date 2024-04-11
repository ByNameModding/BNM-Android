#pragma once

namespace BNM {
    namespace IL2CPP {
#if UNITY_VER == 56
#include "Il2CppHeaders/5.6.4f1.h"
#elif UNITY_VER == 171
#include "Il2CppHeaders/2017.1.h"
#elif UNITY_VER > 171 && UNITY_VER < 181
#include "Il2CppHeaders/2017.4.h"
#elif UNITY_VER == 181
#include "Il2CppHeaders/2018.1.h"
#elif UNITY_VER == 182
#include "Il2CppHeaders/2018.2.h"
#elif UNITY_VER > 182 && UNITY_VER < 185
#include "Il2CppHeaders/2018.4.h"
#elif UNITY_VER > 190 && UNITY_VER < 193
#include "Il2CppHeaders/2019.2.h"
#elif UNITY_VER == 193
#include "Il2CppHeaders/2019.3.h"
#elif UNITY_VER == 194
#include "Il2CppHeaders/2019.4.h"
#elif UNITY_VER == 201
#include "Il2CppHeaders/2020.1.h"
#elif UNITY_VER == 202
#include "Il2CppHeaders/2020.2.h"
#elif UNITY_VER == 203
#include "Il2CppHeaders/2020.3.h"
#elif UNITY_VER == 211
        #include "Il2CppHeaders/2021.1.h"
#elif UNITY_VER == 212
#include "Il2CppHeaders/2021.2.h"
#elif UNITY_VER == 213
#include "Il2CppHeaders/2021.3.h"
#elif UNITY_VER == 221
#include "Il2CppHeaders/2022.1.h"
#elif UNITY_VER >= 222 && UNITY_VER <= 223
#include "Il2CppHeaders/2022.2.h"
#elif UNITY_VER >= 231
#include "Il2CppHeaders/2023.1.h"
#else
#include "Il2CppHeaders/2023.1.h"
#endif
    }

    typedef IL2CPP::Il2CppReflectionType MonoType;
}
