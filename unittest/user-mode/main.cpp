#include <Windows.h>
#include <iostream>
#include <ucxxrt.h>

int main()
{
    try
    {
        try
        {
            try
            {
                //throw 1;
                //throw std::string("123");
                throw std::wstring();
            }
            catch (int& e)
            {
                printf("Catch Exception: %d\n", e);
            }
        }
        catch (std::string & e)
        {
            printf("Catch Exception: %s\n", e.c_str());
        }
    }
    catch (...)
    {
        printf("Catch Exception: ...\n");
    }

    return 0;
}
