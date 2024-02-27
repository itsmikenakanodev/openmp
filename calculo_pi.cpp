#include <iostream>

#include <omp.h>
#include <fmt/core.h>
#include <random>
#include <chrono>
//libreria para programacion funcional
#include <functional>

#define NUMERO_ITERACIONES 100000000

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
#pragma omp parallel default(none) shared(num_threads)
    {
#pragma omp master
        num_threads = omp_get_num_threads();
    }

    fmt::println("Utilizando {} threads", num_threads);

    //-- inicializamos el vector con las sumas parciales: num_threads items
    std::vector<long> sumas_parciales(num_threads);
    long block_size = NUMERO_ITERACIONES / num_threads;

#pragma omp parallel default(none) shared (sumas_parciales, block_size)
    {
        int thread_id = omp_get_thread_num();

        std::random_device rd;
        std::default_random_engine gen(rd());
        std::uniform_real_distribution<> distrib(-1, 1);

        long inside = 0;

        /*
         * thread_0:  0.0..12.5
         * thread_1:  12.5..25.0
         * thread_2:  25.0..37.5
         */

        long start = thread_id * block_size;
        long end = (thread_id + 1) * block_size;

//        fmt::println("Thread_{}, start={}, end={}", thread_id, start, end);

        for (long i = start; i < end; i++) {
            double x = distrib(gen);
            double y = distrib(gen);
            if (x * x + y * y <= 1) {
                inside++;
            }
        }

        sumas_parciales[thread_id] = inside;

    }

    long suma_inside = 0;
    for (auto it: sumas_parciales) {
        suma_inside += it;
    }

    return 4 * (double) suma_inside / NUMERO_ITERACIONES;
}

double pi_omp2() {
    int num_threads;
    long block_size;
    long inside_total;

#pragma omp parallel default(none) shared (block_size, num_threads, inside_total)
    {
#pragma omp master
        {
            num_threads = omp_get_num_threads();
            block_size = NUMERO_ITERACIONES / num_threads;
            fmt::println("Utilizando {} threads", num_threads);
        }

#pragma omp barrier
        int thread_id = omp_get_thread_num();

        std::random_device rd;
        std::default_random_engine gen(rd());
        std::uniform_real_distribution<> distrib(-1, 1);

        long inside = 0;

        /*
         * thread_0:  0.0..12.5
         * thread_1:  12.5..25.0
         * thread_2:  25.0..37.5
         */

        long start = thread_id * block_size;
        long end = (thread_id + 1) * block_size;

        //fmt::println("Thread_{}, start={}, end={}", thread_id, start, end);

        for (long i = start; i < end; i++) {
            double x = distrib(gen);
            double y = distrib(gen);
            if (x * x + y * y <= 1) {
                inside++;
            }
        }

#pragma omp critical
        inside_total = inside_total + inside;

    }

    return 4 * (double) inside_total / NUMERO_ITERACIONES;
}

double pi_omp3() {

    long inside_total;

#pragma omp parallel default(none) shared (inside_total)
    {

        std::random_device rd;
        std::default_random_engine gen(rd());
        std::uniform_real_distribution<> distrib(-1, 1);

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
        inside_total = inside_total + inside;

    }

    return 4 * (double) inside_total / NUMERO_ITERACIONES;
}

double pi_omp4() {
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<> distrib(-1, 1);

    long inside = 0;
#pragma omp parallel for reduction(+:inside) default(none) shared(distrib, gen)
    for (long i = 0; i < NUMERO_ITERACIONES; i++) {
        double x = distrib(gen);
        double y = distrib(gen);
        if (x * x + y * y <= 1) {
            inside++;
        }
    }


    return 4 * (double) inside / NUMERO_ITERACIONES;
}


int main() {
//    fmt::println("Calculando PI serial");
//    double pi1;
//    double tiempo1 = calcular_tiempo([&](){
//        pi1=pi_serial();
//    });
//
//    fmt::println("PI serial={}",pi1);
//    fmt::println("Tiempo de proceso: {}ms", tiempo1);
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
        double pi3;
        double tiempo3 = calcular_tiempo([&]() {
            pi3 = pi_omp2();

        });

        fmt::println("PI omp={}, tiempo={}ms", pi3, tiempo3);
    }
    {
        fmt::println("Calculando PI omp3");
        double pi4;
        double tiempo4 = calcular_tiempo([&]() {
            pi4 = pi_omp3();

        });

        fmt::println("PI omp={}, tiempo={}ms", pi4, tiempo4);
    }
    {
        fmt::println("Calculando PI omp4");
        double pi5;
        double tiempo5 = calcular_tiempo([&]() {
            pi5 = pi_omp4();

        });

        fmt::println("PI omp={}, tiempo={}ms", pi5, tiempo5);
    }
    return 0;
}
