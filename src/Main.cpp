#include <list>
#include <vector>
#include <cstring>
#include <map>
#include <sstream>
#include <thread>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "Includes/Toast.hpp"

//Target lib here
#define targetLibName OBFUSCATE("libil2cpp.so")

#include "Includes/Macros.h"
#include "Structs.h"

bool exec = false;
std::string configPath;
std::string skinPath;
std::map<int, int> skins;
Config config;
Il2CppImage *image;

void crash() {
    int *p = 0;
    *p = 0;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}

bool writeConfigFile() {
    std::ofstream configFile(configPath);

    if (configFile.is_open()) {
        for (const std::string &line: _default) {
            configFile << line << "\n";
        }
        configFile.close();
        return true;
    }
    return false;
}

bool writeSkinsFile() {
    std::ofstream skinsFile(skinPath);

    if (skinsFile.is_open()) {
        for (auto const &kv: skins) {
            skinsFile << kv.first << ":" << kv.second << "\n";
        }
        skinsFile.close();
        return true;
    }
    return false;
}

bool readSkinsFile() {
    std::vector<std::string> fileLines;
    std::string l;
    std::ifstream skinsFile(skinPath);

    if (!skinsFile.is_open()) {
        return false;
    }

    while (getline(skinsFile, l)) {
        if (l.length() < 3) {
            continue;
        }
        fileLines.push_back(l);
    }
    skinsFile.close();

    for (const std::string &line: fileLines) {
        std::vector<std::string> splitLines = split(line, ':');
        int shipId = std::stoi(splitLines[0]);
        int skinId = std::stoi(splitLines[1]);
        skins.insert(std::pair<int, int>(shipId, skinId));
    }

    return true;
}

bool isEnabled(const std::string &param) {
    return param != "false";
}

int getValue(const std::string &param) {
    return std::stoi(param);
}

void checkHeader(const std::string &line, const std::string &header) {
    if (line != header) {
        config.Valid = false;
    }
}

bool getKeyEnabled(const std::string &line, const std::string &key) {
    std::vector<std::string> splitLine = split(line, '=');
    std::string value = splitLine[1];
    if (value != "true" && value != "false" || splitLine[0] != key) {
        config.Valid = false;
        return false;
    }
    return isEnabled(value);
}

std::string getKeyValue(const std::string &line, const std::string &key) {
    std::vector<std::string> splitLine = split(line, '=');
    std::string value = splitLine[1];
    try {
        if (splitLine[0] != key) {
            throw 1;
        }
        if (value == "false") {
            return value;
        }
        if (getValue(value) < 0) {
            throw 1;
        }
    }
    catch (...) {
        config.Valid = false;
        return "false";
    }
    return value;
}

void loadil2cppfuncs() {
    // populate all il2cpp functions
    il2cpp_domain_get = (Il2CppDomain *(*)()) GETSYM(targetLibName, "il2cpp_domain_get");
    il2cpp_domain_assembly_open = (Il2CppAssembly *(*)(void *, char *)) GETSYM(targetLibName,
                                                                               "il2cpp_domain_assembly_open");
    il2cpp_assembly_get_image = (Il2CppImage *(*)(void *)) GETSYM(targetLibName,
                                                                  "il2cpp_assembly_get_image");
    il2cpp_class_from_name = (void *(*)(void *, char *, char *)) GETSYM(targetLibName,
                                                                        "il2cpp_class_from_name");
    il2cpp_class_get_method_from_name = (MethodInfo *(*)(void *, char *, int)) GETSYM(
            targetLibName, "il2cpp_class_get_method_from_name");
    il2cpp_string_new = (Il2CppString *(*)(char *)) GETSYM(targetLibName,
                                                           "il2cpp_string_new");

    // call the functions necessary to get the image
    Il2CppDomain *domain = il2cpp_domain_get();
    Il2CppAssembly *assembly = il2cpp_domain_assembly_open(domain, OBFUSCATE("Assembly-CSharp"));
    image = il2cpp_assembly_get_image(assembly);
}

Il2CppMethodPointer *getFunctionAddress(char *namespaze, char *klass, char *method) {
    void *iklass = il2cpp_class_from_name(image, namespaze, klass);
    MethodInfo *imethod = il2cpp_class_get_method_from_name(iklass, method, -1);
    return imethod->methodPointer;
}

#define GETLUAFUNC(method) getFunctionAddress(OBFUSCATE("LuaInterface"), OBFUSCATE("LuaDLL"), OBFUSCATE(method))
#define STR(str) il2cpp_string_new(OBFUSCATE(str))

