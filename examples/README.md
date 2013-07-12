### Creating an Example Project which Uses MaidSafe Libraries

The super-project provides an ['examples' folder][examples].  Currently this only contains a dummy project with nothing more than a "Hello World" type example.  The intention is to add further small exemplars, but in the meantime the dummy project is an excellent way to try out existing MaidSafe libraries.

The dummy project is set up to include and link to the MaidSafe libraries and their dependencies.  You can copy this dummy directory anywhere and, as long as you provide the path to the build root of the MaidSafe super-project, it will work as a standalone scratch project.

The included [CMakeLists.txt][cmakelists] contains several comment blocks explaining the various sections.

The steps to set up a scratch project are:

1. Copy "MaidSafe/examples/dummy" to desired location and rename "dummy" as appropriate:

        cp -r /home/dev/MaidSafe/examples/dummy /home/dev/scratch
        cd /home/dev/scratch

2. Amend the copied CMakeLists.txt (/home/dev/scratch/CMakeLists.txt) as appropriate.

3. Invoke CMake and provide the path to your MaidSafe build root:

        cmake -Bbuild -H. -DMAIDSAFE_BINARY_DIR="/home/dev/MaidSafe/build"
        cd build
        make


[examples]: https://github.com/maidsafe/MaidSafe/tree/next/examples
[cmakelists]: https://github.com/maidsafe/MaidSafe/blob/next/examples/dummy/CMakeLists.txt