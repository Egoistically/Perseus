// thanks to shmoo and joeyjurjens for the usefull stuff under this comment.
#ifndef ANDROID_MOD_MENU_MACROS_H
#define ANDROID_MOD_MENU_MACROS_H

#if defined(__aarch64__) //Compile for arm64 lib only
#include <And64InlineHook/And64InlineHook.hpp>

#else //Compile for armv7 lib only. Do not worry about greyed out highlighting code, it still works
#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>

#endif

void hook(void *offset, void* ptr, void **orig)
{
#if defined(__aarch64__)
    A64HookFunction(offset, ptr, orig);
#else
    MSHookFunction(offset, ptr, orig);
#endif
}

#define HOOK(offset, ptr, orig) hook((void *)getAbsoluteAddress(targetLibName, string2Offset(OBFUSCATE(offset))), (void *)ptr, (void **)&orig)
#define HOOK_LIB(lib, offset, ptr, orig) hook((void *)getAbsoluteAddress(OBFUSCATE(lib), string2Offset(OBFUSCATE(offset))), (void *)ptr, (void **)&orig)

#define HOOK_NO_ORIG(offset, ptr) hook((void *)getAbsoluteAddress(targetLibName, string2Offset(OBFUSCATE(offset))), (void *)ptr, NULL)
#define HOOK_LIB_NO_ORIG(lib, offset, ptr) hook((void *)getAbsoluteAddress(OBFUSCATE(lib), string2Offset(OBFUSCATE(offset))), (void *)ptr, NULL)

#define GETSYM(lib, sym) dlsym(dlopen(lib, 4), OBFUSCATE(sym))

#define HOOKSYM(sym, ptr, org) hook(GETSYM(targetLibName, sym), (void *)ptr, (void **)&org)
#define HOOKSYM_LIB(lib, sym, ptr, org) hook(GETSYM(OBFUSCATE(lib), sym), (void *)ptr, (void **)&org)

#define HOOKSYM_NO_ORIG(sym, ptr)  hook(GETSYM(targetLibName, sym), (void *)ptr, NULL)
#define HOOKSYM_LIB_NO_ORIG(lib, sym, ptr) hook(GETSYM(OBFUSCATE(lib), sym), (void *)ptr, NULL)

#endif //ANDROID_MOD_MENU_MACROS_H