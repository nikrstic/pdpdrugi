#include<iostream>
#include<vector>
#include<chrono>
#include<omp.h>
using namespace std;
using namespace std::chrono;
template<typename T>
struct BinarnoStablo{
    T data;
    BinarnoStablo<T>* levo;
    BinarnoStablo<T>* desno;
    public:
    BinarnoStablo(T data, BinarnoStablo<T>*levo,BinarnoStablo<T>*desno):data(data),levo(levo),desno(desno){};
    BinarnoStablo(T data):data(data), levo(nullptr),desno(nullptr){};
    BinarnoStablo operator=(BinarnoStablo<T> other){
        this->data=other.data;
        this->levo=other.levo;
        this->desno=other.desno;
    }

};

template<typename T>
void prebroji(BinarnoStablo<T>* koren, int *count,int nivo=0)
{   
    int br_levo=0;
    int br_desno=0;
    if(!koren){
        *count=0;
        return;
    } 
    #pragma omp parallel if(nivo==0) shared (br_levo, br_desno)
    {
        #pragma omp single nowait
        {
            #pragma omp task shared(br_levo) if(nivo<=5)
            prebroji(koren->levo,&br_levo,nivo+1);
            #pragma omp task shared(br_desno) if(nivo<=5)
            prebroji(koren->desno,&br_desno,nivo+1);               
        }
        #pragma omp barrier
        #pragma omp single
        *count=br_levo+br_desno+1;
    }
}

int main(){
        using T = int;
    int n = 1 << 10;
    vector<BinarnoStablo<T>*> tren_nivo(n);
    vector<BinarnoStablo<T>*> sled_nivo(2*n, nullptr);
    while(n > 0) {
        for (int i=0; i<n; i++) {
            tren_nivo[i] = new BinarnoStablo<T>(i, sled_nivo[i*2], 
                sled_nivo[i*2+1]);
        }
        n /= 2;
        sled_nivo.clear();
        sled_nivo = tren_nivo;
        tren_nivo.resize(n);
    }
    BinarnoStablo<T>* glava = sled_nivo[0];
    int br;
    auto start_t = high_resolution_clock::now();
    prebroji(glava, &br);
    auto end_t = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end_t-start_t)
                        .count();
    
    cout << "exec. time = " << duration
         << "ms.\t" << br << endl;
    return 0;
}