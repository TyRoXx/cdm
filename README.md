# cdm: C++ dependency manager
An experimental application for managing C or C++ headers and pre-built
libraries.

You can find a more serious approach here: https://github.com/conan-io/conan

# Continuous Integration
* GCC on Ubuntu 12.04: [![Build Status](https://travis-ci.org/TyRoXx/cdm.svg)](https://travis-ci.org/TyRoXx/cdm)
* Visual C++ 2013: [![Build status](https://ci.appveyor.com/api/projects/status/gicp12vo9mldy1fa?svg=true)](https://ci.appveyor.com/project/TyRoXx/cdm)

# Source code formatting
There is a CMake target called `clang-format` to format the source code
of this project according to the style defined in `.clang-format`. You
can for example run the target `clang-format` in Visual Studio or
`make clang-format` on the command line.

# Roadmap
Libraries to try:
* most important
  * silicium
  * github.com/chriskohlhoff/urdl
  * github.com/Microsoft/GSL
* libclang
* Lua
* Luabind
* LuaJIT
* Boost.Outcome
* Boost.AFIO
* stand-alone ASIO
* eggs-cpp/variant
* CURL
* ZLib
* OpenSSL
* LibreSSL
* Boost.Fiber
* Boost.Hana (header-only)
* sqlpp11
* hpx
* github.com/datasift/served
* folly
* Crypto++
* SDL
* SDL2
* Boost.Http
* SFML
* ericniebler/range-v3
* actor-framework
* Ogre3D
* Irrlicht
* Intel TBB
* Cap’n Proto
* casablanca.codeplex.com
* Protocol Buffers
* cereal
* CppUnit
* AngelScript
* uriparser
