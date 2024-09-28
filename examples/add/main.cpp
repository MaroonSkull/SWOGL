#include <swogl/swogl.h>

#include <iostream>

int main(int, char*[])
{
    auto sum = swogl::add(1, 1);
    std::cout << sum << std::endl;
    return 0;
}
