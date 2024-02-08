#include<iostream>
#include<omp.h>
#include<vector>
#include<chrono>

using namespace std::chrono;
using namespace std;

template<typename T>
struct Cvor{
    T x;
    Cvor<T>* levi;
    Cvor<T>* desni;
    Cvor(T _x, Cvor<T>*_levi=nullptr, Cvor<T>*_desni=nullptr):x(_x),levi(levi),desni(desni){}
    Cvor(Cvor<T>& drugi ): x(drugi.x) {
        if (drugi.levi)
            this->levi=new Cvor(drugi.levi);
        if(drugi.desni)
            this->desni=new Cvor(drugi.desni);

    }
    ~Cvor(){
        if(this->levi){
            delete this->levi;
            this->levi=nullptr;
        }
        if(this->desni){
            delete this->desni;
            this->desni=nullptr;
        }
    }
};

template<typename T>
void prebroji(Cvor<T>* glava, int* rezultat, int nivo=0)
{
    int br_levo=0;
    int br_desno=0;
    if(!glava)
    {
        *rezultat=0;
        return;
    }

    #pragma omp parallel if(nivo==0) shared(br_levo, br_desno) num_threads(8)
    {
       #pragma omp single nowait
        {
            #pragma omp task shared(br_levo) if (nivo<5)
            {
                printf("Thread %d: Calling prebroji on left subtree (level %d)\n", omp_get_thread_num(), nivo);
                prebroji(glava->levi, &br_levo, nivo+1);
            }

            #pragma omp task shared(br_desno) if (nivo<5)
            {
                printf("Thread %d: Calling prebroji on right subtree (level %d)\n", omp_get_thread_num(), nivo);
                prebroji(glava->desni, &br_desno, nivo+1);
            }
        }
        #pragma omp barrier
        #pragma omp single
        *rezultat=br_levo+br_desno+1;
    }
}
int main(){
    using T=int;
    int n=1<<10;
    vector<Cvor<T>*> tren_nivo(n);
    vector<Cvor<T>*> sled_nivo(2*n,nullptr);
    while(n>0)
    {
        for(int i=0;i<n;i++){
            tren_nivo[i]=new Cvor<T>(i, sled_nivo[i*2], sled_nivo[i*2+1]);
        }
        n /= 2;
        sled_nivo.clear();
        sled_nivo = tren_nivo;
        tren_nivo.resize(n);
    }
    Cvor<T>* glava = sled_nivo[0];
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