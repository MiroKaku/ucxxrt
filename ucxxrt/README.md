# [Universal C++ RunTime (UCXXRT)](https://github.com/mirokaku/ucxxrt)

[![Actions Status](https://github.com/MiroKaku/ucxxrt/workflows/CI/badge.svg)](https://github.com/MiroKaku/ucxxrt/actions)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/MiroKaku/ucxxrt/blob/master/LICENSE)
[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
![Windows](https://img.shields.io/badge/Windows-7+-orange.svg)
![Visual Studio](https://img.shields.io/badge/Visual%20Studio-2019-purple.svg)

* [简体中文](ReadMe.zh-cn.md)

> UserMode support will be removed starting 03/29/2022. The last version to support UserMode is [e2f159f8f](https://github.com/MiroKaku/ucxxrt/tree/e2f159f8f04a829359e3a057b70457121485b4dc), UserMode please use [VC-LTL5](https://github.com/Chuyu-Team/VC-LTL5)

## 1. About

ucxxrt is a open source rutime library which based on MSVC.The highlight of this project is that it can be used in kernel-mode drivers.  
It gives you the same experience as user-mode application development in C++ when developing kernel-mode drivers.

Before ucxxrt was born,in order to use C++ on kernel-mode drivers, I use  ([KTL](https://github.com/MeeSong/KTL)、ustd、...).  

But there are several problems，like it isn't support C++ exception and it cost much time on implementing new features which provided by the latest ISO,then ucxxrt was born.  

### 1.1 Principle  

* In kernel-mode driver mode,forced disable kernel-mode flag by using property sheet ,it makes the compiler support C++ exceptions.
* Implement the exception functions such as `throw`、`catch`.  Simulated the exception dispatcher in `throw`.  

### 1.2 Features

Kernel-mode：
- [x] support new/delete operators.  
- [x] support C++ exception (/EHsc).  
- [x] support SAFESEH、GS (Buffer Security Check).  
- [x] support STL (not fully).  
- [x] support static objects.  

[List of features that are not supported at this time↓](#5-List-of-features-that-are-not-supported-at-this-time)

### 1.3 Example

> See project [unittest](https://github.com/MiroKaku/ucxxrt/blob/master/src/unittest.cpp) for more Infomation.  

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

## 2. Compile

IDE：Visual Studio 2019 or higher

* `git clone https://github.com/MiroKaku/ucxxrt.git`
* Open `ucxxrt.sln` and compile

## 3. How to use

1. Copy `ucxxrt/ucxxrt` forder to you project directory.  
2. Add the property sheet `ucxxrt.props` to yor project.  

> !! note：Rename `DriverEntry` to `DriverMain`  

![usage](./readme/use.gif)

## 4. Reference and Acknowledgement

* [Microsoft's C++ Standard Library](https://github.com/microsoft/stl)
* [Chuyu-Team/VC-LTL](https://github.com/Chuyu-Team/VC-LTL)
* [RetrievAL](https://github.com/SpoilerScriptsGroup/RetrievAL)

> Thanks to these excellent projects for help me on developing ucxxrt.

## 5. List of features that are not supported at this time

- [ ] Thread Local Storage (TLS): thread_local、TlsAlloc ...
- [ ] std::thread
- [ ] std::filesystem
- [ ] std::chrono
- [ ] std::stacktrace_entry
- [ ] std::locale
- [ ] std::stream (std::fstream、std::iostream、std::cin、std::cout、std::cerr)
- [ ] std::mutex、std::shared_mutex
- [ ] std::future
- [ ] std::condition_variable
- [ ] std::latch
- [ ] std::semaphore (std::counting_semaphore、std::binary_semaphore)
- [ ] ...
