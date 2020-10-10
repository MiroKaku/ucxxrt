#define _KERNEL_MODE

#include <wdm.h>
#include <ntddk.h>
#include <ucxxrt.h>

#include <string>
#include <random>
#include <unordered_map>
#include <thread>

class Test
{
    ULONG* _Data;

public:

    Test()
        : _Data(new ULONG[1])
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Static Object: " __FUNCTION__ "\n");
    }

    ~Test()
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Static Object: " __FUNCTION__ "\n");
        delete[] _Data;
    }
};

static Test _test;

EXTERN_C VOID DriverUnload(PDRIVER_OBJECT /*aDriverObject*/)
{
    std::thread tmp([] {});
}

EXTERN_C NTSTATUS DriverMain(PDRIVER_OBJECT aDriverObject, PUNICODE_STRING aRegistry)
{
    NTSTATUS vResult = STATUS_SUCCESS;

    for (;;)
    {
        try
        {
            throw* aRegistry;
        }
        catch (UNICODE_STRING & e)
        {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exceptrion: %wZ\n", &e);
        }

        auto vRand = std::mt19937_64(rand());
        auto vMap  = std::unordered_map<uint32_t, std::string>();
        for (auto i = 0u; i < 100; ++i)
        {
            vMap[i] = std::to_string(vRand());
        }

        for (const auto& vItem : vMap)
        {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                "map[%ld] = %s\n", vItem.first, vItem.second.c_str());
        }

        aDriverObject->DriverUnload = DriverUnload;
        break;
    }

    return vResult;
}
