#include <iostream>
#include <vector>

int main() {
    // Violação de 'modernize-use-nullptr' (usando NULL em vez de nullptr)
    int* ptr = NULL;

    // Violação de 'performance-unnecessary-copy-initialization'
    std::vector<int> v = {1, 2, 3};
    for (auto x : v) {
        std::cout << x << std::endl;
    }
    ptr = &v[0];

    std::cout << *ptr << std::endl;

    return 0;
}
