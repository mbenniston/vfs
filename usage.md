
## Usage

## Bundle Explaination

Bundles are vfs's representation of a collection of files stored one after the other. These can be embedded within the program using `vfspack` to generate valid C++ source file.
Note: Embedded bundles should be kept small because the C++ compiler will run out of memory when trying to parse the generated source code. 

## Live-Reloading

When a file is loaded from disk it has the ability to change during the execution of the program. In vfs, disk files by default automatically refresh their content when it changes on disk. This event can be hooked by registering an observer to the disk file.

## Generating Documentation

Documentation is created using doxygen. Install doxygen and run it using the Doxyfile at the root of this repo. This will generate documentation for the project within the `docs` directory.