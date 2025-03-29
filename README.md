# Command & Conquer Renegade

An early fork of the open-source release of Renegade.

Current work before this is back to a fairly complete state:

- [x] Build system using a frankenstein of meson for C++ and a fairly standard Cargo build for...
- [x] Some new Rust/WGPU code to (currently) emulate the DX8 fixed function rendering
- [x] Builds under C++20
- [x] DirectInput ripped out and replaced with windows Raw Input in gameplay and standard mouse input in menus.
- [ ] Audio re-implemented
- [ ] Fixes for current rendering issues, any anything else I have broken!
- [ ] Movies (Bink) re-implemented (FFMpeg has a reverse engineered codec?)

From there there's plenty to do, but I'm not making any promises yet!

## Building

Warning: I have not attempted to verify these steps yet on a fresh machine, so let me know if you run into any issues.

You will need to have the Meson build system, rustup, and MSVC 2019. (It seems Meson still doesn't
support 2022 yet? I might need to switch...)

One way to easily get these is to use the built-in `winget` tool:

```
winget install -e mesonbuild.meson Rustlang.Rustup Microsoft.VisualStudio.2019.BuildTools
```

Then running the Visual Studio Installer and modifying the build tools installation to add:
- `MSVC v142 - VS 2019 C++ x64/x86 build tools (Latest)`
- `C++ ATL for v142 build tools (x86 & x64)` (for now at least)
- `Windows 10/11 SDK` (whichever you like, e.g. the latest)

From here you need to configure meson to use the x86 (e.g. 32-bit) build tools, by running
the "x86 Native Tools Command Prompt for VS 2022", switching to the current directory, and
running `meson setup buildDir` (or whatever you want to call the build directory): this
will use the current environment variables to set up the build system to use the correct x86
compiler.

Now in any prompt you can run `meson compile -C buildDir` to build the project.

At the moment running is still messy, meson's native support for dll lookup doesn't seem to
work with the built "scripts.dll" so you will need to copy `buildDir/Code/Scirpts/scriptsd.dll`
to the output directory (e.g. `Run`) along with the game assets to run. The shipped scripts.dll
will not work with this build (I've changed the ABI); from what I can tell you need only the
`Data` directory and the fonts in the root directory (`54251___.TTF` and `ARI_____.TTF`) from
the original game install. The game also automatically forces the working directory to the executable
location, and searches for the assets from there, so I've added a `-data=` argument you can set to
the path to the `Run` directory while developing for now. This is all fairly temporary, hopefully!

Personally I mostly use Jetbrains' CLion IDE when editing, which will help set up a lot of this up for you too,
though you will still need the dependencies above, and to switch the active toolchain to x86 currently.

## License

This repository and its contents are licensed under the GPL v3 license, with additional terms applied. Please
see [LICENSE.md](LICENSE.md) for details.
