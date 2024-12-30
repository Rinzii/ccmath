# Contribution Guidelines

> [!IMPORTANT]
> Currently the contribution guidelines are a work in progress. Please check back later for more information.

# Contributing to CCMath

First off, thanks for taking the time to contribute! ❤️

All types of contributions are encouraged and valued. See the [Table of Contents](#table-of-contents) for different ways to help and details about how this project handles them. Please make sure to read the relevant section before making your contribution. It will make it a lot easier for us maintainers and smooth out the experience for all involved. The community looks forward to your contributions. 🎉

If you enjoy this project but don’t have the time or ability to contribute code, you can still show your support in the following ways:
- Star the repository
- Mention this project in your own project's documentation
- Share this project on social media or at community meetups


## Table of Contents

- [I Have a Question](#i-have-a-question)
- [I Want To Contribute](#i-want-to-contribute)
  - [Core Tenets of CCMath](#core-tenets-of-ccmath)
  - [Your First Code Contribution](#your-first-code-contribution)
- [How to manually build CCMath with the command line](#how-to-manually-build-ccmath-with-the-command-line)
  - [Prerequisites](#prerequisites)
  - [How to clone project from GitHub](#how-to-clone-project-from-github)
  - [How to build the project](#how-to-build-the-project)
  - [How to run tests](#how-to-run-tests)
- [Additional Notes](#additional-notes)


## I Have a Question

> If you want to ask a question, we assume that you have read the available [Documentation]().

Before you ask a question, it is best to search for existing [Issues](https://github.com/Rinzii/ccmath/issues) that might help you. In case you have found a suitable issue and still need clarification, you can write your question in this issue. It is also advisable to search the internet for answers first.

If you then still feel the need to ask a question and need clarification, we recommend the following:

- Open an [Issue](https://github.com/Rinzii/ccmath/issues/new).
- Provide as much context as you can about what you're running into.
- Provide project and platform versions (nodejs, npm, etc), depending on what seems relevant.

We will then take care of the issue as soon as possible.


## I Want To Contribute

> **Legal Notice**  
> By contributing to this project, you agree that:
> - You have authored 100% of the content, or the content is compatible with the project’s license and is marked as such
> - You have the rights necessary to share the content
> - The content may be provided under the project’s license

***Before any code can be written, consider these core tenets of CCMath as you work on the project.***

### Core Tenets of CCMath

1. Code should be evaluable at compile time when possible and great effort should be taken to comply with this (e.g., `constexpr` and `static_assert`).
2. Match the behavior of the standard library as closely as possible, with no compromises on accuracy.
3. Strive for performance comparable to (or no worse than half the speed of) the standard library.
4. Ensure code can work with `static_assert`.
5. Favor generic solutions over platform-specific ones, when possible.
6. Mimic the behavior of compilers we support (particularly relevant for `ccm::fpclassify`).
7. Provide comprehensive tests covering all edge cases for any new code.
8. Document thoroughly with Doxygen, ensuring clarity and completeness.
9. Follow the same API layout as the standard library. Refer to [cppreference](https://en.cppreference.com/w/cpp/header/cmath) for guidance on `<cmath>'s api.

### Your First Code Contribution

If you’re ready to make your first contribution to CCMath, here’s a basic workflow you can follow:

1. **Fork and Clone**
  - Fork the CCMath repository on GitHub to your personal account.
  - Clone your fork locally:
    ```bash
    git clone https://github.com/<your-username>/ccmath.git
    cd ccmath
    ```

2. **Check Out the `dev` Branch**
  - We recommend making changes off of the `dev` branch:
    ```bash
    git checkout dev
    ```

3. **Set Up Your Environment**
  - Make sure you have the prerequisites installed (CMake, Ninja, Git, and a compatible compiler).
  - Install any other dependencies specific to your platform (for example, using `apt`, `brew`, or `choco`).

4. **Create a Feature Branch**
  - Create a new branch for your proposed changes:
    ```bash
    git checkout -b feature/my-new-feature
    ```

5. **Implement Your Changes**
  - Edit or add source/header files under `ccmath/`.
  - Remember the [Core Tenets of CCMath](#core-tenets-of-ccmath)—aim for constexpr-friendly, standards-compliant, well-documented, and well-tested code.

6. **Build and Test Locally**
  - Use the provided CMake Presets to configure, build, and test:
    ```bash
    cmake --preset=ninja-gcc-debug
    cmake --build --preset=build-ninja-gcc-debug
    ctest --preset=test-ninja-gcc-debug --output-on-failure
    ```
  - If you encounter any build or test failures, consult the project’s [Issues](https://github.com/Rinzii/ccmath/issues) or open a new one if needed.

7. **Document Your Code**
  - Add or update Doxygen comments where appropriate to ensure comprehensive coverage of new or modified functions.
  - If applicable, include references or code examples in docstrings.

8. **Commit and Push**
  - Once your changes pass locally, commit them with a meaningful message:
    ```bash
    git add .
    git commit -m "Implement new math function with tests"
    ```
  - Push your branch to your fork:
    ```bash
    git push -u origin feature/my-new-feature
    ```

9. **Open a Pull Request**
  - On GitHub, open a Pull Request (PR) from your branch to `Rinzii/ccmath`’s `dev` branch.
  - Provide a clear title and description of your changes.
  - Describe any related issue references (if applicable) and how you tested your changes.

10. **Code Review**
- A maintainer will review your PR. If changes are requested, address them and push updates to the same feature branch.
- Once everything is approved, your PR will be merged into `dev`.

That’s it! Thank you for contributing to CCMath. If you have any questions or run into any problems, don’t hesitate to open an issue or add a comment in your PR. We appreciate your help in making CCMath a robust, efficient, and user-friendly math library.


## How to manually build CCMath with the command line

### Prerequisites
- [CMake 3.18+](https://cmake.org/download/)
- [Git](https://git-scm.com/)
- [Ninja](https://ninja-build.org/) (for Linux & macOS
- [Clang](https://clang.llvm.org/) or [GCC](https://gcc.gnu.org/) (for Linux & macOS)
- [Visual Studio 22](https://visualstudio.microsoft.com/) (for Windows)

### How to clone project from GitHub

Step 1: Clone the repository
```bash
git clone https://github.com/Rinzii/ccmath.git
```

Step 2: (recommended) Check out the dev branch
```bash
git checkout dev
```

## Building on Linux & macOS

1. Pick a preset for your compiler and configuration.
  - Examples (see `CMakePresets.json` for the full list):
    - `ninja-gcc-debug`
    - `ninja-gcc-release`
    - `ninja-clang-debug`
    - `ninja-clang-release`
    - `ninja-clang-libcpp-relwithdebinfo`
  - If you need a specific C++ standard, pass `-DCMAKE_CXX_STANDARD=17` (or 20, etc.) after specifying the preset.
2. Configure using one of the presets (example shown uses GCC Debug with C++17):
   ```bash
   cmake --preset=ninja-gcc-debug -DCMAKE_CXX_STANDARD=17
    ```
3. Build the project
   ```bash
   cmake --preset=ninja-gcc-debug -DCMAKE_CXX_STANDARD=17
    ```
   Or, if you prefer a single step after configuring:
  ```bash
  cmake --preset=ninja-gcc-debug -DCMAKE_CXX_STANDARD=17
  ```
  You can also specify `--config Debug` or `--config Release` if needed
4. Run the tests
  ```bash
  ctest --preset=test-ninja-gcc-debug --output-on-failure
  ```
  Or, if you prefer a one-liner after configuring and building:
  ```bash
  ctest -C Debug --output-on-failure
  ```
  (Ensure you built a Debug config if you’re testing Debug.)

## Building on Windows (Visual Studio 2022)

1. Choose a preset, for example:
  - `vs22-debug`
  - `vs22-release`
  - `vs22-clang-debug`

2. Configure:
   ```powershell
   cmake --preset=vs22-debug -DCMAKE_CXX_STANDARD=17
    ```
3. Build using the corresponding build preset:
    ```powershell
    cmake --build --preset=vs22-debug
      ```
   Or, if you prefer a single step after configuring:
    ```powershell
    # In the same directory where you ran cmake --preset=...
    cmake --build . --config Debug
    ```
4. Run the tests
    ```powershell
    ctest --preset=test-vs22-debug --output-on-failure
    ```
    Or, if you prefer a one-liner after configuring and building:
    ```powershell
    ctest -C Debug --output-on-failure
    ```

## Additional Notes

- The exact preset names may differ in your local `CMakePresets.json`. Check that file for the list of available presets.
- Presets for coverage, sanitizer builds, or specialized compilers like Intel may also be available (for example, `ninja-intel-relwithdebinfo` or `ninja-gcc-ccache-debug`).
- If you run into issues, confirm that your CMake, Ninja, and compiler versions are current and supported.
- See our [Contribution Guidelines](CONTRIBUTING.md) for more detailed information on contributing and troubleshooting.
