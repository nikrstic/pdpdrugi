#include<iostream>
#include<omp.h>
#include<math.h>
using namespace std;
template<typename T>
bool donjetrougaona(T** matrix, int n,int m, double eps=1e-6){
    int nule=0;
    
    for(int i=0;i<n;i++){
        #pragma omp parallel for reduction(+:nule)
        for(int j=0;j<i;j++){
            cout<<nule;

            if(abs(matrix[i][j])>eps)
                nule+=matrix[i][j];
        }
    }
    return nule==0;
}

template<typename T>
void rowTransform(T**matrix, int n, int i, int j)
{
    T pivot=matrix[i][i];
    T pivot1= matrix[j][i];
    T result;
    if(pivot!=0){
         result= (pivot1/pivot)*(-1);
    }
    for(int k=0;k<n;k++){
        matrix[i][k]*=result;
        matrix[j][k]+=matrix[i][k];
    } 
}
template<typename T>
void gausszordanova(T** matrix, int n)
{
    T** jedinicna=new T*[n];
    for(int i=0;i<n;i++)
    {
        jedinicna[i] = new T[n];
    }
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(j==i){
                jedinicna[i][j]=1;
            }
            else{
                jedinicna[i][j]=0;
            }
        }
    }
    for(int i=0;i<n-1;i++)
    {
        for(int j=i+1;j<n;j++){
            rowTransform(matrix,n,i,j);
        }
    }
}
template<typename T>
void ispisi(T** matrix, int n)
{
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            cout<<matrix[i][j]<<" ";
        }
        cout<<endl;
    }
}
int main(){
    
    int n=4;
    int** matrix = new int*[n];
    for (int i = 0; i < n; i++) {
        matrix[i] = new int[n];
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
    cout<<donjetrougaona(matrix,n,n);
    cout<<endl<<endl;

    ispisi(matrix,n);
    gausszordanova(matrix,n);
    cout<<endl<<endl;
    ispisi(matrix,n);




    return 0;
}