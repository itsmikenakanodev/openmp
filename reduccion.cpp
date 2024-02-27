#include <iostream>

#include <omp.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <vector>
#include <functional>

#define MAX_ELEMENTOS 1024

int main() {


    std::vector<float> datos(MAX_ELEMENTOS);

    for(int i=0;i<MAX_ELEMENTOS;i++){
        datos[i]=i+1;
    }
    //std::vector<int> datos = {0, 1, 2, 3, 4, 5, 6, 7,8};
    std::vector<int> tmp;

    //copiar los datos a otro vector
    std::copy(datos.begin(),datos.end(),std::back_inserter(tmp));

    int num_thread_inicial=std::ceil(datos.size()/2.0);
    //fijar el numeor de hilos inicial
    omp_set_num_threads(num_thread_inicial);
    fmt::println("threads: {}",num_thread_inicial);

    int suma;
#pragma omp parallel default(none) shared( suma, tmp, std::cout,datos)
    {

        int num_threads = omp_get_num_threads(); //obtener el numero de hilos
        int thread_id = omp_get_thread_num();

        //s=s>>1  es igual que s=s/2
        //falta poner que si es impar y es el ultimo no sumar nada
        for(int s=num_threads;s>1;s=std::ceil(s/2.0)){
            // 0  1  2  3
            if(thread_id<s) {
                if(datos.size()%2==0) {
                    //etapa1
                    //hilo 0     tmp[0]=tmp[0]+tmp[4]
                    //hilo 1     tmp[1]=tmp[1]+tmp[5]
                    //hilo 2     tmp[2]=tmp[2]+tmp[6]
                    //hilo 3     tmp[3]=tmp[3]+tmp[7]
                    //etapa 2
                    //hilo 0     tmp[0]=tmp[0]+tmp[4]
                    //hilo 1     tmp[1]=tmp[1]+tmp[5]
                    tmp[thread_id] = tmp[thread_id] + tmp[thread_id + s];

                }else{
                    //etapa1
                    //hilo 0     4 es diferente 7? si entonces -> tmp[0]=tmp[0]+tmp[4]
                    //hilo 1     5 es diferente 7? si entonces -> tmp[1]=tmp[1]+tmp[5]
                    //hilo 2     6 es diferente 7? si entonces -> tmp[2]=tmp[2]+tmp[6]
                    //hilo 3     7 es diferente 7? no entonces -> tmp[3] no cambia
                    if(thread_id+s != (2*s)-1){
                        tmp[thread_id] = tmp[thread_id] + tmp[thread_id + s];

                    }
                }
                //if(thread_id+s != (2s)-1){
                //}

                fmt::println("thread_{} [{},{}]", thread_id, thread_id, thread_id + s);
            }
#pragma omp barrier
            if(thread_id==0){
                fmt::println("fin etapa {}",s);
            }
        }
        if(thread_id==0){
            suma=tmp[0]+tmp[1];
        }

    }

    fmt::println("{}",datos);
    fmt::println("suma={}",suma);

    return 0;
}