void loadluafuncs() {
    // populate lua funcs
    lua_newthread = (lua_State *(*)(lua_State *)) GETLUAFUNC("lua_newthread");
    lua_getfield = (void (*)(lua_State *, int, Il2CppString *)) GETLUAFUNC("lua_getfield");
    lua_gettable = (void (*)(lua_State *, int)) GETLUAFUNC("lua_gettable");
    lua_setfield = (void (*)(lua_State *, int, Il2CppString *)) GETLUAFUNC("lua_setfield");
    lua_objlen = (size_t(*)(lua_State *, int)) GETLUAFUNC("lua_objlen");
    lua_pushnil = (void (*)(lua_State *)) GETLUAFUNC("lua_pushnil");
    lua_createtable = (void (*)(lua_State *, int, int)) GETLUAFUNC("lua_createtable");
    lua_pushnumber = (void (*)(lua_State *, double)) GETLUAFUNC("lua_pushnumber");
    lua_pushboolean = (void (*)(lua_State *, int)) GETLUAFUNC("lua_pushboolean");
    lua_settop = (void (*)(lua_State *, int)) GETLUAFUNC("lua_settop");
    lua_next = (int (*)(lua_State *, int)) GETLUAFUNC("lua_next");
    lua_tonumber = (double (*)(lua_State *, int)) GETLUAFUNC("lua_tonumber");
    lua_type = (int (*)(lua_State *, int)) GETLUAFUNC("lua_type");
    lua_pushcclosure = (void (*)(lua_State *, lua_CFunction, int)) GETLUAFUNC(
            "lua_pushcclosure");
    lua_pcall = (int (*)(lua_State *, int, int, int)) GETLUAFUNC("lua_pcall");
    lua_insert = (void (*)(lua_State *, int)) GETLUAFUNC("lua_insert");
    lua_pushvalue = (void (*)(lua_State *, int)) GETLUAFUNC("lua_pushvalue");
    lua_pushstring = (void (*)(lua_State *, Il2CppString *)) GETLUAFUNC("lua_pushstring");
    lua_remove = (void (*)(lua_State *, int)) GETLUAFUNC("lua_remove");
    lua_gettop = (int (*)(lua_State *)) GETLUAFUNC("lua_gettop");
}

void replaceAttributeN(lua_State *L, Il2CppString *attribute, int number) {
    lua_pushnumber(L, number);
    lua_setfield(L, -2, attribute);
}

void emptyAttributeT(lua_State *L, Il2CppString *attribute) {
    lua_newtable(L);
    lua_setfield(L, -2, attribute);
}

std::vector<int> getTableIds(lua_State *L) {
    std::vector<int> tableIds;

    // get table "all", where all ids are stored
    lua_getfield(L, -1, STR("all"));

    // loop it
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        tableIds.push_back((int) lua_tonumber(L, -1));
        lua_pop(L, 1);
    }

    // pop "all" table
    lua_pop(L, 1);
    return tableIds;
}

void modAircraft(lua_State *L) {
    lua_getfield(L, -1, STR("aircraft_template"));

    std::vector<int> aircraft = getTableIds(L);

    for (const int &aircraftId: aircraft) {
        lua_pushnumber(L, aircraftId);
        lua_gettable(L, -2);

        if (isEnabled(config.Aircraft.Accuracy)) {
            replaceAttributeN(L, STR("accuracy"), getValue(config.Aircraft.Accuracy));
        }
        if (isEnabled(config.Aircraft.AccuracyGrowth)) {
            replaceAttributeN(L, STR("ACC_growth"), getValue(config.Aircraft.AccuracyGrowth));
        }
        if (isEnabled(config.Aircraft.AttackPower)) {
            replaceAttributeN(L, STR("attack_power"), getValue(config.Aircraft.AttackPower));
        }
        if (isEnabled(config.Aircraft.AttackPowerGrowth)) {
            replaceAttributeN(L, STR("AP_growth"),
                              getValue(config.Aircraft.AttackPowerGrowth));
        }
        if (isEnabled(config.Aircraft.CrashDamage)) {
            replaceAttributeN(L, STR("crash_DMG"), getValue(config.Aircraft.CrashDamage));
        }
        if (isEnabled(config.Aircraft.Hp)) {
            replaceAttributeN(L, STR("max_hp"), getValue(config.Aircraft.Hp));
        }
        if (isEnabled(config.Aircraft.HpGrowth)) {
            replaceAttributeN(L, STR("hp_growth"), getValue(config.Aircraft.HpGrowth));
        }
        if (isEnabled(config.Aircraft.Speed)) {
            replaceAttributeN(L, STR("speed"), getValue(config.Aircraft.Speed));
        }

        lua_pop(L, 1);
    }

    lua_pop(L, 1);
}

int hookBUAddBuff(lua_State *L);

