# mss-macro

An Aroma plugin to simulate manual superswim.

## Installation

(`[ENVIRONMENT]` is a placeholder for the actual environment name.)

1. Copy the file `AromaBasePlugin.wps` into `sd:/wiiu/environments/[ENVIRONMENT]/plugins`.
2. Requires the [WiiUPluginLoaderBackend](https://github.com/wiiu-env/WiiUPluginLoaderBackend) in `sd:/wiiu/environments/[ENVIRONMENT]/modules`.
3. Requires the [NotificationModule](https://github.com/wiiu-env/NotificationModule) in `sd:/wiiu/environments/[ENVIRONMENT]/modules`.
4. Requires the [RPXLoadingModule](https://github.com/wiiu-env/RPXLoadingModule) in `sd:/wiiu/environments/[ENVIRONMENT]/modules`.
5. Requires the [SDHotSwapModule](https://github.com/wiiu-env/SDHotSwapModule) in `sd:/wiiu/environments/[ENVIRONMENT]/modules`.

## Usage

Via the plugin config menu (press L, DPAD Down and Minus on the GamePad, Pro Controller or Classic Controller) you can configure the plugin. The available options are the following:

- **Enable macro system**:
  - Enables/Disables the functionality of the macro
- **Enable stick inputs**:
  - Enables/Disables the stick portion of the macro
- **Delay Nerf**:
  - The delay between pausing/unpausing.
  - This exists for the purpose of nerfing this macro to be slower than RTA manual superswims

## Building

For building you need:
- [wups](https://github.com/wiiu-env/WiiUPluginSystem)
- [wut](https://github.com/decaf-emu/wut)
- [libnotifications](https://github.com/wiiu-env/libnotifications)
- [librpxloader](https://github.com/wiiu-env/librpxloader)
- [libcurlwrapper](https://github.com/wiiu-env/libcurlwrapper)
- [libsdutils](https://github.com/wiiu-env/libsdutils)
- [libmocha](https://github.com/wiiu-env/libmocha)

## Building using the Dockerfile

It's possible to use a docker image for building. This way you don't need anything installed on your host system.

```
# Build docker image (only needed once)
docker build . -t aroma-base-plugin-builder

# make 
docker run -it --rm -v ${PWD}:/project aroma-base-plugin-builder make

# make clean
docker run -it --rm -v ${PWD}:/project aroma-base-plugin-builder make clean
```

## Format the code via docker

`docker run --rm -v ${PWD}:/src ghcr.io/wiiu-env/clang-format:13.0.0-2 -r ./src --exclude ./src/utils/json.hpp -i`
