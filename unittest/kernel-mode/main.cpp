#include <ntddk.h>
#include <wdm.h>
#include <ucxxrt.h>

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

        aDriverObject->DriverUnload = DriverUnload;
        break;
    }

    return vResult;
}
