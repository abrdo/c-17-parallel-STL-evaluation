#include <range/v3/all.hpp>
#include <iostream>

using namespace ranges;

int main() 
{
    std::vector<int> a1{15, 7, 3,  5};
    std::vector<int> a2{ 1, 2, 6, 21};
    sort(view::zip(a1, a2), std::less<>{}, &std::pair<int, int>::first); 
    std::cout << view::all(a1) << '\n';
    std::cout << view::all(a2) << '\n';
}
