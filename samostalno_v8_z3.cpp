#include<iostream>
#include<omp.h>
#include<chrono>

using namespace std;
using namespace std::chrono;

template<typename T>
void skalarni_proizvod(T* a, T* b,T*y,int n){
    T result=0;
    #pragma omp simd reduction(+:result) simdlen(4)
    for(int i=0;i<n;i++){
        result+=a[i]*b[i];
    }
    *y=result;
}
template<typename T>
T* matrica_vektor(T**a,T* b, T*y, int n, int m)
{
    #pragma omp parallel for 
    for(int i=0;i<n;i++)
    {
        skalarni_proizvod(a[i],b,&y[i],m);
    }
    
}


int main(){
     int n = 1 << 2;
    using T = double;
    T** A = new T*[n];
    for (int i=0; i< n; i++)
        A[i] = new T[n];
    T* x = new T[n];
    T* y = new T[n];

    #pragma omp parallel for
    for (int i=0; i<n; i++) {
        x[i] = (T)rand() / RAND_MAX;
        for (int j=0; j<n; j++)
            A[i][j] = (T)rand() / RAND_MAX;
    }
    auto start_t = high_resolution_clock::now();
    matrica_vektor(A, x, y, n, n);
    auto end_t = high_resolution_clock::now();
    auto dur = duration_cast<milliseconds>(end_t - start_t).count();
    cout << "Exec. time = " << dur << endl;
    delete []x;
    delete []y;
    for (int i=0; i<n; i++)
        delete[]A[i];
    delete[]A;
    return 0;










    return 0;
}