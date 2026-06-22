#include <iostream>
#include <vector>

template <typename K, typename V>
struct B{
    K key;
    V value;
    int psl = -1;
};

template <typename K, typename V>
struct RobinHood{
    std::vector<B<K,V>> tab;
};


int main(){

}