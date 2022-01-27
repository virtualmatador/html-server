#include <exception>
#include <iostream>

int main(int argc, const char* argv[])
{
    int result = 0;
    try
    {
    }
    catch(const std::exception& e)
    {
        result = -1;
        std::cerr << e.what() << std::endl;
    }
    return result;
}
