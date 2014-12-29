# Working with CMake

### What are the naming conventions?
* Functions and macros should be snake-case (e.g. `target_link_libraries(...)`)
* Most (if not all) CMake built-in variables are uppercase with underscores (e.g. `CMAKE_CXX_FLAGS`)
* Variables and options which are meant to be specified by the user (e.g. via command line args) are also all uppercase with underscores (e.g. `INCLUDE_TESTS`)
* All other variables we define should be camel-case (e.g. `SourceDir`)
* Functions and macros we define in cmake\_modules/utils.cmake should be prefixed "ms\_" and commented properly (e.g. `ms_glob_dir(...)`)

### How do I define a new executable or static library
Use the `ms_add_executable` or `ms_add_static_library` functions in cmake\_modules/utils.cmake.  For both functions, the list of source files should be passed in as the last arguments.

### How do I link a non-header Boost library?
Use the camel-case variable ending in "Libs", e.g. `${BoostChronoLibs}`.

Since Boost doesn't come CMake-ready, we have a fairly complicated process whereby we download and extract boost sources, build its compiler (called b2) and set up a bunch of helper targets.  These helper targets trigger the respective Boost library to be built if required, but shouldn't be used in place of the camel-case variable mentioned above.

### Where do I add `options`?
For global ones (e.g. `INCLUDE_TESTS`) there is a group of `options` in the top-level CMakeLists.txt.  For any project-specific ones, just add them to the project's own CMakeLists.txt.
