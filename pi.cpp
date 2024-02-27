#include <iostream>

#include <omp.h>
#include <fmt/core.h>
#include <random>
#include <chrono>
#include <vector>
//libreria para programacion funcional
#include <functional>

#define NUMERO_ITERACIONES 1000000

namespace ch = std::chrono;

double calcular_tiempo(std::function<void(void)> fn) {
    auto start = ch::high_resolution_clock::now();
    //calculo
    fn();
    auto end = ch::high_resolution_clock::now();

    ch::duration<double, std::milli> tiempo = end - start;
    return tiempo.count();
}

double pi_serial() {
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<> distrib(-1, 1);

    long inside = 0;

    for (long n = 0; n < NUMERO_ITERACIONES; n++) {
        double x = distrib(gen);
        double y = distrib(gen);
        if (x * x + y * y <= 1) {
            inside++;
        }
    }

    return 4 * (double) inside / NUMERO_ITERACIONES;
}

double pi_omp1() {
    int num_threads;
    long inside_total=0;


#pragma omp parallel default(none) shared(num_threads)
    {
#pragma omp master
        {
            num_threads = omp_get_num_threads();
        }

    }

    long block_size = NUMERO_ITERACIONES / num_threads;
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<> distrib(-1, 1);

#pragma omp parallel default(none) shared(block_size,inside_total,gen,distrib)
    {
        long inside_parcial=0;
        auto start = block_size * omp_get_thread_num();
        auto end= block_size * (omp_get_thread_num()+1);
        for(long i=start; i<end; i++){
            double x = distrib(gen);
            double y = distrib(gen);
            if (x * x + y * y <= 1) {
                inside_parcial++;
            }
        }

#pragma omp critical
        inside_total +=inside_parcial;

}
    return 4 * (double) inside_total / NUMERO_ITERACIONES;
}

double pi_omp2() {
    int num_threads;
    long inside_total=0;

#pragma omp parallel default(none) shared(num_threads)
    {
#pragma omp master
        {
            num_threads = omp_get_num_threads();
        }
    }

    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<> distrib(-1, 1);

#pragma omp parallel default(none) shared(inside_total,gen,distrib)
    {
        long inside_parcial=0;
        #pragma omp for
        for(long i=0; i<NUMERO_ITERACIONES; i++){
            double x = distrib(gen);
            double y = distrib(gen);
            if (x * x + y * y <= 1) {
                inside_parcial++;
            }
        }

#pragma omp critical
        inside_total +=inside_parcial;
    }

    return 4 * (double) inside_total / NUMERO_ITERACIONES;
}

double pi_omp3() {
    long inside_total=0;

    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<> distrib(-1, 1);

#pragma omp parallel for reduction(+:inside_total) default(none) shared(gen,distrib)
    for(long i=0; i<NUMERO_ITERACIONES; i++){
            double x = distrib(gen);
            double y = distrib(gen);
            if (x * x + y * y <= 1) {
                inside_total++;
            }
    }


    return 4 * (double) inside_total / NUMERO_ITERACIONES;
}

double pi_paralelo2() {

    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<> distrib(-1, 1);
    long inside_total = 0;


#pragma omp parallel default (none) shared(distrib, gen,inside_total)
    {
        long inside = 0;
#pragma omp for
        for (long i = 0; i < NUMERO_ITERACIONES; i++) {
            double x = distrib(gen);
            double y = distrib(gen);
            if (x * x + y * y <= 1) {
                inside++;

            }
        }

#pragma omp critical
        inside_total += inside;
    }

    return 4*(double) inside_total/NUMERO_ITERACIONES;

}

long pi_omp5(long *input, int tamanio) {

    long inside=input[0];
#pragma omp parallel for reduction(min:inside) default(none) shared(tamanio,input)
    for (long i = 1; i < tamanio ; i++) {
            inside = input[i];
    }
    return inside;
}

long pi_omp6(long *input, int tamanio) {

    long inside=input[0];
#pragma omp parallel for reduction(max:inside) default(none) shared(tamanio,input)
    for (long i = 1; i < tamanio ; i++) {
            inside = input[i];
    }
    return inside;
}


int main(){
    {
        fmt::println("Calculando PI omp1");
        double pi2;
        double tiempo2 = calcular_tiempo([&]() {
            pi2 = pi_omp1();

        });

        fmt::println("PI omp={}, tiempo={}ms", pi2, tiempo2);
    }
    {
        fmt::println("Calculando PI omp2");
        double pi2;
        double tiempo2 = calcular_tiempo([&]() {
            pi2 = pi_omp2();

        });

        fmt::println("PI omp={}, tiempo={}ms", pi2, tiempo2);
    }
    {
        fmt::println("Calculando PI omp3");
        double pi2;
        double tiempo2 = calcular_tiempo([&]() {
            pi2 = pi_omp3();

        });

        fmt::println("PI omp={}, tiempo={}ms", pi2, tiempo2);
    }

    std::vector<long> datos1(10);
    for (long i = 0; i < 10; i++) {
        datos1[i] = i+1;
    }
    auto min=pi_omp5(datos1.data(),datos1.size());
    fmt::println("El valor minimo del vector es: {}",min);

    return 0;
}