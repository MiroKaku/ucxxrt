#include <ucxxrt.h>
#include <kext/kallocator.h>

#include <string>
#include <random>
#include <vector>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <system_error>
#include <thread>

#ifndef ASSERT
#  define ASSERT assert
#endif

#define LOG(Format, ...) DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "[ucxxrt] [" __FUNCTION__ ":%u]: " Format "\n", __LINE__, ## __VA_ARGS__)

namespace UnitTest
{
    template<typename T>
    using  Vector = std::vector<T, std::kallocator<T, PagedPool, 'tset'>>;
    static Vector<std::function<void()>> TestVec;


#define TEST(name) TEST_ ## name
#define TEST_PUSH(name) ::UnitTest::TestVec.emplace_back(TEST(name))


    class TEST(StaticObject)
    {
        ULONG* mData = nullptr;

    public:

        TEST(StaticObject)() noexcept
            : mData(new ULONG[1]{ 1 })
        {
            LOG("has called.");
        }

        ~TEST(StaticObject)() noexcept
        {
            LOG("has called.");

            ASSERT(mData[0] == 1);
            delete[] mData;
        }
    };

    static TEST(StaticObject) StaticObject;


    void TEST(Float2Int)()
    {
        // On x86, call _ftoui/_ftoui2/_ftoui3

        float Val1 = 1.6f;
        int   Val2 = static_cast<int>(Val1);

        ASSERT(Val2 == 1);
        LOG("1.6f == %d", Val2);
    }


    void TEST(ThrowInt)()
    {
        try {
            try {
                try {
                    throw 1;
                }
                catch (const int& Exception) {
                    LOG("catch exception: %d", Exception);
                }
            }
            catch (const std::string& Exception) {
                ASSERT(false);
                LOG("catch exception: %s", Exception.c_str());
            }
        }
        catch (...) {
            ASSERT(false);
            LOG("catch exception: ...");
        }
    }


    void TEST(ThrowObject)()
    {
        try {
            try {
                try {
                    throw std::string(__FUNCTION__);
                }
                catch (const int& Exception) {
                    ASSERT(false);
                    LOG("catch exception: %d", Exception);
                }
            }
            catch (const std::string& Exception) {
                LOG("catch exception: %s", Exception.c_str());
            }
        }
        catch (...) {
            ASSERT(false);
            LOG("catch exception: ...");
        }
    }


    void TEST(ThrowUnknow)()
    {
        try {
            try {
                try {
                    throw std::wstring(__FUNCTIONW__);
                }
                catch (const int& Exception) {
                    ASSERT(false);
                    LOG("catch exception: %d", Exception);
                }
            }
            catch (const std::string& Exception) {
                ASSERT(false);
                LOG("catch exception: %s", Exception.c_str());
            }
        }
        catch (...) {
            LOG("catch exception: ...");
        }
    }

    void TEST(Map)()
    {
        auto Sand = LARGE_INTEGER();
        KeQueryTickCount(&Sand);

        auto Rand = std::mt19937_64(Sand.QuadPart);
        auto Map  = std::unordered_map<uint32_t, std::string>();

        for (auto Idx = 0u; Idx < 10; ++Idx) {
            Map[Idx] = std::to_string(Rand());
        }

        for (const auto& [Idx, Val] : Map) {
            LOG("map[%ld] = %s", Idx, Val.c_str());
        }
    }


    template<typename T>
    class TEST(InitializerListObject)
    {
        using iterator               = typename std::vector<T>::iterator;
        using const_iterator         = typename std::vector<T>::const_iterator;
        using reverse_iterator       = typename std::vector<T>::reverse_iterator;
        using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

        std::vector<T> mVec;

    public:
        TEST(InitializerListObject)(std::initializer_list<T> List)
            : mVec(List)
        {
        }

        [[nodiscard]] const_iterator begin() const noexcept
        {
            return mVec.begin();
        }

        [[nodiscard]] iterator end() noexcept
        {
            return mVec.end();
        }

        [[nodiscard]] const_iterator cbegin() const noexcept
        {
            return mVec.cbegin();
        }

        [[nodiscard]] const_iterator cend() const noexcept
        {
            return mVec.cend();
        }
    };

    void TEST(InitializerList)()
    {
        auto Vec = TEST(InitializerListObject) { 0, 1, 2, 3, 4 };

        int Idx = 0;
        for (const auto Val : Vec) {
            LOG("%d", Val);

            ASSERT(Idx == Val); ++Idx;
        }
    }

