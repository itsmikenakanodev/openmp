#include <iostream>

#include <omp.h>
#include <fmt/core.h>
#include <vector>
#include <functional>

#define MAX_ELEMENTOS 1024

typedef std::function<float(float, float)> binary_op;

float reduccion_serial(const float *input, const int n, binary_op op) {
    float suma = 0.0f;
    for (int i = 0; i < n; i++) {
        suma = op(suma, input[i]);
    }
    return suma;
}

float reduccion_paralela(const float *input, const int n, binary_op op) {
    float suma_total = 0.0f;
    int num_threads;
    long block_size;

#pragma omp parallel default(none) shared(num_threads, op, input, block_size, suma_total)
    {

#pragma omp master
        {
            num_threads = omp_get_num_threads();
            block_size = MAX_ELEMENTOS / num_threads;
            fmt::println("Numero de hilos: {}, block_size= {}", num_threads, block_size);
        }

#pragma omp barrier
        int thread_id = omp_get_thread_num();

        long start = thread_id * block_size;
        long end = (thread_id + 1) * block_size;

        if (thread_id == num_threads - 1) {
            end = MAX_ELEMENTOS;
        }

        fmt::println("thread_{:2}, start= {}, end= {}", thread_id, start, end);

        float suma = 0.0f;

        for (int i = start; i < end; i++) {
            suma = op(suma, input[i]);
        }

#pragma omp critical
        suma_total += suma;

#pragma omp barrier
        fmt::println("thread_{:2}, suma={}", thread_id, suma);
    }
    return suma_total;
}

float reduccion_paralela2(const float *input, const int n, binary_op op) {
    float suma_total = 0.0f;
    int num_threads;

#pragma omp parallel default(none) shared(num_threads, op, input, suma_total)
    {

#pragma omp master
        {
            num_threads = omp_get_num_threads();
            fmt::println("Numero de hilos: {}", num_threads);
        }

#pragma omp barrier
        int thread_id = omp_get_thread_num();

        float suma = 0.0f;

        for (int i = thread_id; i < MAX_ELEMENTOS; i += num_threads) {
            suma = op(suma, input[i]);
        }

#pragma omp critical
        suma_total += suma;

#pragma omp barrier
        fmt::println("thread_{:2}, suma={}", thread_id, suma);
    }
    return suma_total;
}

float reduccion_paralela3(const float *input, const int n, binary_op op) {
    float suma_total = 0.0f;
    int num_threads;

#pragma omp parallel default(none) shared(num_threads, op, input, suma_total)
    {

#pragma omp master
        {
            num_threads = omp_get_num_threads();
            fmt::println("Numero de hilos: {}", num_threads);
        }

#pragma omp barrier
        int thread_id = omp_get_thread_num();

        float suma = 0.0f;

        for (int i = thread_id; i < MAX_ELEMENTOS; i += num_threads) {
            suma = op(suma, input[i]);
        }

#pragma omp critical
        suma_total += suma;

#pragma omp barrier
        fmt::println("thread_{:2}, suma={}", thread_id, suma);
    }
    return suma_total;
}

float reduccion_paralela4(float *input, const int n, binary_op op) {
    int num_threads;

#pragma omp parallel default(none) shared(num_threads)
    {
#pragma omp master
        {
            num_threads = omp_get_num_threads();
            fmt::println("Numero de hilos: {}", num_threads);
        }
    }
    for( int e=0; e<n;e+=num_threads){
    for (int s = num_threads / 2; s > 0; s /= 2) {
        //fmt::println("iteracion={} ", s);
        #pragma omp parallel num_threads(s) default(none) shared(num_threads, op, input, n, s,e)
        {
            int thread_id = omp_get_thread_num();
            //fmt::println("hilo ocupado={} ", thread_id);
            input[thread_id+e] = op(input[thread_id+e], input[(thread_id+e) + s]);
        }
    }
    fmt::println("valor: {}",input[e]);
    }

    for(int j=num_threads;j<n;j+=num_threads){
        input[0]=input[0]+input[j];
    }

    return input[0];
}

int main() {

    /*std::vector<float> datos(MAX_ELEMENTOS);
    for(int i=0;i<MAX_ELEMENTOS;i++){
        datos[i]=i+1;
    }

    auto op_add=[](auto v1, auto v2){
        return v1+v2;
    };

    auto suma1 = reduccion_serial(datos.data(),datos.size(), op_add);

    fmt::println("Reduccion serial: {}", suma1);

    auto suma2= reduccion_paralela(datos.data(),datos.size(), op_add);
    fmt::println("Reduccion paralela: {}", suma2);

    auto suma3= reduccion_paralela2(datos.data(),datos.size(), op_add);
    fmt::println("Reduccion paralela2: {}", suma3);*/

    auto op_add1 = [](auto v1, auto v2) {
        return v1 + v2;
    };

    std::vector<float> datos1(1024);
    for (int i = 0; i < 1024; i++) {
        datos1[i] = i+1;
    }
    auto suma4 = reduccion_paralela4(datos1.data(), datos1.size(), op_add1);
    fmt::println("Reduccion paralela4: {}", suma4);

    return 0;
}