void modEnemies(lua_State *L) {
    lua_getfield(L, -1, STR("enemy_data_statistics"));

    std::vector<int> enemyShips = getTableIds(L);

    for (const int &enemyId: enemyShips) {
        lua_pushnumber(L, enemyId);
        lua_gettable(L, -2);

        if (isEnabled(config.Enemies.AntiAir)) {
            replaceAttributeN(L, STR("antiaircraft"), getValue(config.Enemies.AntiAir));
        }
        if (isEnabled(config.Enemies.AntiAirGrowth)) {
            replaceAttributeN(L, STR("antiaircraft_growth"),
                              getValue(config.Enemies.AntiAirGrowth));
        }
        if (isEnabled(config.Enemies.AntiSubmarine)) {
            replaceAttributeN(L, STR("antisub"), getValue(config.Enemies.AntiSubmarine));
        }
        if (isEnabled(config.Enemies.Armor)) {
            replaceAttributeN(L, STR("armor"), getValue(config.Enemies.Armor));
        }
        if (isEnabled(config.Enemies.ArmorGrowth)) {
            replaceAttributeN(L, STR("armor_growth"), getValue(config.Enemies.ArmorGrowth));
        }
        if (isEnabled(config.Enemies.Cannon)) {
            replaceAttributeN(L, STR("cannon"), getValue(config.Enemies.Cannon));
        }
        if (isEnabled(config.Enemies.CannonGrowth)) {
            replaceAttributeN(L, STR("cannon_growth"), getValue(config.Enemies.CannonGrowth));
        }
        if (isEnabled(config.Enemies.Evasion)) {
            replaceAttributeN(L, STR("dodge"), getValue(config.Enemies.Evasion));
        }
        if (isEnabled(config.Enemies.EvasionGrowth)) {
            replaceAttributeN(L, STR("dodge_growth"), getValue(config.Enemies.EvasionGrowth));
        }
        if (isEnabled(config.Enemies.Hit)) {
            replaceAttributeN(L, STR("hit"), getValue(config.Enemies.Hit));
        }
        if (isEnabled(config.Enemies.HitGrowth)) {
            replaceAttributeN(L, STR("hit_growth"), getValue(config.Enemies.HitGrowth));
        }
        if (isEnabled(config.Enemies.Hp)) {
            replaceAttributeN(L, STR("durability"), getValue(config.Enemies.Hp));
        }
        if (isEnabled(config.Enemies.HpGrowth)) {
            replaceAttributeN(L, STR("durability_growth"), getValue(config.Enemies.HpGrowth));
        }
        if (isEnabled(config.Enemies.Luck)) {
            replaceAttributeN(L, STR("luck"), getValue(config.Enemies.Luck));
        }
        if (isEnabled(config.Enemies.LuckGrowth)) {
            replaceAttributeN(L, STR("luck_growth"), getValue(config.Enemies.LuckGrowth));
        }
        if (isEnabled(config.Enemies.Reload)) {
            replaceAttributeN(L, STR("reload"), getValue(config.Enemies.Reload));
        }
        if (isEnabled(config.Enemies.ReloadGrowth)) {
            replaceAttributeN(L, STR("reload_growth"), getValue(config.Enemies.ReloadGrowth));
        }
        if (config.Enemies.RemoveEquipment) { emptyAttributeT(L, STR("equipment_list")); }
        if (isEnabled(config.Enemies.Speed)) {
            replaceAttributeN(L, STR("speed"), getValue(config.Enemies.Speed));
        }
        if (isEnabled(config.Enemies.SpeedGrowth)) {
            replaceAttributeN(L, STR("speed_growth"), getValue(config.Enemies.SpeedGrowth));
        }
        if (isEnabled(config.Enemies.Torpedo)) {
            replaceAttributeN(L, STR("torpedo"), getValue(config.Enemies.Torpedo));
        }
        if (isEnabled(config.Enemies.TorpedoGrowth)) {
            replaceAttributeN(L, STR("torpedo_growth"),
                              getValue(config.Enemies.TorpedoGrowth));
        }

        lua_pop(L, 1);
    }

    // pop enemy_data_statistics
    lua_pop(L, 1);

    if (config.Enemies.RemoveSkill) {
        lua_getfield(L, -1, STR("enemy_data_skill"));

        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            if (lua_istable(L, -1)) {
                emptyAttributeT(L, STR("skill_list"));
                replaceAttributeN(L, STR("is_repeat"), 0);
            }
            lua_pop(L, 1);
        }

        lua_pop(L, 1);
    }
    if (config.Enemies.RemoveBuffs) {
        lua_getglobal(L, STR("ys"));
        lua_getfield(L, -1, STR("Battle"));
        lua_getfield(L, -1, STR("BattleUnit"));
        lua_getfield(L, -1, STR("AddBuff"));
        lua_setfield(L, -2, STR("oldAddBuff"));
        lua_pushcfunction(L, hookBUAddBuff);
        lua_setfield(L, -2, STR("AddBuff"));
        lua_pop(L, 3);
    }
}

int nilFunc(lua_State *L) {
    return 0;
}

int hookSetShipSkinCommand(lua_State *L);

int hookSFVSetSkinList(lua_State *L);

int wrapShipCtor(lua_State *L);

int hookCommitCombat(lua_State *L);

int hookCommitTrybat(lua_State *L);

