#pragma once
#include <android/log.h>
#include <Includes/obfuscate.h>

//! Write your Obfuscate
#define OBFUSCATE_BNM(str) str // const char *
#define OBFUSCATES_BNM(str) std::string(OBFUSCATE_BNM(str)) // std::string
#define OBFUSCATE_KEY_BNM(str, key) str // const char *
#define OBFUSCATES_KEY_BNM(str, key) std::string(OBFUSCATE_KEY_BNM(str, key)) // std::string

#define BNMTAG OBFUSCATE_BNM("ByNameModding")
#define LOGIBNM(...) ((void)__android_log_print(4,  BNMTAG, __VA_ARGS__))