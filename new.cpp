#include <iostream>
#include <vector>
#include <thread>

using namespace std;


template <class T>
void compexch(T& a, T& b)
{

    if (b < a)
        swap(a, b);

}


template <class T>
void shuffle(std::vector<T> a, unsigned int l, unsigned int r)
{
    
    auto half = (unsigned int) (l + r) / 2;
    vector<T> tmp(a.size());
    unsigned int i, j;

    for (i = l, j = 0; i <= r; i += 2, j++){   
        tmp[i] = a[l + j];
        tmp[i + 1] = a[half + j + 1];}

    for (auto i = 0; i < tmp.size(); i++)
        a[i] = tmp[i];

}

template <class T>
void unshuffle(std::vector<T> a, unsigned int l, unsigned int r)
{

    auto half = (unsigned int) (l + r) / 2;
    vector<T> tmp(a.size());
    unsigned int i, j;

    for (i = l, j = 0; i <= r; i += 2, j++){
        tmp[l + j] = a[i];
        tmp[half + j + 1] = a[i + 1];}

    for (auto i = 0; i < tmp.size(); i++)
        a[i] = tmp[i];

}

template <class T>
void OddEvenMergeSort(std::vector<T> &a, unsigned int l, unsigned int r)
{

    if (r == l + 1)
        compexch(a[l], a[r]);
    if (r < l + 2)
        return;
    unshuffle(a, l, r);
    auto half = (unsigned int) (l + r) / 2;
    thread th1(OddEvenMergeSort, a, l, half);
    thread th2(OddEvenMergeSort, a, half + 1, r);
    th1.join();
    th2.join();

    shuffle(a, l, r);
    for (auto i = l + 1; i < r; i += 2)
        compexch(a[i], a[i + 1]);

    auto halfSize = (r - l + 1) / 2 - 1;

    for (int i = l + 1; i + halfSize < r; i++)
        compexch(a[i], a[i + halfSize]);

}

template <class T>
void printVector(std::vector<T> a)
{

    for (auto i = 0; i < a.size(); i++)
    {
        cout << a[i];
        if (i < a.size())
            cout << ", ";
    }
}


int main()
{

    vector<int> a = {1, 2, 3, 4}; 
    OddEvenMergeSort(a, 0, a.size() - 1);
    printVector(a);

    return 0;
}










