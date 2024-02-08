#include<iostream>
#include<chrono>

using namespace std;
using namespace std::chrono;

template<typename T>
struct Matrix{
    private:
    T* ptr_to_data=nullptr;
    public:
    int n,m;
    Matrix(int _n, int _m, rand_init=false):n(_n),m(_m){
        allocate_memory(rand_init);
    }
    Matrix(const Matrix& other): n(other.n), m(other.m){
        ptr_to_data=new T[n*m];
        if(other.ptr_to_data)
            memcpy(ptr_to_data,other.ptr_to_data,n*m*sizeof(T));
    }
    Matrix& operator=(const Matrix& other){
        if(this==&other) return *this;
        if(n*m!=other.n*other.m){
            delete[]ptr_to_data;
            n=other.n;
            m=other.m;
            allocate_memory(false);
        }
        if(other.ptr_to_data)
            memcpy(ptr_to_data,other.ptr_to_data,n*m*sizeof(T));
        return *this;
    }
    Matrix(Matrix&& other):n(other.n),m(other.m), ptr_to_data(other.ptr_to_data){other.ptr_to_data=nullptr;}

    Matrix& operator= (Matrix&& other){
        delete[]ptr_to_data;
        n=other.n;
        m=other.m;
        ptr_to_data=other.ptr_to_data;
        other.ptr_to_data=nullptr;
        return *this;
    }
    inline const T operator() (int i, int j) const {return ptr_to_data[i*m+j];}
    inline T& operator()(int i, int j) {return ptr_to_data[i*m+j];}
    Matrix get_submatrix(int i_start, int i_end, int j_start, int j_end) const {
        int submatrix_n=i_end-i_start;
        int submatrix_m=j_end-j_start;

        Matrix<T> tmp(submatrix_n, submatrix_m);
        #pragma omp parallel for default(shared)
        for(int i=0;i<submatrix_n;i++){
            for(int j=0;j<submatrix_m;j++)
                tmp(i,j)=this->operator()(i+i_start,j+j_start);
    }
        return std::move(tmp);
    }
    void set_submatrix(const Matrix& SM, int i_start, int j_start){
        for(int i=0;i<SM.n;i++){
            for(int j=0;j<SM.m;j++)
                this->operator()(i+i_start,j+j_start)=SM(i,j);
        }
        cout<<endl;
    }
    void print() const{
        for(int i=0;i<SM.n;i++){
            for(int j=0;j<SM.m;j++)
                this->operator()(i,j)<< " ";
        }
        cout<<endl;
    }
private:
    void allocate_memory(bool rand_init){
        if(ptr_to_data) return;
        ptr_to_data=new T[n*m];
        if(rand_init){
            #pragma omp parallel for
            for(int i=0;i<n*m;i++){
                ptr_to_data[i]=(T)rand()/RAND_MAX;
            }
        }
    }
};
template<typename T>
Matrix<T> operator+(const Matrix<T>& A, const Matrix<T>& B) {
    Matrix<T> C(A.n, A.m);
    for (int i=0; i<A.n; i++) {
        for (int j=0; j<A.m; j++) {
            C(i, j) = A(i, j) + B(i, j);
        }
    }
    return std::move(C);
}

template<typename T>
Matrix<T> operator-(const Matrix<T>& A, const Matrix<T>& B) {
    Matrix<T> C(A.n, A.m);
    for (int i=0; i<A.n; i++) {
        for (int j=0; j<A.m; j++) {
            C(i, j) = A(i, j) - B(i, j);
        }
    }
    return std::move(C);
}

template<typename T>
Matrix<T> matmul(const Matrix<T>& A, const Matrix<T>& B, 
    bool use_omp = true) {
    Matrix<T> C(A.n, B.m);
    #pragma omp parallel for default(shared) if(use_omp)
    for (int i=0; i<C.n; i++) {
        for (int j=0; j<C.m; j++) {
            T cij = 0;
            for (int k=0; k<A.m; k++) 
                cij += A(i, k) * B(k, j);
            C(i, j) = cij;
        }
    }
    return std::move(C);
}
template<typename T>
Matrix<T> matmul_strassen(const Matrix<T>& X, const Matrix<T>& Y){
    int n=X.n;
    Matrix<T> Z(n,n);
    
}