void modSkins(lua_State *L) {
    // replace SetShipSkinCommand.execute with its hook
    lua_getglobal(L, STR("SetShipSkinCommand"));
    lua_pushcfunction(L, hookSetShipSkinCommand);
    lua_setfield(L, -2, STR("execute"));
    lua_pop(L, 1);

    // replace ShipFashionView.SetSkinList with its hook
    lua_getglobal(L, STR("ShipFashionView"));
    lua_pushcfunction(L, hookSFVSetSkinList);
    lua_setfield(L, -2, STR("SetSkinList"));
    lua_pop(L, 1);

    // doesn't work anymore
//    lua_getglobal( L, STR( "SwichSkinLayer" ));
//    lua_pushcfunction( L, hookSSLSetSkinList );
//    lua_setfield( L, -2, STR( "setSkinList" ));
//    lua_pop( L, 1 );

    // rename Ship's New function (its Ctor) to oldCtor
    // and set New to wrapShipCtor.
    lua_getglobal(L, STR("Ship"));
    lua_getfield(L, -1, STR("New"));
    lua_setfield(L, -2, STR("oldCtor"));
    lua_pushcfunction(L, wrapShipCtor);
    lua_setfield(L, -2, STR("New"));
    lua_pop(L, 1);

    // replace ShipSkin's SKIN_TYPE_NOT_HAVE_HIDE (by default 4) to 10,
    // so all its comparisons pass through, thus making "censored" skins visible
    lua_getglobal(L, STR("ShipSkin"));
    replaceAttributeN(L, STR("SKIN_TYPE_NOT_HAVE_HIDE"), 10);
    lua_pop(L, 1);

    if (!readSkinsFile()) {
        return;
    }
}

void modMisc(lua_State *L) {
    if (config.Misc.ExerciseGodmode) {
        lua_getfield(L, -1, STR("ConvertedBuff"));
        lua_getfield(L, -1, STR("buff_19"));
        lua_pushnumber(L, 0);
        lua_gettable(L, -2);
        lua_getfield(L, -1, STR("effect_list"));
        lua_pushnumber(L, 1);
        lua_gettable(L, -2);
        lua_getfield(L, -1, STR("arg_list"));
        replaceAttributeN(L, STR("number"), -1);
        lua_pop(L, 6);
    }
    if (config.Misc.FastStageMovement) {
        lua_getglobal(L, STR("ChapterConst"));
        replaceAttributeN(L, STR("ShipStepDuration"), 0);
        replaceAttributeN(L, STR("ShipStepQuickPlayScale"), 0);
        lua_pop(L, 1);
    }
}

void modWeapons(lua_State *L) {
    lua_getfield(L, -1, STR("weapon_property"));

    std::vector<int> weapons = getTableIds(L);

    for (int weaponId: weapons) {
        lua_pushnumber(L, weaponId);
        lua_gettable(L, -2);

        if (isEnabled(config.Weapons.Damage)) {
            replaceAttributeN(L, STR("damage"), getValue(config.Weapons.Damage));
        }
        if (isEnabled(config.Weapons.ReloadMax)) {
            replaceAttributeN(L, STR("reload_max"), getValue(config.Weapons.ReloadMax));
        }

        lua_pop(L, 1);
    }

    lua_pop(L, 1);
}

