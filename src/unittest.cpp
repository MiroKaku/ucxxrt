#if __has_include(<wdm.h>)
#define _KERNEL_MODE
#endif

#ifdef _KERNEL_MODE
#include <wdm.h>
#include <ntddk.h>
#else
#include <windows.h>
#include <assert.h>
#endif

#include <ucxxrt.h>

#include <string>
#include <random>
#include <vector>
#include <functional>
#include <unordered_map>

#ifdef _KERNEL_MODE
#define LOG(_0, _1, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, __VA_ARGS__)
#else
#define LOG(_0, _1, ...) printf(__VA_ARGS__)
#endif

static std::vector<std::function<void()>> TestVec;
#define TEST(f) TestVec.emplace_back(f)
#ifndef ASSERT
#  define ASSERT assert
#endif

class Test$StaticObject
{
    ULONG* _Data;

public:

    Test$StaticObject()
        : _Data(new ULONG[1]{ 1 })
    {
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Static Object: " __FUNCTION__ "\n");
    }

    ~Test$StaticObject()
    {
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Static Object: " __FUNCTION__ "\n");

        ASSERT(_Data[0] == 1);
        delete[] _Data;
    }
};

static Test$StaticObject _test;

void Test$Float2Int()
{
    // On x86, call _ftoui/_ftoui2/_ftoui3

    float f = 1.6f;
    auto  i = (uint32_t)f;

    ASSERT(i == 1);
    UNREFERENCED_PARAMETER(i);
}

void Test$ThrowInt()
{
    try
    {
        try
        {
            try
            {
                throw 1;
            }
            catch (int& e)
            {
                LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: %d\n", e);
            }
        }
        catch (std::string& e)
        {
            LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: %s\n", e.c_str());
        }
    }
    catch (...)
    {
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: ...\n");
    }
}

void Test$ThrowObject()
{
    try
    {
        try
        {
            try
            {
                throw std::string("123");
            }
            catch (int& e)
            {
                LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: %d\n", e);
            }
        }
        catch (std::string& e)
        {
            LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: %s\n", e.c_str());
        }
    }
    catch (...)
    {
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: ...\n");
    }
}

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
                LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: %d\n", e);
            }
        }
        catch (std::string& e)
        {
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
    for (auto i = 0u; i < 100; ++i)
    {
        Map[i] = std::to_string(Rand());
    }

    for (const auto& Item : Map)
    {
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "map[%ld] = %s\n", Item.first, Item.second.c_str());
    }
}

#ifdef _KERNEL_MODE
EXTERN_C NTSTATUS DriverMain(PDRIVER_OBJECT aDriverObject, PUNICODE_STRING /*aRegistry*/)
#else
EXTERN_C int main()
#endif
{
    TEST(Test$Float2Int);
    TEST(Test$ThrowInt);
    TEST(Test$ThrowObject);
    TEST(Test$ThrowUnknow);
    TEST(Test$HashMap);

    for (const auto& Test : TestVec)
    {
        Test();
    }

#ifdef _KERNEL_MODE
    aDriverObject->DriverUnload = [](PDRIVER_OBJECT)
    {
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Exit ucxxrt-test.\n");
    };
#endif

    return 0l;
}
