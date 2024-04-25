# Contribution Guidelines

> [!IMPORTANT]
> Currently the contribution guidelines are a work in progress. Please check back later for more information.

<!-- omit in toc -->
# Contributing to CCMath

First off, thanks for taking the time to contribute! ❤️

All types of contributions are encouraged and valued. See the [Table of Contents](#table-of-contents) for different ways to help and details about how this project handles them. Please make sure to read the relevant section before making your contribution. It will make it a lot easier for us maintainers and smooth out the experience for all involved. The community looks forward to your contributions. 🎉

> And if you like the project, but just don't have time to contribute, that's fine. There are other easy ways to support the project and show your appreciation, which we would also be very happy about:
> - Star the project
> - Refer this project in your project's readme
> - Mention the project at local meetups and tell your friends/colleagues

<!-- omit in toc -->
## Table of Contents

- [I Have a Question](#i-have-a-question)
- [I Want To Contribute](#i-want-to-contribute)
- [Your First Code Contribution](#your-first-code-contribution)


## I Have a Question

> If you want to ask a question, we assume that you have read the available [Documentation]().

Before you ask a question, it is best to search for existing [Issues](https://github.com/Rinzii/ccmath/issues) that might help you. In case you have found a suitable issue and still need clarification, you can write your question in this issue. It is also advisable to search the internet for answers first.

If you then still feel the need to ask a question and need clarification, we recommend the following:

- Open an [Issue](https://github.com/Rinzii/ccmath/issues/new).
- Provide as much context as you can about what you're running into.
- Provide project and platform versions (nodejs, npm, etc), depending on what seems relevant.

We will then take care of the issue as soon as possible.


## I Want To Contribute

> ### Legal Notice <!-- omit in toc -->
> When contributing to this project, you must agree that you have authored 100% of the content, that you have the necessary rights to the content and that the content you contribute may be provided under the project license.

***Before any code can be written consider these core tenets of CCMath as you work on the project.***

### Core Tenets of CCMath

* At all times should code produced for CCMath be evaluable at compile time. (e.g. `constexpr` & `static_assert`)
* Always aim to have the same output as the standard. Never compromise on accuracy.
* Always aim to have speed's comparable to the standard. (Aim for being no slower than 1/2 the speed of the standard if possible)
* Always aim to have your code work with `static_assert`.
* At all times should you prefer a generic solution over a platform dependent solution if possible.
* You should always mimic the behavior of every compiler we support. (This is most visible in `ccm::fpclassify`)
* Always have a comprehensive test suite for your implementation that covers all edge cases.
* Ensure your code is always properly documented with doxygen in a detailed manner.
* Always have the same API layout as the standard. (This can be cross-referenced from [cppreference](https://en.cppreference.com/w/))

### Your First Code Contribution
<!-- TODO
Add instructions for setting up your env, ide, and building the project
-->

WIP
