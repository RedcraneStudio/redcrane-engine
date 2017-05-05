# Freetype GL - A C OpenGL Freetype engine

A small library for displaying Unicode in OpenGL using a single texture and
a single vertex buffer.

This repository is a small, no-frills distribution of freetype-gl for easy
integration with CMake projects. It removes the tests, demos, and the associated
assets. The CMakeLists.txt is also simplified, albeit slightly opinionated:
freetype-gl is instructed to always use OpenGL VAOs but this can easily be
changed for your needs.

[See the official repository for details!](http://github.com/rougier/freetype-gl)

## Contributors

* Ryan.H.Kawicki (Initial CMake project)
* Julian Mayer (Several bugfixes and code for demo-opengl-4.cc)
* Sylvain Duclos (Android port)
* Wang Yongcong (Improvements on the windows build and code review)
* Jonas Wielicki (Bug report & fix on the CMakefile)
* whatmannerofburgeristhis (Bug report in makefont)
* Andrei Petrovici (Fine analysis of the whole code and report of potential problems)
* Cristi Caloghera (Report on bad vertex buffer usage)
* Andrei Petrovici (Code review)
* Kim Jacobsen (Bug report & fix)
* bsoddd (Bug report & fix)
* Greg Douglas (Bug report & fix)
* Jim Teeuwen (Bug report & fix)
* quarnster (Bug report & fix)
* Per Inge Mathisen (Bug report & fix)
* Wojciech Mamrak (Code review, bug report & fix)
* Wael Eloraiby (Put code to the C89 norm and fix CMakefile)
* Christian Forfang (Code review, fix & patch for 3.2 core profile)
* Lukas Murmann (Code review & fix for 3.2 core profile)
* Jérémie Roy (Code review, fix and new ideas)
* dsewtz (Bug report & fix)
* jcgamestoy (Bug report & fix)
* Behdad Esfahbod (Bug fix on harfbuzz demo)
* Marcel Metz (Bug report & fix, CMmake no demo option, makefont parameters)
* PJ O'Halloran (svn to git migration)
* William Light (Face creation from memory)
* Jan Niklas Hasse (Bug report & fix + README.md)
* Pierre-Emmanuel Lallemant (Bug report & fix + travis setup)
* Robert Conde (Bug report & fix)
* Mikołaj Siedlarek (Build system bug fix)
* Preet Desai (Bug report & fix)
* Andy Staton (CMake fix and added namespace safeguard (avoiding glm collisions))
* Daniel Burke (Removed GLEW dependency and fix problems with font licences)
* Bob Kocisko (Added horizontal text alignment and text bounds calculation)
* Ciro Santilli (Improve markdown documentation)
