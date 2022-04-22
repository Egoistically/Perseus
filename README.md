# Perseus
Fancy name for a simple native library that patches Azur Lane scripts.  
Does not rely on offsets, so game updates shouldn't break it so long as no security measures are introduced.

## Config
The config file is located on the path `/sdcard/Perseus/`. Comments should make it pretty straightforward.  
An example with some values changed:
```ini
# [*] Delete the file to reset it. Restart the game to apply any changes.
# [*] Options can be one of the following types:
#      [1] Bool - e.g. "true" or "false" - Keys: All "Enabled"s, RemoveEquipment and RemoveSkill.
#      [2] Array of whole numbers with no whitespaces - e.g. "{206060,206061}" - Keys: Only SkinList.
#      [3] Whole numbers or false to disable changes - If you need an example you're five - Keys: All the ones not mentioned above.
# [*] "Enabled"s apply to the entirety of their section, if they're disabled everything will be.
# [*] If the game crashes, this file is most likely misconfigured.
[Aircraft]
Enabled=true
Accuracy=false
AccuracyGrowth=false
AttackPower=false
AttackPowerGrowth=false
CrashDamage=false
Hp=10
HpGrowth=false
Speed=1

[Enemies]
Enabled=true
AntiAir=false
AntiAirGrowth=false
AntiSubmarine=false
Armor=false
ArmorGrowth=false
Cannon=false
CannonGrowth=false
Evasion=false
EvasionGrowth=false
Hit=false
HitGrowth=false
Hp=false
HpGrowth=false
Luck=false
LuckGrowth=false
Reload=false
ReloadGrowth=false
RemoveEquipment=true
RemoveSkill=true
Speed=false
SpeedGrowth=false
Torpedo=false
TorpedoGrowth=false

[Skins]
Enabled=true
SkinList={206061}

[Weapons]
Enabled=false
Damage=false
ReloadMax=false
```
To modify skins simply add the `id` of the skin you desire to `SkinList`. All ids can be found [here](https://raw.githubusercontent.com/AzurLaneTools/AzurLaneData/main/EN/ShareCfg/ship_skin_template.json).

## Credits
* Library built upon [Android Hooking Patching Template](https://github.com/LGLTeam/Android-Hooking-Patching-Template).
* Mods based on [Azur Lane Scripts Autopatcher](https://github.com/n0k0m3/Azur-Lane-Scripts-Autopatcher).