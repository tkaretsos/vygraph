# vygraph

## Requirements

* **LLVM, Clang and Clang-tools source code.**  
Current version is 3.4. Probably works with the newest version, but it's not tested.

* **CMake**  
Version 2.8.1 or higher.

* **g++**  
Version 4.8.1 or higher.

## How to build

vygraph is using the Clang's build system. Read more [here](http://clang.llvm.org/get_started.html).  
I provide two scripts to make things a bit simpler, because vygraph's source directory lies under a  
different directory from the LLVM/Clang source code. Here are the two scripts:  

#### download_llvm
This script is only for downloading and extracting the required LLVM and Clang sources. If you already  
have the sources of LLVM, Clang and [Clang tools](http://clang.llvm.org/docs/ClangTools.html), it is not required to execute this script.  

`sh download_llvm <directory>`

where `<directory>` is the directory where you want the sources to be downloaded and extracted.  
When this script is executed, it will generate the file `llvm_dirs` which contains the paths for  
the source, build and bin directories of LLVM. You can use this file with the next script, in order  
to configure the project.

#### configure
This script helps set the current project as a tool project of Clang, so when you build LLVM and Clang  
this project will be built as well. When you run this script you need to provide it with three directories.  
The LLVM source directory, the LLVM build directory and the LLVM bin directory:

`sh configure <llvm_src_dir> <llvm_obj_dir> <llvm_bin_dir>`

If you previously ran the `download_llvm` script you can also:

`sh configure $(cat llvm_dirs)`

After `configure` script is executed successfully you can run `make` in the project directory and LLVM,  
Clang, Clang tools and this project will be built. 

#### Example
If you do not have the sources of LLVM and Clang:  

`sh download_llvm ..`  
`sh configure ($cat llvm_dirs)`  
`make`  


## How to run

After the project has been successfully built, a symlink to the executable will be created in the project's  
root directory (`vygraph`). Execute it and give a C source code file as an input:

`./vygraph my_file.c --`

The two dashes in the end are required by the tool.
