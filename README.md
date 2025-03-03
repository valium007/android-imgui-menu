# android-imgui-menu
Inject imgui menu at runtime.

## Building
- Install NDK
- Change APP_ABI and APP_PLATFORM in Application.mk per your needs
```
git clone https://github.com/valium007/android-imgui-menu
cd android-imgui-menu
/path/to/NDK/ndk-build NDK_PROJECT_PATH=./ NDK_APPLICATION_MK=./Application.mk APP_BUILD_SCRIPT=./Android.mk
```

## Injecting
Download [AndKittyInjector](https://github.com/MJx0/AndKittyInjector/releases/tag/v4.1.0) and either use termux or adb to open a shell.
```
sudo ./AndKittyInjector -pkg package-name -lib libdraw.so -dl_memfd
```

## Credits
- [Dear ImGui](https://github.com/ocornut/imgui) - Bloat-free Graphical User interface
- [xHook](https://github.com/iqiyi/xHook) - A PLT hook library for Android native ELF
- [AndKittyInjector](https://github.com/MJx0/AndKittyInjector) - Inject a shared library into a process using ptrace