int hookSetShipSkinCommand(lua_State *L) {
    // calls getBody on slot1, returning a table with shipId and skinId
    lua_getfield(L, 2, STR("getBody"));
    lua_insert(L, -2);
    lua_pcall(L, 1, 1, 0);

    // assigns skinId to a variable and pops it, leaving the stack intact
    lua_getfield(L, -1, STR("skinId"));
    int skinId = (int) lua_tonumber(L, -1);
    lua_pop(L, 1);

    // assigns shipId to a variable and pops it, leaving the stack intact
    lua_getfield(L, -1, STR("shipId"));
    int shipId = (int) lua_tonumber(L, -1);
    lua_pop(L, 1);

    // calls getProxy(BayProxy), which returns a table with its functions
    lua_getglobal(L, STR("getProxy"));
    lua_getglobal(L, STR("BayProxy"));
    lua_pcall(L, 1, 1, 0);

    // gets the function getShipById from the proxy, shifts it to the left
    // so that the proxy table from before is passed as its first argument,
    // pushes shipId and calls the function.
    // returns the ship table.
    lua_getfield(L, -1, STR("getShipById"));
    lua_insert(L, -2);
    lua_pushnumber(L, shipId);
    lua_pcall(L, 2, 1, 0);

    // if skinId is 0, which means default skin
    if (skinId == 0) {
        // gets the getConfig function from the ship table, pushes a copy
        // of the ship table onto the top of the stack, as it's needed as first argument,
        // and pushes "skin_id" as second argument, then calls the function.
        // returns the ship's default skin id, which replaces the 0 of skinId.
        lua_getfield(L, -1, STR("getConfig"));
        lua_pushvalue(L, -2);
        lua_pushstring(L, STR("skin_id"));
        lua_pcall(L, 2, 1, 0);
        skinId = (int) lua_tonumber(L, -1);
        lua_pop(L, 1);
    }

    // replaces the ship's skinId attribute with the one saved
    lua_pushnumber(L, skinId);
    lua_setfield(L, -2, STR("skinId"));

    // calls getProxy(BayProxy) again
    lua_getglobal(L, STR("getProxy"));
    lua_getglobal(L, STR("BayProxy"));
    lua_pcall(L, 1, 1, 0);

    // gets the function updateShip, shifts it to the left as done before,
    // pushes a copy of the ship table onto the top of the stack, and calls
    // the function.
    lua_getfield(L, -1, STR("updateShip"));
    lua_insert(L, -2);
    lua_pushvalue(L, -3);
    lua_pcall(L, 2, 0, 0);

    // calls getProxy(PlayerProxy)
    lua_getglobal(L, STR("getProxy"));
    lua_getglobal(L, STR("PlayerProxy"));
    lua_pcall(L, 1, 1, 0);

    // gets the function sendNotification, shifts it to the left,
    // gets the global SetShipSkinCommand to get SKIN_UPDATED, then removes
    // said global, creates a new table, pushes a copy of the ship table onto
    // the top of the stack to assign it to the key "ship" of the table,
    // and then calls the function.
    // before calling it, the end of the stack is:
    // (ship) [sendNotification] [PlayerProxy] [SKIN_UPDATED] [table]
    lua_getfield(L, -1, STR("sendNotification"));
    lua_insert(L, -2);
    lua_getglobal(L, STR("SetShipSkinCommand"));
    lua_getfield(L, -1, STR("SKIN_UPDATED"));
    lua_remove(L, -2);
    lua_newtable(L);
    lua_pushvalue(L, -5);
    lua_setfield(L, -2, STR("ship"));
    lua_pcall(L, 3, 0, 0);

    // pops the ship table, as it's not needed anymore
    lua_pop(L, 1);

    // gets pg.TipsMgr's GetInstance function, removes pg.TipsMgr from the stack
    // and calls the function.
    lua_getglobal(L, STR("pg"));
    lua_getfield(L, -1, STR("TipsMgr"));
    lua_getfield(L, -1, STR("GetInstance"));
    lua_remove(L, -2);
    lua_remove(L, -2);
    lua_pcall(L, 0, 1, 0);

    // gets function ShowTips from GetInstance's table, shifts it to the left,
    // gets the function i18n and calls it with "ship_set_skin_success", then passes it
    // as second argument to ShowTips (first being "this") and calls it.
    lua_getfield(L, -1, STR("ShowTips"));
    lua_insert(L, -2);
    lua_getglobal(L, STR("i18n"));
    lua_pushstring(L, STR("ship_set_skin_success"));
    lua_pcall(L, 1, 1, 0);
    lua_pcall(L, 2, 0, 0);

    auto found = skins.find(shipId);
    if (skins.find(shipId) == skins.end()) {
        skins.insert(std::pair<int, int>(shipId, skinId));
    } else {
        skins.find(shipId)->second = skinId;
    }

    if (!writeSkinsFile()) {
        crash();
    }

    return 0;
}

int hookSFVSetSkinList(lua_State *L) {
    // sets ShipFashionView.skinList to pg.ship_skin_template.all,
    // where all skin indexes are stored.
    lua_getglobal(L, STR("pg"));
    lua_getfield(L, -1, STR("ship_skin_template"));
    lua_getfield(L, -1, STR("all"));
    lua_remove(L, -2);
    lua_remove(L, -2);
    lua_setfield(L, 1, STR("skinList"));

    return 0;
}

int wrapShipCtor(lua_State *L) {
    // gets Ship.oldCtor function and calls it using
    // arg1 and arg2 (slot0, slot1) as arguments.
    // returns the Ship object.
    lua_getglobal(L, STR("Ship"));
    lua_getfield(L, -1, STR("oldCtor"));
    lua_remove(L, -2);
    lua_pushvalue(L, 1);
    lua_pushvalue(L, 2);
    lua_pcall(L, 2, 1, 0);

    // get the Ship's id (unique id, not ship's id).
    lua_getfield(L, -1, STR("id"));
    int shipId = (int) lua_tonumber(L, -1);
    lua_pop(L, 1);

    // search the id on skins, if found, replace
    // skinId with the skin.
    const auto &ship = skins.find(shipId);
    if (ship != skins.end()) {
        replaceAttributeN(L, STR("skinId"), ship->second);
    }

    return 1;
}

