APP_ABI 			:= armeabi-v7a arm64-v8a x86
APP_STL 			:= c++_static
APP_OPTIM 			:= release
APP_THIN_ARCHIVE 	:= true
APP_PIE 			:= true

LOCAL_LDFLAGS += -Wl,--gc-sections,--strip-all
APP_CFLAGS  += -fvisibility=hidden -fvisibility-inlines-hidden -fexceptions
APP_CFLAGS  += -g0 -O3 -fomit-frame-pointer -ffunction-sections -fdata-sections