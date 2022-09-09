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

Your instructor may need to fix some things in the template.
If so, there will be new commits on the upstream repo that you'll need to merge into yours.
If there are changes, you'll get a notification on Brightspace with instructions.
They'll look something like this:

First, pull the changes from the upstream.
```
$ git checkout main
$ git pull upstream main
$ git log # print the log to review the changes
```

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

Run `> CMake: Scan for Kits` so that VS Code finds the version of Clang that you installed in step 2.

Run `> CMake: Select Kit` and select the version of Clang that you installed in step 2.
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

Select the Homebrew Clang kit (provided in `.vscode/cmake-kits.json`).

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

Enable CMake kits by changing your your VS Code settings.
Hit `Ctrl + ,` to open the VS Code settings.
You can make this a global setting or just for the current folder switching between the "User" and "Workspace" tabs.
Search for "CMake: Use CMake Presets" (cmake.useCMakePresets) and choose "never" in the dropdown.

Open the Command Palette with `Ctrl + Shift + P` and run "CMake: Scan for kits".

From the Command Palette, run "CMake: Select kit". Select "Visual Studio 2022 - x64".

### Building

From the Command Palette, run "CMake: Build"

## References

- QOI [specification](https://qoiformat.org/qoi-specification.pdf)
- QOI [reference implementation](https://github.com/phoboslab/qoi)
- Sy Brand's documentation [tutorial](https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/)
- Victoria Rudakova's documentation [tutorial](https://vicrucann.github.io/tutorials/quick-cmake-doxygen/)
- C++ Weekly - [Fuzz Testing](https://www.youtube.com/watch?v=gO0KBoqkOoU)