int hookCommitCombat(lua_State *L) {
    // get slot0.contextData.editFleet, pass to int and then pop
    // editFleet.
    lua_getfield(L, 1, STR("contextData"));
    lua_getfield(L, -1, STR("editFleet"));
    int editFleet = (int) lua_tonumber(L, -1);
    lua_pop(L, 1);

    // get normalStageIDs from contextData, which wasn't popped earlier,
    // pass it to int then pop both.
    lua_getfield(L, -1, STR("normalStageIDs"));
    int normalStageLength = (int) lua_objlen(L, -1);
    lua_pop(L, 2);

    if (editFleet > normalStageLength) {
        if (config.Enemies.Enabled || config.Aircraft.Enabled || config.Weapons.Enabled) {
            // pg.TipsMgr.GetInstance()
            lua_getglobal(L, STR("pg"));
            lua_getfield(L, -1, STR("TipsMgr"));
            lua_getfield(L, -1, STR("GetInstance"));
            lua_remove(L, -2);
            lua_remove(L, -2);
            lua_pcall(L, 0, 1, 0);

            // :ShowTips(slot0, string)
            lua_getfield(L, -1, STR("ShowTips"));
            lua_insert(L, -2);
            lua_pushstring(L, STR("Don't cheat on EX ;)"));
            lua_pcall(L, 2, 0, 0);

            return 0;
        }
        // get emit from slot0, push a copy of slot0,
        // get slot0.contextData.mediatorClass.ON_EX_PRECOMBAT
        // and remove contextData & mediatorClass.
        // [emit] [slot0] [ON_EX_PRECOMBAT]
        lua_getfield(L, 1, STR("emit"));
        lua_pushvalue(L, 1);
        lua_getfield(L, 1, STR("contextData"));
        lua_getfield(L, -1, STR("mediatorClass"));
        lua_getfield(L, -1, STR("ON_EX_PRECOMBAT"));
        lua_remove(L, -2);
        lua_remove(L, -2);

        // get contextData.editFleet, remove contextData
        // and push false to the stack.
        // [emit] [slot0] [ON_EX_PRECOMBAT] [editFleet] [false]
        // then call the function
        lua_getfield(L, 1, STR("contextData"));
        lua_getfield(L, -1, STR("editFleet"));
        lua_remove(L, -2);
        lua_pushboolean(L, 0);
        lua_pcall(L, 4, 0, 0);
    } else {
        // get emit from slot0, push a copy of slot0,
        // get slot0.contextData.mediatorClass.ON_EX_PRECOMBAT
        // and remove contextData & mediatorClass.
        // [emit] [slot0] [ON_PRECOMBAT]
        lua_getfield(L, 1, STR("emit"));
        lua_pushvalue(L, 1);
        lua_getfield(L, 1, STR("contextData"));
        lua_getfield(L, -1, STR("mediatorClass"));
        lua_getfield(L, -1, STR("ON_PRECOMBAT"));
        lua_remove(L, -2);
        lua_remove(L, -2);

        // get contextData.editFleet, and remove contextData,
        // [emit] [slot0] [ON_EX_PRECOMBAT] [editFleet]
        // then call the function.
        lua_getfield(L, 1, STR("contextData"));
        lua_getfield(L, -1, STR("editFleet"));
        lua_remove(L, -2);
        lua_pcall(L, 3, 0, 0);
    }

    return 0;
}

int hookCommitTrybat(lua_State *L) {
    if (config.Enemies.Enabled || config.Aircraft.Enabled || config.Weapons.Enabled) {
        // pg.TipsMgr.GetInstance()
        lua_getglobal(L, STR("pg"));
        lua_getfield(L, -1, STR("TipsMgr"));
        lua_getfield(L, -1, STR("GetInstance"));
        lua_remove(L, -2);
        lua_remove(L, -2);
        lua_pcall(L, 0, 1, 0);

        // :ShowTips(slot0, string)
        lua_getfield(L, -1, STR("ShowTips"));
        lua_insert(L, -2);
        lua_pushstring(L, STR("Don't cheat on EX ;)"));
        lua_pcall(L, 2, 0, 0);

        return 0;
    }
    // get emit from slot0, push a copy of slot0,
    // get slot0.contextData.mediatorClass.ON_EX_PRECOMBAT
    // and remove contextData & mediatorClass.
    // [emit] [slot0] [ON_EX_PRECOMBAT]
    lua_getfield(L, 1, STR("emit"));
    lua_pushvalue(L, 1);
    lua_getfield(L, 1, STR("contextData"));
    lua_getfield(L, -1, STR("mediatorClass"));
    lua_getfield(L, -1, STR("ON_EX_PRECOMBAT"));
    lua_remove(L, -2);
    lua_remove(L, -2);

    // get contextData.editFleet, remove contextData
    // and push false to the stack.
    // [emit] [slot0] [ON_EX_PRECOMBAT] [editFleet] [false]
    // then call the function
    lua_getfield(L, 1, STR("contextData"));
    lua_getfield(L, -1, STR("editFleet"));
    lua_remove(L, -2);
    lua_pushboolean(L, 1);
    lua_pcall(L, 4, 0, 0);

    return 0;
}

int hookBUAddBuff(lua_State *L) {
    constexpr int FOE_CODE = -1;

    lua_getfield(L, 1, STR("_IFF"));
    const int IFF = static_cast< int >( lua_tonumber(L, -1));
    lua_pop(L, 1);

    if (IFF != FOE_CODE) {
        lua_getglobal(L, STR("ys"));
        lua_getfield(L, -1, STR("Battle"));
        lua_getfield(L, -1, STR("BattleUnit"));
        lua_getfield(L, -1, STR("oldAddBuff"));

        lua_pushvalue(L, 1);
        lua_pushvalue(L, 2);
        lua_pushvalue(L, 3);

        lua_pcall(L, 3, 0, 0);
        lua_pop(L, 3);
    }

    return 0;
}

void (*old_lua_tolstring)(lua_State *instance, int index, int &strLen);

