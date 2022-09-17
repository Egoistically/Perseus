# Perseus
Fancy name for a simple native library that patches Azur Lane scripts.  
Does not rely on offsets, so game updates shouldn't break it so long as no security measures are introduced.

**I do not intend to maintain this further. I may (or may not) release the source code in the future, not before rewriting it from scratch though.  
Feel free to do whatever you wish with it so long as you don't mind getting banned.**

## Loading the library
Add the following method to `UnityPlayerActivity`, anywhere above its `onCreate`:
```smali
.method private static native init(Landroid/content/Context;)V
.end method
```
And these lines to `onCreate`:
```smali
	const-string v0, "Perseus"

	invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V

	invoke-static {p0}, Lcom/unity3d/player/UnityPlayerActivity;->init(Landroid/content/Context;)V
```
(Preferably without replacing other variables, such as between `.locals 2` and `const/4 v0, 0x1`.)

## Config
Settings can be found inside `Perseus.ini`, located within the game's external files directory (`/sdcard/Android/data/{package-name}/files/`).  
If you're unsure of your region's package name, just look it up. All of them include "AzurLane" though.

Enabling the Skins mod gives you all skins in-game as if you had bought them. They are persistent between restarts.

## Credits
* https://github.com/adamyaxley/Obfuscate.
* https://github.com/Rprop/And64InlineHook.
* https://github.com/joeyjurjens/Android-Hooking-Template.
* https://github.com/n0k0m3/Azur-Lane-Scripts-Autopatcher.