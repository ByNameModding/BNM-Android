#pragma once

//#define UNITY_VER 171 // 2017.1.x // il2cpp version: 24
#define UNITY_VER 172 // From 2017.2.x to 2017.4.x // il2cpp version: 24
//#define UNITY_VER 181 // 2018.1.x // il2cpp version: 24
//#define UNITY_VER 182 // 2018.2.x // il2cpp version: 24
//#define UNITY_VER 183 // From 2018.3.x to 2018.4.x // il2cpp version: 24.1
//#define UNITY_VER 191 // From 2019.1.x to 2019.2.x // il2cpp version: 24.2
//#define UNITY_VER 193 // 2019.3.x // il2cpp version: 24.3
//#define UNITY_VER 194 // 2019.4.x // il2cpp version: 24.3 and 24.4
//#define UNITY_VER 201 // 2020.1.x // il2cpp version: 24.3 and 24.4
//#define UNITY_VER 202 // From 2020.2.x to 2020.3.x (They are same) // il2cpp version: 27.1
//#define UNITY_VER 211 // 2021.1.x // il2cpp version: 27.2
//#define UNITY_VER 212 // 2021.2.x // il2cpp version: 27 and 27.2

#ifndef NDEBUG
//! DEBUG LOGS
#define BNM_DEBUG

//! INFO LOGS
#define BNM_INFO

//! ERROR LOGS
#define BNM_ERROR
#endif

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS // If you want try port for windows
#endif

//! Includes
#include <android/log.h>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <sstream>
#include <dlfcn.h>
#include <dlfcn.h>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <sstream>
#include <vector>
#include <any>
#include <setjmp.h>
#include <assert.h>
#include <utility>
#include <memory>
#include <algorithm>
#include <limits>
#include <vector>
#include "ByNameModding/macros.h"


#if UNITY_VER == 171
#include "ByNameModding/Il2CppTypeDefs/2017.1.h"
#elif UNITY_VER > 171 && UNITY_VER < 181
#include "ByNameModding/Il2CppTypeDefs/2017.4.h"
#elif UNITY_VER == 181
#include "ByNameModding/Il2CppTypeDefs/2018.1.h"
#elif UNITY_VER == 182
#include "ByNameModding/Il2CppTypeDefs/2018.2.h"
#elif UNITY_VER > 182 && UNITY_VER < 185
#include "ByNameModding/Il2CppTypeDefs/2018.4.h"
#elif UNITY_VER > 190 && UNITY_VER < 193
#include "ByNameModding/Il2CppTypeDefs/2019.2.h"
#elif UNITY_VER == 193
#include "ByNameModding/Il2CppTypeDefs/2019.3.h"
#elif UNITY_VER == 194
#include "ByNameModding/Il2CppTypeDefs/2019.4.h"
#elif UNITY_VER == 201
#include "ByNameModding/Il2CppTypeDefs/2020.1.h"
#elif UNITY_VER > 201 && UNITY_VER < 204
#include "ByNameModding/Il2CppTypeDefs/2020.3.h"
#elif UNITY_VER == 211
#include "ByNameModding/Il2CppTypeDefs/2021.1.h"
#elif UNITY_VER == 212
#include "ByNameModding/Il2CppTypeDefs/2021.2.h"
#else
#include "ByNameModding/Il2CppTypeDefs/2020.3.h"
#endif
using namespace std;
typedef Il2CppReflectionType MonoType;
#include "ByNameModding/ByNameModdingUtils.h"
#include "ByNameModding/Il2CppTypeDefs/il2cpp_mono_types.h"
#include "ByNameModding/LoadClass_Field.h"
#include "ByNameModding/BasicStructs/BasicStructs.h"
#include "ByNameModding/NewClass_structs.h"
#include "ByNameModding/NewClass.h"

// For System.Collections.Generic.Dictionary

// If game use .NET 4.x
using namespace NET4x;

// If game use .NET 3.5
// .NET 3.5 is deprecated but some old games use it
// using namespace NET35;