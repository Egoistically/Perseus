//
// Created by shirome on 19/04/2022.
//

#ifndef ANDROID_HOOKING_PATCHING_TEMPLATE_MAIN_STRUCTS_H
#define ANDROID_HOOKING_PATCHING_TEMPLATE_MAIN_STRUCTS_H

#include "lua/lstate.h"

typedef struct Il2CppDomain
{
    void* domain;
    void* setup;
    void* default_context;
    const char* friendly_name;
    uint32_t domain_id;
} Il2CppDomain;

typedef struct Il2CppAssembly
{
    void* imageIndex;
    void* customAttributeIndex;
    int32_t referencedAssemblyStart;
    int32_t referencedAssemblyCount;
    void* aname;
} Il2CppAssembly;

typedef struct Il2CppImage
{
    const char* name;
    void* assemblyIndex;
    void* typeStart;
    uint32_t typeCount;
    void* entryPointIndex;
    void* nameToClassHashTable;
    uint32_t token;
} Il2CppImage;

typedef void(*Il2CppMethodPointer)();

typedef struct MethodInfo
{
    Il2CppMethodPointer* methodPointer;
    void* invoker_method;
    const char* name;
    void* declaring_type;
    const void* return_type;
    const void* parameters;

    union
    {
        const void* rgctx_data;
        const void* methodDefinition;
    };

    union
    {
        const void* genericMethod;
        const void* genericContainer;
    };

    void* customAttributeIndex;
    uint32_t token;
    uint16_t flags;
    uint16_t iflags;
    uint16_t slot;
    uint8_t parameters_count;
    uint8_t is_generic : 1;
    uint8_t is_inflated : 1;
} MethodInfo;

// define il2cpp functions
Il2CppDomain* (*il2cpp_domain_get)();
Il2CppAssembly* (*il2cpp_domain_assembly_open)(void* domain, char* name);
Il2CppImage* (*il2cpp_assembly_get_image)(void* assembly);
void* (*il2cpp_class_from_name)(void* image, char* namespaze, char* name);
MethodInfo* (*il2cpp_class_get_method_from_name)(void* klass, char* name, int argsCount);
Il2CppString* (*il2cpp_string_new)(char* str);

// mod structures
struct Config {
    struct AAircraft {
        bool Enabled = false;
        std::string Accuracy = "false";
        std::string AccuracyGrowth = "false";
        std::string AttackPower = "false";
        std::string AttackPowerGrowth = "false";
        std::string CrashDamage = "false";
        std::string Hp = "false";
        std::string HpGrowth = "false";
        std::string Speed = "false";
    };

    struct EEnemies {
        bool Enabled = false;
        std::string AntiAir = "false";
        std::string AntiAirGrowth = "false";
        std::string AntiSubmarine = "false";
        std::string Armor = "false";
        std::string ArmorGrowth = "false";
        std::string Cannon = "false";
        std::string CannonGrowth = "false";
        std::string Evasion = "false";
        std::string EvasionGrowth = "false";
        std::string Hit = "false";
        std::string HitGrowth = "false";
        std::string Hp = "false";
        std::string HpGrowth = "false";
        std::string Luck = "false";
        std::string LuckGrowth = "false";
        std::string Reload = "false";
        std::string ReloadGrowth = "false";
        bool RemoveBuffs = false;
        bool RemoveEquipment = false;
        bool RemoveSkill = false;
        std::string Speed = "false";
        std::string SpeedGrowth = "false";
        std::string Torpedo = "false";
        std::string TorpedoGrowth = "false";
    };

    struct MMisc {
        bool Enabled = false;
        bool ExerciseGodmode = false;
        bool FastStageMovement = false;
    };

    struct SSkins {
        bool Enabled = false;
    };

    struct WWeapons {
        bool Enabled = false;
        std::string Damage = "false";
        std::string ReloadMax = "false";
    };

    AAircraft Aircraft;
    EEnemies Enemies;
    SSkins Skins;
    MMisc Misc;
    WWeapons Weapons;
    bool Valid = true;
};

std::vector<std::string> _default{
        OBFUSCATE("# [*] Delete the file to reset it. Restart the game to apply any changes."),
        OBFUSCATE("# [*] Options can be one of the following types:"),
        OBFUSCATE("#      [1] Bool - e.g. \"true\" or \"false\" - Keys: All \"Enabled\"s, ExerciseGodmode, FastStageMovement, RemoveBuffs, RemoveEquipment and RemoveSkill."),
        OBFUSCATE("#      [2] Whole numbers or false to disable changes - If you need an example you're five - Keys: All the ones not mentioned above."),
        OBFUSCATE("# [*] \"Enabled\"s apply to the entirety of their section, if they're disabled everything will be."),
        OBFUSCATE("# [*] If the game crashes, this file is most likely misconfigured."),
        OBFUSCATE(""),
        OBFUSCATE("original repo: github.com/Egoistically/Perseus."),
        OBFUSCATE(""),
        OBFUSCATE("[Aircraft]"),
        OBFUSCATE("Enabled=false"),
        OBFUSCATE("Accuracy=false"),
        OBFUSCATE("AccuracyGrowth=false"),
        OBFUSCATE("AttackPower=false"),
        OBFUSCATE("AttackPowerGrowth=false"),
        OBFUSCATE("CrashDamage=false"),
        OBFUSCATE("Hp=false"),
        OBFUSCATE("HpGrowth=false"),
        OBFUSCATE("Speed=false"),
        OBFUSCATE(""),
        OBFUSCATE("[Enemies]"),
        OBFUSCATE("Enabled=false"),
        OBFUSCATE("AntiAir=false"),
        OBFUSCATE("AntiAirGrowth=false"),
        OBFUSCATE("AntiSubmarine=false"),
        OBFUSCATE("Armor=false"),
        OBFUSCATE("ArmorGrowth=false"),
        OBFUSCATE("Cannon=false"),
        OBFUSCATE("CannonGrowth=false"),
        OBFUSCATE("Evasion=false"),
        OBFUSCATE("EvasionGrowth=false"),
        OBFUSCATE("Hit=false"),
        OBFUSCATE("HitGrowth=false"),
        OBFUSCATE("Hp=false"),
        OBFUSCATE("HpGrowth=false"),
        OBFUSCATE("Luck=false"),
        OBFUSCATE("LuckGrowth=false"),
        OBFUSCATE("Reload=false"),
        OBFUSCATE("ReloadGrowth=false"),
        OBFUSCATE("RemoveBuffs=false"),
        OBFUSCATE("RemoveEquipment=false"),
        OBFUSCATE("RemoveSkill=false"),
        OBFUSCATE("Speed=false"),
        OBFUSCATE("SpeedGrowth=false"),
        OBFUSCATE("Torpedo=false"),
        OBFUSCATE("TorpedoGrowth=false"),
        OBFUSCATE(""),
        OBFUSCATE("[Misc]"),
        OBFUSCATE("Enabled=false"),
        OBFUSCATE("ExerciseGodmode=false"),
        OBFUSCATE("FastStageMovement=false"),
        OBFUSCATE(""),
        OBFUSCATE("[Skins]"),
        OBFUSCATE("Enabled=false"),
        OBFUSCATE(""),
        OBFUSCATE("[Weapons]"),
        OBFUSCATE("Enabled=false"),
        OBFUSCATE("Damage=false"),
        OBFUSCATE("ReloadMax=false")
};

#endif //ANDROID_HOOKING_PATCHING_TEMPLATE_MAIN_STRUCTS_H