void lua_tolstring(lua_State *instance, int index, int &strLen) {
    if (instance && !exec) {
        exec = true;

        lua_State *nL = lua_newthread(instance);
        lua_getglobal(nL, STR("pg"));

        lua_getglobal(nL, STR("CheaterMarkCommand"));
        lua_pushcfunction(nL, nilFunc);
        lua_setfield(nL, -2, STR("execute"));
        lua_pop(nL, 1);

        lua_getglobal(nL, STR("ActivityBossSceneTemplate"));
        lua_pushcfunction(nL, hookCommitCombat);
        lua_setfield(nL, -2, STR("commitCombat"));
        lua_pop(nL, 1);

        lua_getglobal(nL, STR("ActivityBossSceneTemplate"));
        lua_pushcfunction(nL, hookCommitTrybat);
        lua_setfield(nL, -2, STR("commitTrybat"));
        lua_pop(nL, 1);

        if (config.Aircraft.Enabled) { modAircraft(nL); }
        if (config.Enemies.Enabled) { modEnemies(nL); }
        if (config.Skins.Enabled) { modSkins(nL); }
        if (config.Misc.Enabled) { modMisc(nL); }
        if (config.Weapons.Enabled) { modWeapons(nL); }
    }
    return old_lua_tolstring(instance, index, strLen);
}

// thread where everything is ran
void *hack_thread(void *) {
    // check if target lib is loaded
    do {
        sleep(3);
    } while (!isLibraryLoaded(targetLibName));

    // load necessary functions
    loadil2cppfuncs();
    loadluafuncs();

    hook((void *) GETLUAFUNC("lua_tolstring"), (void *) lua_tolstring,
         (void **) &old_lua_tolstring);

    return nullptr;
}

void getConfigPath(JNIEnv *env, jobject context) {
    jclass cls_Env = env->FindClass(OBFUSCATE("android/app/NativeActivity"));
    jmethodID mid = env->GetMethodID(cls_Env, OBFUSCATE("getExternalFilesDir"),
                                     OBFUSCATE("(Ljava/lang/String;)Ljava/io/File;"));
    jobject obj_File = env->CallObjectMethod(context, mid, NULL);
    jclass cls_File = env->FindClass(OBFUSCATE("java/io/File"));
    jmethodID mid_getPath = env->GetMethodID(cls_File, OBFUSCATE("getPath"),
                                             OBFUSCATE("()Ljava/lang/String;"));
    auto obj_Path = (jstring) env->CallObjectMethod(obj_File, mid_getPath);
    const char *path = env->GetStringUTFChars(obj_Path, nullptr);

    std::string route(path);
    configPath = route + "/Perseus.ini";
    skinPath = route + "/Skins.ini";

    env->ReleaseStringUTFChars(obj_Path, path);
}

