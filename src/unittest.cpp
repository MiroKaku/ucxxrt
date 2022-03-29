#include <ucxxrt.h>

#include <string>
#include <random>
#include <vector>
#include <functional>
#include <unordered_map>
#include <stdexcept>

#define LOG(Format, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[ucxxrt] " __FUNCTION__ ": " Format "\n", __VA_ARGS__)

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
        LOG("has called.");
    }

    ~Test$StaticObject()
    {
        LOG("has called.");

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
                LOG("catch Exception: %d", e);
            }
        }
        catch (std::string& e)
        {
            ASSERT(false);
            LOG("catch Exception: %s", e.c_str());
        }
    }
    catch (...)
    {
        ASSERT(false);
        LOG("catch Exception: ...");
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
                ASSERT(false);
                LOG("catch Exception: %d", e);
            }
        }
        catch (std::string& e)
        {
            LOG("catch Exception: %s", e.c_str());
        }
    }
    catch (...)
    {
        ASSERT(false);
        LOG("catch Exception: ...");
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
                ASSERT(false);
                LOG("catch Exception: %d", e);
            }
        }
        catch (std::string& e)
        {
            ASSERT(false);
            LOG("catch Exception: %s", e.c_str());
        }
    }
    catch (...)
    {
        LOG("catch Exception: ...");
    }
}

void Test$HashMap()
{
    auto Sand = LARGE_INTEGER();
    KeQueryTickCount(&Sand);

    auto Rand = std::mt19937_64(Sand.QuadPart);
    auto Map  = std::unordered_map<uint32_t, std::string>();
    for (auto i = 0u; i < 10; ++i)
    {
        Map[i] = std::to_string(Rand());
    }

    for (const auto& Item : Map)
    {
        LOG("map[%ld] = %s", Item.first, Item.second.c_str());
    }
}

template<typename T>
class Test$InitializerListObject
{
    using iterator               = typename std::vector<T>::iterator;
    using const_iterator         = typename std::vector<T>::const_iterator;
    using reverse_iterator       = typename std::vector<T>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

    std::vector<T> _vec;

public:
    Test$InitializerListObject(std::initializer_list<T> x)
        : _vec(x)
    {}

    [[nodiscard]] const_iterator begin() const noexcept {
        return _vec.begin();
    }

    [[nodiscard]] iterator end() noexcept {
        return _vec.end();
    }

    [[nodiscard]] const_iterator cbegin() const noexcept {
        return _vec.cbegin();
    }

    [[nodiscard]] const_iterator cend() const noexcept {
        return _vec.cend();
    }
};

void Test$InitializerList()
{
    auto v = Test$InitializerListObject{ 0, 1, 2, 3, 4 };

    int x = 0;
    for (const auto i : v)
    {
        ASSERT(i == x); ++x;
    }
}

std::unordered_map<std::string, ULONG_PTR> Test$StaticObjectInitializer =
{
    { "1", 1 },
    { "2", 2 },
    { "3", 3 },
    { "4", 4 },
    { "5", 5 },
};

EXTERN_C NTSTATUS DriverMain(PDRIVER_OBJECT aDriverObject, PUNICODE_STRING /*aRegistry*/)
{
    LOG("entry.");

    TEST(Test$Float2Int);
    TEST(Test$ThrowInt);
    TEST(Test$ThrowObject);
    TEST(Test$ThrowUnknow);
    TEST(Test$HashMap);
    TEST(Test$InitializerList);

    for (const auto& Test : TestVec)
    {
        Test();
    }

    aDriverObject->DriverUnload = [](PDRIVER_OBJECT)
    {
        LOG("exit.");
    };

    return 0l;
}
