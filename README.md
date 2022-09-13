# QOI Decoder Starter Project

This project is intended to be used as a starting point for writing a decoder for the QOI image format.

## Getting Started

### Pre-Setup

The first step is to get the template onto your local machine.
You can start by making an empty local repository:
```
$ mkdir -p /path/to/your/local/repo
$ cd /path/to/your/local/repo
$ git init
```

Now you can configure a remote connection to the template repository and pull its history into yours.
```
$ git remote add upstream https://github.com/eced4406/qoi-starter-project.git
$ git pull upstream main
```

Your local repository should now be populated with the contents of the template, and you should see commits from yours truly when you run `git log`.

Now, fetch the git submodules (vcpkg):
```
$ git submodule update --init
```

At this point, you can push your repo up to a hosting service (like GitHub or GitLab) or just keep it local.
For more info, see [here](https://docs.github.com/en/get-started/importing-your-projects-to-github/importing-source-code-to-github/adding-locally-hosted-code-to-github#adding-a-local-repository-to-github-using-git).

#### Branches

This will make it easier to submit your assignment and pull in any upstream changes (see below).

You can create and checkout a branch named "assignment" like this:
```
$ git checkout -b assignment
```

#### Getting Updates

Your instructor may need to fix or add some things in the template.
If so, there will be new commits on the upstream repo that you'll need to merge into yours.
If there are changes, you'll get a notification on Brightspace with instructions.
They'll look something like this:

First, pull the changes from the upstream.
```
$ git checkout main
$ git pull upstream main
$ git log # print the log to review the changes
```

Remember, you can run `git status` to check the state of your repository, including what branch you're currently on.

Then, merge the changes into your working branch.
```
$ git checkout assignment # or whatever you called your working branch for the first assignment
$ git merge main
```

### Setup

Now that you have the source code on your local machine, we can start to setup the project.
There are a few tools that you'll need.
Check the platform-specific instructions for tips on installing them.

What you'll need:
- CMake v3.23+
- Clang v13
- Visual Studio Code

Choose your own adventure:
- [Linux](#linux-ubuntu-or-debian)
- [MacOS](#macos)
- [Windows](#windows)

#### Linux (Ubuntu or Debian)

> ℹ️ Debian-based distributions, such as Ubuntu, are the most common, so I've provided instructions for them.
> You're welcome to use something else, but you may be on your own when it comes to troubleshooting.
> If you get this working on another distribution, feel free to share any tips with the class via our discussion board.

Get started by installing these packages:

```
$ sudo apt-get update
$ sudo apt-get install \
    build-essential \
    curl \
    gdb \
    libx11-dev \
    libxft-dev \
    libxext-dev \
    libsdl2-dev \
    ninja-build \
    pkg-config \
    tar \
    unzip \
    zip
```

##### 1. Install CMake

If you are using Ubuntu or Debian, you will likely not have the latest CMake available through apt's default repositories.
Rather than installing it with apt, you can use install script provided on CMake's download page [here](https://cmake.org/download/).

```
$ cd /tmp
$ wget https://github.com/Kitware/CMake/releases/download/v3.23.2/cmake-3.23.2-linux-x86_64.sh
$ chmod +x cmake-3.23.2-linux-x86_64.sh
$ ./cmake-3.23.2-linux-x86_64.sh --prefix=$HOME/.local --exclude-subdir --skip-license
```

Make sure `$HOME/.local/bin` is prepended to your `$PATH`.
It's common to manage your path in your shell's dotfile in your `$HOME` directory (e.g. `$HOME/.bashrc` for bash or `$HOME/.zshrc` for zsh).
Once CMake is installed and made available on your `PATH`, verify the version:

```
$ cmake --version
cmake version 3.23.2
...
```

Alternatively, you can use Python's `pip` (or even better `pipx`) to install CMake.
This is my preferred method, but your mileage may vary.
Here are some links that may prove helpful if you decide to try this approach:
- [pipx installation](https://github.com/pypa/pipx/#install-pipx)
- [cmake pypi package](https://pypi.org/project/cmake/)

##### 2. Install Clang (and LLVM tools)

Next, we'll install Clang 13. The LLVM project provides [up-to-date packages for Debian and Ubuntu](https://apt.llvm.org/).

```
$ cd /tmp
$ wget https://apt.llvm.org/llvm.sh
$ chmod +x llvm.sh
$ sudo ./llvm.sh 13 all
```

Verify that clang-13 was installed:

```
$ clang-13 --version
Ubuntu clang version 13.0.x
Target: x86_64-pc-linux-gnu
Thread model: posix
InstalledDir: /usr/bin
```

##### 3. Setup VS Code

Install the C/C++ Extension pack for VS Code (it should be recommended to you when you open the project for the first time).
You may need to restart/reload VS Code for this to take effect.

Select the Linux Debug/Release CMake preset depending on what you want to build (I'd recommend starting with Debug).

#### MacOS

Use [Homebrew](https://brew.sh/) to install the dependencies.

```
$ brew install cmake ninja llvm@13
```

Open the project with Visual Studio Code.

```
$ cd /path/to/qoi
$ code .
```

Select the MacOS ARM/Intel Debug/Release CMake configure preset depending on your system and what you want to build (I'd recommend starting on Debug).
These presets can be found in `CMakePresets.json` if you're curious.

#### Windows

##### 1. Install Visual Studio 2022

You have two options here.
You can either install the full Visual Studio 2022 (Community edition) if you want the Visual Studio IDE.
Alternatively, you can just install the build tools.
For this course, all you need are the build tools, but you may want the full Visual Studio IDE for other courses or projects.
- [Visual Studio 2022 - Community](https://visualstudio.microsoft.com/downloads/#visual-studio-community-2022)
- [Visual Studio 2022 - Build Tools](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022)

Make sure to select "Desktop Development with C++" when choosing what to install.

##### 2. Install CMake

Download and install the "Windows x64 Installer" from the Latest Release section [here](https://cmake.org/download/).

##### 3. Configure VS Code

Open the project folder in VS Code and install the recommended extensions.
You might need to restart VS Code after installing the extensions.

Open the Command Palette with `Ctrl + Shift + P` and run "CMake: Configure".
It should prompt you to choose a preset; select the one for the version of Visual Studio you have installed (2019 or 2022).

From the Command Palette, run "CMake: Build".
Select the "Debug" build preset so you can step through the code.

### Building

From the Command Palette, run "CMake: Build"

### Running and Debugging

The qoi_viewer application takes one command line argument: the path to the .qoi file to open.
If you just run the application without specifying the path, all you'll see is a help message about how to run the program.

When running from the command line, just add the path (absolute or relative to your current working directory) of a qio file to the end of your command:

```
$ cd build/out/bin # You might have to add Debug or Release to the end on Windows
$ ./qoi_viewer /path/to/your/repo/data/dice.qoi # remember to substitute this path with the real one!
```

This is great for a quick test, but you probably want to set up VS Code to build and run the application without having to go to the command line.
* Open up the "Run and Debug" menu on the left hand sidebar.
* Click "create a launch.json file", and then click "Add Configuration".
* Pick the "C/C++: Launch" option (the exact text may different depending on your platform).
* Update the "program" field to point to `"${command:cmake.launchTargetPath}"`.
* Add the path to the QOI file you want to open to the "arguments" list.
* Save the file.
* From the Command Palette, run "CMake: Set Debug Target" and select "qoi_viewer".
Now you can launch the viewer by typing F5 or by hitting the green play button at the top of the "Run and Debug" sidebar tab.

You can read more about debugging C/C++ projects with VS Code here:
* https://code.visualstudio.com/docs/cpp/cpp-debug
* https://vector-of-bool.github.io/docs/vscode-cmake-tools/debugging.html

### Benchmark

Part of your evaluation for this assignment will be based on the results of the benchmark.
The benchmark tests your decoder against four different variations of the "testcard_rgba" image.
* `data/benchmark/0.qoi` is the simplest image. It uses only the `qoi_op_rgb` and `_rgba` types.
* `data/benchmark/1.qoi` builds on `0.qoi` and makes use of the `qoi_op_run` type.
* `data/benchmark/2.qoi` builds on `1.qoi` and uses the `qoi_op_index` type.
* `data/benchmark/3.qoi` builds on `2.qoi` and uses the `qoi_op_diff` and `_luma` types.

To run the benchmark, first make sure you've merged in the latest changes from `upstream` (see [Getting Updates](#getting-updates)).
Then run "CMake: Delete cache and reconfigure".
Then select and run the "qoi_benchmark" utility.

## References

- QOI [specification](https://qoiformat.org/qoi-specification.pdf)
- QOI [reference implementation](https://github.com/phoboslab/qoi)
- Sy Brand's documentation [tutorial](https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/)
- Victoria Rudakova's documentation [tutorial](https://vicrucann.github.io/tutorials/quick-cmake-doxygen/)
- C++ Weekly - [Fuzz Testing](https://www.youtube.com/watch?v=gO0KBoqkOoU)
