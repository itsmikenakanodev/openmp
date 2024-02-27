#include <iostream>
#include <omp.h>
#include <fmt/core.h>
#include <functional>
#include <fmt/ranges.h>
#include <random>
#include <chrono>



std::vector<int> ordenar(std::vector<int> datos){

    int num_threads;
    int block_size;
    std::vector<int> resul;

#pragma omp parallel default(none) shared(datos, num_threads, block_size, resul)
    {

#pragma omp master
        {
            num_threads = omp_get_num_threads();
            block_size = datos.size()/num_threads;
        }

        auto start = block_size * omp_get_thread_num();
        auto end= block_size * (omp_get_thread_num()+1);

        if(omp_get_thread_num() == num_threads-1){
            end = datos.size();
        }

        std::sort(datos.begin()+start, datos.begin()+end);

#pragma omp critical
        if(omp_get_thread_num()==0){
           std::copy(datos.begin()+start,datos.begin()+end,std::back_inserter(resul));
        }else{
            for(int i=0;i<resul.size();i++){
                if(resul[i]>datos[end]){
                    fmt::println("datos[i]={} < result[i]={}",datos[end],resul[i]);
                    resul.insert(resul.begin()+i,datos[end]);
                    end--;
                }else{
                    resul.insert(resul.begin()+(i+1),datos[end]);
                    datos.erase(datos.begin()+end);
                    end--;

                }
                if(start==end){
                    continue;
                }
            }
        }


    }

    return datos;

}

int main() {

    std::vector<int> a = {8, 23, 19, 67, 45, 35, 13, 1, 24, 13, 30, 3, 5, 20, 4, 60};

    std::vector<int> tmp;
    std::copy(a.begin(), a.end(), std::back_inserter(tmp));

    std::vector<int> b = ordenar(tmp);
    fmt::println("{}", b);
    return 0;
}