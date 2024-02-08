#include<iostream>
#include<omp.h>
#include<cmath>
#include<chrono>
using namespace std;
using namespace std::chrono;

template<typename T>
bool donje_trougaona_v1(T** matrix, int n, int m, T eps=1e-6)
{
    T count=0;
    for(int i=0;i<n;i++)
    {
        #pragma omp parallel for reduction( + : count) 
        for(int j=0;j<i;j++){
            count+=matrix[i][j];
        }
    }
    return count==0;

}
template<typename T>
void broj_razl_nula(T** matrix, int n,int& non_zero, T eps=1e-6){
    for(int i=0;i<n;i++){
        if(abs(matrix[n][i])<eps){
            non_zero++;
        }
    }

}

template<typename T>
bool donje_trougaona_v2(T** matrix, int n, T eps=1e-6){
    int non_zero=0;
    #pragma omp parallel shared(matrix, n, eps,non_zero)
    {
        #pragma omp single nowait
        for(int i=0;i<n;i++){
            #pragma omp task
            //do umesto i
            broj_razl_nula(matrix,i,non_zero);

        }
    }
    return non_zero==0;
}
template<typename T>
void transform(T** matrix, T pivot, int row, int col, int n, T* b, int j)
{
    T koeficijent = -1 * pivot / matrix[row][j];
    for (int i = 0; i < n; i++)
    {
        matrix[row][i] = matrix[row][i] - matrix[col][i] * koeficijent;
       
    }
     b[row] = b[row] - b[col] * koeficijent;
}


template<typename T>
void gauss_elimination(T** matrix, T* x, int n, T* b)
{
    #pragma omp parallel shared(matrix, x, n, b)
    {
        int i = 0;
        while (i < n) {
            #pragma omp single
            {
                int j = i;
                T pivot = matrix[i][j];
                for (int k = 0; k < n; k++) {
                    #pragma omp task
                    {
                        transform(matrix, pivot, i, k, n, b, j);
                    }
                }
            }
            #pragma omp single
            i++;
        }

        #pragma omp for
        for (int i = 0; i < n; i++) {
            x[i] = b[i] / matrix[i][i];
        }
    }
}
template<typename T>
void transform_row(T** A, T*b, int n, int row_i, int row_j, T factor) {
    /*
    Osnovna operacija koju ćemo vršiti je dodavanje nekog i-tog reda nekom j-tom redu,
    pri čemu je i-ti red skaliran (argument factor).
    */
    for (int j=row_i; j<n; j++) // matematički je sasvim u redu da j krene od nule,
    // ali mi znamo da će pre kolone row_i u redu row_i biti samo nule tako da množenje
    // nule nekim brojem i dodavanje (uzgred takođe nuli) nema nikakav efekat, pa možemo
    // krenuti od kolone row_i
        A[row_j][j] += A[row_i][j] * factor;
    b[row_j] += b[row_i] * factor;
}

template<typename T>
void gauss_elimination1(T** matrix, T* b, T* x, int n, int num_threads = 8) {
    int pivot_i = 0;
    #pragma omp parallel num_threads(num_threads) shared(pivot_i)
    {
        while (pivot_i < n) {
            // Ista ideja kao i ranije: jedna nit kreira zadatke, a ceo tim niti ih izvršava.
            #pragma omp single
            {
                int pivot_j = pivot_i;
                T pivot = matrix[pivot_i][pivot_j];
                for (int i=pivot_i+1; i<n; i++) {
                    // potrebno je dodati i-toj vrsti
                    // vrstu pivot_i pomnozenu sa
                    // -1* A[i][pivot_i] / pivot
                    T factor = -1*matrix[i][pivot_i] / pivot;
                    #pragma omp task if(n-pivot_i > 256)
                    transform_row(matrix, b, n, pivot_i, i, factor);
                }
            }   
            #pragma omp single
            pivot_i++;
        }
    }
    // Računanje nepoznatih x zamenom vrednosti. 
    for (int i=n-1; i>=0; i--) {
        x[i] = b[i] / matrix[i][i];
        #pragma omp parallel for
        for (int row_above = i-1; row_above >= 0; row_above--)
            b[row_above] -= x[i] * matrix[row_above][i];
    }
}





int main(){
    using T=double;
    int n=4;
    T* x=new T[n];
    T* b=new T[n];
    T** matrix=new T*[n];
    for(int i=0;i<4;i++){
        matrix[i] = new T[n];
    }
     matrix[0][0] = 4;
    matrix[0][1] = 1;
    matrix[0][2] = 2;
    matrix[0][3] = -3;
    matrix[1][0] = -3;
    matrix[1][1] = 3;
    matrix[1][2] = -1;
    matrix[1][3] = 4;
    matrix[2][0] = -1;
    matrix[2][1] = 2;
    matrix[2][2] = 5;
    matrix[2][3] = 1;
    matrix[3][0] = 5;
    matrix[3][1] = 4;
    matrix[3][2] = 3;
    matrix[3][3] = -1;
    b[0] = -16;
    b[1] = 20;
    b[2] = -4;
    b[3] = -10;
    for(int i=0;i<4;i++)
    {
        cout<< b[i]<< " ";
    }
    cout<<endl<<endl;
    //gauss_elimination(matrix,x,n,b);
    gauss_elimination1(matrix,b,x,n);
    for(int i=0;i<4;i++)
    {
        cout<< x[i]<< " ";
    }
    return 0;
}

