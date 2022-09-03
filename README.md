# QOI Decoder Starter Project

This project is intended to be used as a starting point for writing a decoder for the QOI image format.

> ⚠️ While you should be able to build this project on all major platforms (Windows, MacOS, and common Linux distributions), it is recommended to do so in a UNIX-y environment.
> In a subsequent lab, we're going to be building on the project, adding some tools that work best in Linux or MacOS.
> 
> If you are using a Windows device, I've found WSL2 works pretty well.
> I have tested this project on an Ubuntu WSL2 instance from Windows 11.
> Even the GUI image viewer works!

## Getting Started

### Pre-Setup

#### Option A: GitHub

![Screenshot showing "Use this template" button in GitHub](img/qoi-template.png)

If you are a GitHub user and want to host your source code there, you can get started quickly by clicking "Use this template" on the repository's [home page](https://github.com/eced4406/qoi-starter-project).
This will prompt you to create a new private repository under your own account (similar to a fork).
Complete the form and clone the repository (with the `--recurse-submodules` option - more on that later) to bring the source code down to your local machine.

```
$ git clone --recurse-submodules https://github.com/YOUR_GITHUB_USERNAME/YOUR_PROJECT_NAME.git 
```

Fill in the placeholders above appropriately.
You can also use the SSH version of the URL if that's how you prefer to authenticate with GitHub.
Check out the [relevant GitHub docs](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/about-authentication-to-github#authenticating-with-the-command-line) for more information.

If you cloned the repository but forgot to add the `--recurse-submodules` option, you can run the following command to fetch them.
```
$ cd path/to/your/local/repo
$ git submodule update --init
```

#### Option B: Git without the Hub

If you want to get started with Git but don't want to use GitHub, you can pull the template repository into a local Git repository.

The first step is to make a local Git repository:
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

Fetch the git submodules with `git submodule update --init`.

#### Branches

Regardless of which approach you took, you should create a branch for your work.
This will make it easier to submit your assignment and pull in any upstream changes (e.g. if I need to fix something).

You can create and checkout a branch named "assignment-1" like this:
```
$ git checkout -b assignment-1
```

### Setup

Now that you have the source code on your local machine, we can start to setup the project.
There are a few tools that you'll need.
Check the platform-specific instructions for tips on installing them.

What you'll need:
- CMake v3.23+
- Clang v14+
- Visual Studio Code

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

Next, we'll install the latest stable version of Clang. The LLVM project provides [up-to-date packages for Debian and Ubuntu](https://apt.llvm.org/).

```
$ cd /tmp
$ wget https://apt.llvm.org/llvm.sh
$ chmod +x llvm.sh
$ sudo ./llvm.sh 14 all
```

Verify that clang-14 was installed:

```
$ clang-14 --version
Ubuntu clang version 14.0.5-++20220610081652+4d5dad43b2eb-1~exp1~20220610081739.155
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
$ brew install cmake llvm
```

Open the project with Visual Studio Code.

```
$ cd /path/to/qoi
$ code .
```

Select the Homebrew Clang kit (provided in `.vscode/cmake-kits.json`).

#### Windows

Before setting up this project on Windows, please see my note at the top of this README.
If you decide to use WSL2 rather than using Windows natively, you can follow the Linux instructions from within your WSL2 environment.

With that disclaimer out of the way, here's how you can get started on Windows natively.

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

Open the Command Palette with `Ctrl + Shift + P` and run "CMake: Scan for kits".

From the Command Palette, run "CMake: Select kit". Select "Visual Studio 2022 - x64".

### Building



## References

- QOI [specification](https://qoiformat.org/qoi-specification.pdf)
- QOI [reference implementation](https://github.com/phoboslab/qoi)
- Sy Brand's documentation [tutorial](https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/)
- Victoria Rudakova's documentation [tutorial](https://vicrucann.github.io/tutorials/quick-cmake-doxygen/)
- C++ Weekly - [Fuzz Testing](https://www.youtube.com/watch?v=gO0KBoqkOoU)
