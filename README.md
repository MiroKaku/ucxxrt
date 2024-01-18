# [Universal C++ RunTime (UCXXRT)](https://github.com/mirokaku/ucxxrt)

[![Actions Status](https://github.com/MiroKaku/ucxxrt/workflows/build%20&%20publish/badge.svg)](https://github.com/MiroKaku/ucxxrt/actions)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/MiroKaku/ucxxrt/blob/main/LICENSE)
![Windows](https://img.shields.io/badge/Windows-7+-orange.svg)
![Visual Studio](https://img.shields.io/badge/Visual%20Studio-2022-purple.svg)
[![nuget](https://img.shields.io/nuget/v/ucxxrt)](https://www.nuget.org/packages/ucxxrt/)

* [简体中文](https://github.com/MiroKaku/ucxxrt/blob/main/README.zh-CN.md)

> UserMode support has been removed since March 29th, 2022. The final version known to support UserMode is [e2f159f8f](https://github.com/MiroKaku/ucxxrt/tree/e2f159f8f04a829359e3a057b70457121485b4dc). Please use [VC-LTL5](https://github.com/Chuyu-Team/VC-LTL5) instead in UserMode.

## 1. About

`ucxxrt` is an open source runtime library based on MSVC. The highlight of this project is its usability in *kernel-mode drivers*, and it provides you nearly the same experience as developing user-mode applications in C++.

Before `ucxxrt` was born, in order to use C++ STL in kernel-mode drivers, users have to craft their own template libraries (eg. [KTL](https://github.com/MeeSong/KTL), ustd, ...).
There are still several problems. Like it does not support C++ exceptions, and mainly, it costs very much time to implement new language features when C++ ISO standard updates.

Then `ucxxrt` was born.

### 1.1 How `ucxxrt` works

* When developing kernel-mode drivers, kernel-mode flag is disabled by using property sheets, forcibly making the compiler support C++ exceptions. Exception flag(`/EHsc`) is also enabled.

* Implements exception functions like `throw`, `catch`. Simulates the exception dispatcher in `throw` and handles the exception in callback functions.

### 1.2 Features

- [x] support x86, x64, ARM(experimental), ARM64(experimental).
- [x] support new/delete operators.
- [x] support C++ exception (/EHa, /EHsc).
- [x] support SAFESEH、GS (Buffer Security Check).
- [x] support STL (not fully).
- [x] support static objects.

[List of currently unsupported features ↓](#6-List-of-features-that-are-not-supported-at-this-time)

### 1.3 Example

> See project [unittest](https://github.com/MiroKaku/ucxxrt/blob/main/test/unittest.cpp) for more information.

```cpp
void Test$ThrowUnknow()
{
    try
    {
        try
        {
            try
            {
                throw std::wstring();
            }
            catch (int& e)
            {
                ASSERT(false);
                LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: %d\n", e);
            }
        }
        catch (std::string& e)
        {
            ASSERT(false);
            LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: %s\n", e.c_str());
        }
    }
    catch (...)
    {
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: ...\n");
    }
}

void Test$HashMap()
{
    auto Rand = std::mt19937_64(::rand());
    auto Map = std::unordered_map<uint32_t, std::string>();
    for (auto i = 0u; i < 10; ++i)
    {
        Map[i] = std::to_string(Rand());
    }

    for (const auto& Item : Map)
    {
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "map[%ld] = %s\n", Item.first, Item.second.c_str());
    }
}
```

## 2. How to use

**First, rename `DriverEntry` to `DriverMain`.**

### 2.1 Method 1 (recommended)

Right click on the project, select "Manage NuGet Packages".
Search for `ucxxrt`, choose the version that suits you, and then click "Install".

![nuget](https://raw.githubusercontent.com/MiroKaku/ucxxrt/main/readme/nuget.png)

### 2.2 Method 2

1. Download the latest package from [Releases](https://github.com/MiroKaku/ucxxrt/releases) and unzip it.

2. Add the property sheet `ucxxrt.props` to your project.

![usage](https://raw.githubusercontent.com/MiroKaku/ucxxrt/main/readme/use.gif)

## 3. How to build

IDE：Visual Studio 2022 latest version

* `git clone --recurse-submodules https://github.com/MiroKaku/ucxxrt.git`
* Open `ucxxrt.sln` and build.

## 4. Acknowledgements

Thanks to [JetBrains](https://www.jetbrains.com/?from=meesong) for providing free licenses such as [Resharper C++](https://www.jetbrains.com/resharper-cpp/?from=meesong) for my open-source projects.

[<img src="https://resources.jetbrains.com/storage/products/company/brand/logos/ReSharperCPP_icon.png" alt="ReSharper C++ logo." width=200>](https://www.jetbrains.com/?from=meesong)

## 5. References

* [Microsoft's C++ Standard Library](https://github.com/microsoft/stl)
* [Chuyu-Team/VC-LTL](https://github.com/Chuyu-Team/VC-LTL)
* [RetrievAL](https://github.com/SpoilerScriptsGroup/RetrievAL)

> Great thanks to these excellent projects. Without their existence, there would be no `ucxxrt` then.

## 6. List of currently unsupported features

- [ ] Thread Local Storage (TLS): thread_local、TlsAlloc ...
- [ ] std::filesystem
- [ ] std::chrono
- [ ] std::stacktrace_entry
- [ ] std::locale
- [ ] std::stream (std::fstream、std::iostream、std::cin、std::cout、std::cerr)
- [ ] std::future
- [ ] std::latch
- [ ] std::semaphore (std::counting_semaphore、std::binary_semaphore)
- [ ] ...