void init(JNIEnv *env, jclass clazz, jobject context) {

    // get external path where config shall be located
    getConfigPath(env, context);

    if (access(configPath.c_str(), F_OK) != 0) {
        if (!writeConfigFile()) {
            crash();
        }
    }

    std::vector<std::string> fileLines;

    std::string line;
    std::ifstream configFile(configPath);
    if (configFile.is_open()) {
        while (getline(configFile, line)) {
            // if line contains "#" or its length is less than 3 don't load it
            if (line.find('#') != std::string::npos || line.length() < 3) {
                continue;
            }
            fileLines.push_back(line);
        }
        configFile.close();
    } else {
        crash();
    }

    std::string wm = OBFUSCATE("original repo: github.com/Egoistically/Perseus.");
    if (fileLines[0] != wm)
        crash();
    fileLines.erase(fileLines.begin());

    if (fileLines.size() != 46) {
        config.Valid = false;
        writeConfigFile();
    }

    // Aircraft
    checkHeader(fileLines[0], OBFUSCATE("[Aircraft]"));
    config.Aircraft.Enabled = getKeyEnabled(fileLines[1], OBFUSCATE("Enabled"));
    if (config.Aircraft.Enabled) {
        config.Aircraft.Accuracy = getKeyValue(fileLines[2], OBFUSCATE("Accuracy"));
        config.Aircraft.AccuracyGrowth = getKeyValue(fileLines[3],
                                                     OBFUSCATE("AccuracyGrowth"));
        config.Aircraft.AttackPower = getKeyValue(fileLines[4], OBFUSCATE("AttackPower"));
        config.Aircraft.AttackPowerGrowth = getKeyValue(fileLines[5],
                                                        OBFUSCATE("AttackPowerGrowth"));
        config.Aircraft.CrashDamage = getKeyValue(fileLines[6], OBFUSCATE("CrashDamage"));
        config.Aircraft.Hp = getKeyValue(fileLines[7], OBFUSCATE("Hp"));
        config.Aircraft.HpGrowth = getKeyValue(fileLines[8], OBFUSCATE("HpGrowth"));
        config.Aircraft.Speed = getKeyValue(fileLines[9], OBFUSCATE("Speed"));
    }

    // Enemies
    checkHeader(fileLines[10], OBFUSCATE("[Enemies]"));
    config.Enemies.Enabled = getKeyEnabled(fileLines[11], OBFUSCATE("Enabled"));
    if (config.Enemies.Enabled) {
        config.Enemies.AntiAir = getKeyValue(fileLines[12], OBFUSCATE("AntiAir"));
        config.Enemies.AntiAirGrowth = getKeyValue(fileLines[13], OBFUSCATE("AntiAirGrowth"));
        config.Enemies.AntiSubmarine = getKeyValue(fileLines[14], OBFUSCATE("AntiSubmarine"));
        config.Enemies.Armor = getKeyValue(fileLines[15], OBFUSCATE("Armor"));
        config.Enemies.ArmorGrowth = getKeyValue(fileLines[16], OBFUSCATE("ArmorGrowth"));
        config.Enemies.Cannon = getKeyValue(fileLines[17], OBFUSCATE("Cannon"));
        config.Enemies.CannonGrowth = getKeyValue(fileLines[18], OBFUSCATE("CannonGrowth"));
        config.Enemies.Evasion = getKeyValue(fileLines[19], OBFUSCATE("Evasion"));
        config.Enemies.EvasionGrowth = getKeyValue(fileLines[20], OBFUSCATE("EvasionGrowth"));
        config.Enemies.Hit = getKeyValue(fileLines[21], OBFUSCATE("Hit"));
        config.Enemies.HitGrowth = getKeyValue(fileLines[22], OBFUSCATE("HitGrowth"));
        config.Enemies.Hp = getKeyValue(fileLines[23], OBFUSCATE("Hp"));
        config.Enemies.HpGrowth = getKeyValue(fileLines[24], OBFUSCATE("HpGrowth"));
        config.Enemies.Luck = getKeyValue(fileLines[25], OBFUSCATE("Luck"));
        config.Enemies.LuckGrowth = getKeyValue(fileLines[26], OBFUSCATE("LuckGrowth"));
        config.Enemies.Reload = getKeyValue(fileLines[27], OBFUSCATE("Reload"));
        config.Enemies.ReloadGrowth = getKeyValue(fileLines[28], OBFUSCATE("ReloadGrowth"));
        config.Enemies.RemoveBuffs = getKeyEnabled(fileLines[29], OBFUSCATE("RemoveBuffs"));
        config.Enemies.RemoveEquipment = getKeyEnabled(fileLines[30],
                                                       OBFUSCATE("RemoveEquipment"));
        config.Enemies.RemoveSkill = getKeyEnabled(fileLines[31], OBFUSCATE("RemoveSkill"));
        config.Enemies.Speed = getKeyValue(fileLines[32], OBFUSCATE("Speed"));
        config.Enemies.SpeedGrowth = getKeyValue(fileLines[33], OBFUSCATE("SpeedGrowth"));
        config.Enemies.Torpedo = getKeyValue(fileLines[34], OBFUSCATE("Torpedo"));
        config.Enemies.TorpedoGrowth = getKeyValue(fileLines[35], OBFUSCATE("TorpedoGrowth"));
    }

    checkHeader(fileLines[36], OBFUSCATE("[Misc]"));
    config.Misc.Enabled = getKeyEnabled(fileLines[37], OBFUSCATE("Enabled"));
    if (config.Misc.Enabled) {
        config.Misc.ExerciseGodmode = getKeyEnabled(fileLines[38],
                                                    OBFUSCATE("ExerciseGodmode"));
        config.Misc.FastStageMovement = getKeyEnabled(fileLines[39],
                                                      OBFUSCATE("FastStageMovement"));
    }

    // Skins
    checkHeader(fileLines[40], OBFUSCATE("[Skins]"));
    config.Skins.Enabled = getKeyEnabled(fileLines[41], OBFUSCATE("Enabled"));

    // Weapons
    checkHeader(fileLines[42], OBFUSCATE("[Weapons]"));
    config.Weapons.Enabled = getKeyEnabled(fileLines[43], OBFUSCATE("Enabled"));
    if (config.Weapons.Enabled) {
        config.Weapons.Damage = getKeyValue(fileLines[44], OBFUSCATE("Damage"));
        config.Weapons.ReloadMax = getKeyValue(fileLines[45], OBFUSCATE("ReloadMax"));
    }

    if (!config.Valid) {
        crash();
    }

    Toast(env, context, OBFUSCATE("Enjoy the feet, by @Egoistically"),
          ToastLength::LENGTH_LONG);

    pthread_t ptid;
    pthread_create(&ptid, nullptr, hack_thread, nullptr);

    if (!toastCalled) {
        crash();
    }
}

int RegisterMain(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {
                    OBFUSCATE("init"),
                    OBFUSCATE("(Landroid/content/Context;)V"),
                    reinterpret_cast<void *>(init)
            },
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/unity3d/player/UnityPlayerActivity"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;

    return JNI_OK;
}

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);

    if (RegisterMain(env) != 0)
        return JNI_ERR;
    return JNI_VERSION_1_6;
}
