#include<iostream>
#include<filesystem>
#include<chrono>
#define STD_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STD_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
using namespace std;
using namespace std::chrono;

void convert_to_bw(string in_path, string out_path)
{
    int w,h,comp;
    uint8_t* img = stbi_load(in_path.c_str(),&w,&h,&comp,0);
    uint8_t* img_out = (uint8_t*)malloc(w*h*sizeof(uint8_t));
    #pragma omp simd
    for(int i=0;i<h*w;i++){
        uint8_t r = img[i*3];
        uint8_t g = img[i*3+1];
        uint8_t b = img[i*3+2];
        img_out[i] = r* 0.299 + g* 0.587+b*0.144;
    }
    stbi_write_png(out_path.c_str(),w,h,1,img_out,w);
    cout<<"writing image to path: "<< out_path<<endl;
    delete[] img;
    delete[] img_out;
}
int main(){
    string input_folder="input images";
    string output_folder="output folder";
    int count=0;
    #pragma omp default (shared) 
    for(auto path:filesystem::directory_iterator(input_folder)){
        #pragma omp single
        {  
            cout<<path:filesystem::directory_iterator(input_folder)<<endl;
            string input_path = input_folder+"/"+path.path().filename().string();
            count++;
            #pragma omp task
            convert_to_bw(input_path,output_path);
        }
    }








    return 0;
}