    std::unordered_map<std::string, ULONG_PTR> TEST(StaticObjectInitializer) =
    {
        { "1", 1 },
        { "2", 2 },
        { "3", 3 },
        { "4", 4 },
        { "5", 5 },
    };


    void TEST(SystemErrorCode)()
    {
        std::error_code Code(STATUS_INVALID_PARAMETER, std::system_category());
        LOG("%s", Code.message().c_str());
    }


    void TEST(Realloc)()
    {
        const auto Buffer1 = static_cast<int*>(malloc(sizeof(int)));

        if (Buffer1) {
            *Buffer1 = 123;

            const auto Buffer2 = static_cast<size_t*>(realloc(Buffer1, sizeof(size_t)));
            if (Buffer2) {
                ASSERT(*Buffer2 == 123);
                free(Buffer2);
            }
            else {
                free(Buffer1);
            }
        }
    }

    // compile with: /EHa
    void TEST(SEH)()
    {
        try {
            constexpr int InvalidAddress = 0;
            *reinterpret_cast<void**>(InvalidAddress) = TEST(SEH);
        }
        catch (...) {
            LOG("Caught a C exception");
        }
    }

    // compile with: /EHa
    class TEST(StructExceptionTranslator)
    {
        unsigned int mCode = 0;
    public:
        TEST(StructExceptionTranslator) () = delete;

        TEST(StructExceptionTranslator)(const TEST(StructExceptionTranslator)& Other) noexcept
            : mCode(Other.mCode)
        {
        }

        TEST(StructExceptionTranslator)(TEST(StructExceptionTranslator)&& Other) noexcept
            : mCode(Other.mCode)
        {
            Other.mCode = 0;
        }

        explicit TEST(StructExceptionTranslator)(const unsigned int Code) noexcept
            : mCode(Code)
        {
        }

        [[nodiscard]] unsigned int GetCode() const noexcept
        {
            return mCode;
        }
    };

    void TEST(SETranslate)()
    {
        _set_se_translator([](const unsigned int Code, _EXCEPTION_POINTERS* /*Exception*/) {
            LOG("In SE Translator.");
            throw TEST(StructExceptionTranslator)(Code);
        });

        try {
            constexpr int InvalidAddress = 0;
            *reinterpret_cast<void**>(InvalidAddress) = TEST(SETranslate);
        }
        catch (const TEST(StructExceptionTranslator)& Exception) {
            LOG("Caught a __try exception, error 0x%08X.", Exception.GetCode());
        }
    }

    std::mutex              TEST(Mutex);
    std::condition_variable TEST(ConditionVariable);
    std::string             TEST(ThreadData);
    bool TEST(ThreadReady)      = false;
    bool TEST(ThreadProcessed)  = false;

    void TEST(Thread)()
    {
        auto Worker = std::thread([]
        {
            std::unique_lock Lock(TEST(Mutex));
            TEST(ConditionVariable).wait(Lock, [] { return TEST(ThreadReady); });

            LOG("Worker thread is processing data");
            TEST(ThreadData) += " after processing";

            TEST(ThreadProcessed) = true;
            LOG("Worker thread signals data processing completed");

            Lock.unlock();
            TEST(ConditionVariable).notify_one();
        });

        TEST(ThreadData) = "Example data";
        {
            std::lock_guard Lock(TEST(Mutex));
            TEST(ThreadReady) = true;
            LOG("main() signals data ready for processing");
        }
        TEST(ConditionVariable).notify_one();

        {
            std::unique_lock Lock(TEST(Mutex));
            TEST(ConditionVariable).wait(Lock, [] { return TEST(ThreadProcessed); });
        }
        LOG("Back in main(), data = %s", TEST(ThreadData).c_str());

        Worker.join();
    }

}

namespace Main
{
    EXTERN_C NTSTATUS DriverMain(const PDRIVER_OBJECT DriverObject, const PUNICODE_STRING Registry)
    {
        using namespace UnitTest;
        UNREFERENCED_PARAMETER(Registry);

        LOG("entry.");

        TEST_PUSH(Float2Int);
        TEST_PUSH(ThrowInt);
        TEST_PUSH(ThrowObject);
        TEST_PUSH(ThrowUnknow);
        TEST_PUSH(Map);
        TEST_PUSH(InitializerList);
        TEST_PUSH(SystemErrorCode);
        TEST_PUSH(Realloc);
        TEST_PUSH(SEH);
        TEST_PUSH(SETranslate);
        TEST_PUSH(Thread);

        for (const auto& Test : TestVec) {
            Test();
        }

        DriverObject->DriverUnload = [](auto)
        {
            LOG("exit.");
        };

        return 0l;
    }

}
