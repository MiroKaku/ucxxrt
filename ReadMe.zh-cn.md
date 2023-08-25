# [Universal C++ RunTime (UCXXRT)](https://github.com/mirokaku/ucxxrt) - 通用 C++ 运行时库

[![Actions Status](https://github.com/MiroKaku/ucxxrt/workflows/build%20&%20publish/badge.svg)](https://github.com/MiroKaku/ucxxrt/actions)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/MiroKaku/ucxxrt/blob/main/LICENSE)
![Windows](https://img.shields.io/badge/Windows-7+-orange.svg)
![Visual Studio](https://img.shields.io/badge/Visual%20Studio-2022-purple.svg)
[![nuget](https://img.shields.io/nuget/v/ucxxrt)](https://www.nuget.org/packages/ucxxrt/)

* [English](https://github.com/MiroKaku/ucxxrt/blob/main/README.md)

> 03/29/2022 开始移除了 UserMode 支持。最后支持 UserMode 的版本为 [e2f159f8f](https://github.com/MiroKaku/ucxxrt/tree/e2f159f8f04a829359e3a057b70457121485b4dc), UserMode 请使用 [VC-LTL5](https://github.com/Chuyu-Team/VC-LTL5)

## 1. 关于

ucxxrt 是一个基于微软的 MSVC 修改的开源运行时库，最大的亮点就是可以在驱动中使用，使得驱动开发拥有同应用开发一样的C++体验。

在 ucxxrt 诞生之前，在驱动中使用 C++ 是由自己按需实现模板库（[KTL](https://github.com/MeeSong/KTL)、ustd、...）  
但是存在几个问题，如不支持异常。其中最主要的问题是每次新标准出来都要自己去实现一遍新特性，很浪费时间。至此，ucxxrt 就诞生了。

### 1.1 原理

* 在驱动模式下，通过属性表强行关闭内核模式标识，让编译器支持异常。同时开启异常选项 (`/EHsc`)
* 实现 `throw`、`catch` 等异常函数。在 `throw` 函数中，直接模拟分发回调到异常处理函数。

### 1.2 特性

- [x] 支持 x86、x64、ARM（实验性）、ARM64（实验性）
- [x] 支持 new/delete
- [x] 支持 C++ 异常 (/EHa、/EHsc)
- [x] 支持 SAFESEH、GS (Buffer Security Check)
- [x] 支持 STL (部分)
- [x] 支持 静态对象

[暂不支持的特性列表↓](#6-暂不支持的特性列表)

### 1.3 例子

> 项目 [unittest](https://github.com/MiroKaku/ucxxrt/blob/main/test/unittest.cpp) 查看更多信息。

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

## 2. 怎样使用

**首先 `DriverEntry` 需要改为 `DriverMain`。**

### 2.1 方法一（推荐）

右键单击该项目并选择“管理 NuGet 包”，然后搜索`ucxxrt`并选择适合你的版本，最后单击“安装”。

![nuget](https://github.com/MiroKaku/ucxxrt/blob/main/readme/nuget.png?raw=true)

### 2.2 方法二

 1. 从 [release](https://github.com/MiroKaku/ucxxrt/releases) 下载最新包并解压。
 2. 在 Visual Studio 中，你可以打开属性管理器（视图 - 属性管理器），然后右键添加现有属性表，然后选择 `ucxxrt.props` 即可。

![使用方法](https://github.com/MiroKaku/ucxxrt/blob/main/readme/use.zh-cn.gif?raw=true)

## 3. 怎样编译

IDE：Visual Studio 2019 或更高版本 (编译 ARM/ARM64 需要 Visual Studio 2022)

* `git clone --recurse-submodules https://github.com/MiroKaku/ucxxrt.git`
* 打开 `ucxxrt.sln` 进行编译。

## 4. 鸣谢

> [IntelliJ IDEA](https://zh.wikipedia.org/zh-hans/IntelliJ_IDEA) 是一个在各个方面都最大程度地提高开发人员的生产力的 IDE。

特别感谢 [JetBrains](https://www.jetbrains.com/?from=meesong) 为开源项目提供免费的 [Resharper C++](https://www.jetbrains.com/resharper-cpp/?from=meesong) 等 IDE 的授权

[<img src="https://resources.jetbrains.com/storage/products/company/brand/logos/ReSharperCPP_icon.png" alt="ReSharper C++ logo." width=200>](https://www.jetbrains.com/?from=meesong)

## 5. 引用参考

* [Microsoft's C++ Standard Library](https://github.com/microsoft/stl)
* [Chuyu-Team/VC-LTL](https://github.com/Chuyu-Team/VC-LTL)
* [RetrievAL](https://github.com/SpoilerScriptsGroup/RetrievAL)

> 非常感谢这些优秀的项目，没有它们的存在，就不会有 ucxxrt。

## 6. 暂不支持的特性列表

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
