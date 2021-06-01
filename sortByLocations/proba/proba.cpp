#include <iostream>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h>

int main(){
    std::vector<int> v =  { 0, 1, 2, 4, 6, 9 };
    auto ptr = std::lower_bound(v.begin(), v.end(), 5);

    std::cout << &ptr << std::endl;

    return 0;
}