# cCompiler
An assembler for x86.

## Requirements
 - Will require python (version 3) installed globally
 - premake5 (not required for Windows - for Windows premake5 is bundled with the repo)
 - A C++11 compiler (only tested with MSVC as of Visual Studio 2017 so far).

## Building
 - Clone the repository `https://github.com/DontBelieveMe/cCompiler.git`.
 - You probably want to be on the `v3` branch (that's where development is happening currently).
 - Follow the instructions for your platform (detailed below).
 - #### Windows
   - Run `cb premake` to generate the Visual Studio Project Files.
   - (Command Line) Run `cb build` to build the project.
   - (Command Line) The project can then be run by calling the `asm` batch file with your arguments  
     e.g. `asm -s input_file.asm ...`
   - (Visual Studio) Open the project files in visual studio
   - (Visual Studio) Set the `cCompiler-as` project as the startup project.
   - (Visual Studio) Build & Run as normal from visual studio.
   - It is recommnded to use the command line approach (aka using the `cb` build utility script)
   
   - #### `cb` command line reference
     - `cb premake` Run premake and generate Visual Studio project files as well as running some python code generation.
     - `cb build` Invoke msbuild and build the project
     - `cb test` Run the unit test suite (`cb build` will build the tests)
     - `cb devenv` Open Visual Studio
     - `cb clean` Delete the build folder as well as all Visual Studio project files.
 - #### Linux
   - todo: fill this in
 - #### MacOS
   - todo: fill this in.
