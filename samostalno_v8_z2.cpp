#include<iostream>                          
#include<omp.h>
#include<vector>
using namespace std;


template<typename T>
T skalarni_proizvod(vector<T> a, vector<T> b){
    T result=0;
    #pragma omp parallel for simd reduction(+:result)
    for(int i=0;i<a.size();i++){
        result+=a.at(i)*b.at(i);
        cout<<a.at(i)*b.at(i)<<endl;
    }
    return result;
}

int main(){
    using T=int;
    vector<T> a;
    vector<T> b;
    a.push_back(3);
    a.push_back(4);
    a.push_back(5);
    b.push_back(5);
    b.push_back(4);
    b.push_back(3);
    T result =skalarni_proizvod(a,b);
    cout<<result<<endl;











    return 0;
}