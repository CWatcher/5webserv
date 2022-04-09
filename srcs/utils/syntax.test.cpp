#include "syntax.hpp"
#include <iostream>
#include <vector>

enum_class(Enum)
    kValueZero = 0,
    kValueTwo = 2,
    kValueOne = 1
enum_class_end

int main()
{
    std::vector<Enum> v;
    v.push_back(Enum::kValueOne);
    v.push_back(Enum::kValueTwo);
    foreach(std::vector<Enum>, v, it)
        std::cout << *it << ' ';
    std::cout << std::endl;
    cforeach(std::vector<Enum>, v, it)
        std::cout << *it << ' ';
    std::cout << std::endl;
    rforeach(std::vector<Enum>, v, it)
        std::cout << *it << ' ';
    std::cout << std::endl;
    crforeach(std::vector<Enum>, v, it)
        std::cout << *it << ' ';
    std::cout << std::endl;
}
