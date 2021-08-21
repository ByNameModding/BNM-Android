#pragma once

//! define - don't support floating pointer values
//#define IL2CPP_VERSION 240 //! From 2017 to 2018.2.x
#define IL2CPP_VERSION 241 //! 2018.3.x
//#define IL2CPP_VERSION 242 //! 2019.x
//#define IL2CPP_VERSION 270 //! 2020.x - 2021.x
//#define UNITY20 //! If unity 2020.x

//! Includes
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
#include <android/log.h>
#include <assert.h>
#include <utility>
#include <memory>
#include <algorithm>
#include <limits>
#include <vector>


#include "ByNameModding/Il2CppTypeDefs/il2cpptypes.h"
#if IL2CPP_VERSION == 240
#include "ByNameModding/Il2CppTypeDefs/24.0.h"
#elif IL2CPP_VERSION == 241
#include "ByNameModding/Il2CppTypeDefs/24.1.h"
#elif IL2CPP_VERSION == 242
#include "ByNameModding/Il2CppTypeDefs/24.2.h"
#elif IL2CPP_VERSION == 270
#include "ByNameModding/Il2CppTypeDefs/27.0.h"
#else
#include "ByNameModding/Il2CppTypeDefs/24.2.h"
#endif
typedef Il2CppReflectionType MonoType;
#include "ByNameModding/ByNameModdingUtils.h"
#include "ByNameModding/LoadClass_Field.h"
#include "ByNameModding/BasicStructs/BasicStructs.h"

