# Build Instructions using Microsoft Visual C++

These instructions were used to build LOOT using Microsoft Visual Studio 2012 and Microsoft Visual Studio Express 2013 for Desktop, though they should also apply to other versions of MSVC.

#### Boost

```
bootstrap.bat
b2 toolset=msvc threadapi=win32 link=static runtime-link=static variant=release address-model=32 --with-log --with-date_time --with-thread --with-filesystem --with-locale --with-regex --with-system  --with-iostreams
```

`link`, `runtime-link` and `address-model` can all be modified if shared linking or 64 bit builds are desired. LOOT uses statically-linked Boost libraries by default: to change this, edit [CMakeLists.txt](../CMakeLists.txt).

#### wxWidgets

Just build the solution provided by wxWidgets. You may need to change the C/C++ Runtime Library setting in each project to `Multi-threaded (/MT)`.

#### yaml-cpp

1. Configure CMake and generate a build system for Visual Studio by running:
  ```
  mkdir build
  cd build
  cmake.exe .. -G "Visual Studio 12" -DBOOST_ROOT={BOOST_ROOT} -DMSVC_SHARED_RT=OFF
  ```
  Adapt the commands as necessary for your particular setup.
2. Open the generated solution file, and build it with `Release` configuration.

#### Libloadorder

Follow the instructions in libloadorder's README.md to build it as a static library.

Example CMake keys: `-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=build -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=build`

#### Libgit2

1. Configure CMake and generate a build system for Visual Studio by running:
  ```
  mkdir build
  cd build
  cmake.exe .. -G "Visual Studio 12" -DBUILD_SHARED_LIBS=OFF -DSTATIC_CRT=ON
  ```
  Adapt the commands as necessary for your particular setup.
2. Open the generated solution file, and build it with `Release` configuration.

#### LOOT

LOOT uses the following CMake variables to set build parameters:

Parameter | Values | Default |Description
----------|--------|---------|-----------
`BUILD_SHARED_LIBS` | `ON`, `OFF` | `OFF` | Whether or not to build a shared libloot.
`PROJECT_STATIC_RUNTIME` | `ON`, `OFF` | `ON` | Whether to link the C++ runtime statically or not. This also affects the Boost libraries used.
`PROJECT_ARCH` | `32`, `64` | `32` | Whether to build 32 or 64 bit LOOT binaries.
`ALPHANUM_ROOT` | path | `../../alphanum` | Path to the folder containing `alphanum.hpp`.
`LIBESPM_ROOT` | path | `../../libespm` | Path to the root of the libespm repository folder.
`LIBGIT2_ROOT` | path | `../../libgit2` | Path to the root of the libgit2 repository folder.
`LIBLOADORDER_ROOT` | path | `../../libloadorder` | Path to the root of the libloadorder repository folder.
`YAMLCPP_ROOT` | path | `../../yaml-cpp` | Path to the root of the yaml-cpp folder.
`WXWIDGETS_ROOT` | path | `../../wxWidgets` | Path to the root of the wxWidgets folder.

The default paths given in the table above are relative to LOOT's `CMakeLists.txt`.

1. Set CMake up so that it builds the binaries in the `build` subdirectory of the LOOT folder.
2. Define any necessary parameters.
3. Configure CMake, then generate a build system for Visual Studio 12.
4. Open the generated solution file, and build it.
