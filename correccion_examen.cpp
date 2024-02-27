#include <iostream>
#include <omp.h>
#include <vector>
#include <chrono>
#include <functional>
#include <fstream>

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fort.hpp>

namespace ch = std::chrono;

static std::vector<int> datos;

void calcular_tiempo(const std::string& msg, std::function<void(void)> fn) {
    fmt::println("-------------------------------");
    fmt::println("{}", msg);

    auto start = ch::high_resolution_clock::now();

    // cálculo
    fn();

    auto end = ch::high_resolution_clock::now();
    ch::duration<double, std::milli> tiempo = end - start;

    fmt::println("{}, tiempo={}ms", msg, tiempo.count() );
}

std::string crear_tabla(const std::vector<int>& freqs) {
    fort::utf8_table table;
    table << fort::header;
    table << "Valor";
    table << "Conteo";
    table << fort::endr;

    for (int index = 0; index < freqs.size(); index++) {
        table << index;
        table << freqs[index];
        table << fort::endr;
    }

    return table.to_string();
}

std::vector<int> read_file() {
    std::fstream fs("c:/tmp/datos.txt", std::ios::in);
    std::string line;
    std::vector<int> ret;
    while (std::getline(fs, line)) {
        ret.push_back(std::stoi(line));
    }
    fs.close();
    return ret;
}

//-----------------------------------------------------------------

std::vector<int> serial_frecuencias() {
    std::vector<int> cc(101);

    for (int dato : datos) {
        cc[dato]++;
    }

    return cc;
}

double serial_promedio() {
    double suma = 0;
    for (int dato : datos) {
        suma = suma + dato;
    }

    return suma / datos.size();
}

std::tuple<int, int> serial_min_max() {
    int min = datos[0];
    int max = datos[0];

    for (int dato : datos) {
        if (dato < min)
            min = dato;
        if (dato > max)
            max = dato;
    }

    return {min, max};
}

//-----------------------------------------------------------------

std::vector<int> paralelo_frecuencias() {
    std::vector<int> cc(101);

    #pragma omp parallel default(none) shared(datos, cc)
    {
        int thread_id = omp_get_thread_num();
        int thread_num = omp_get_num_threads();

        int cc_local[101];
        std::memset(cc_local, 0, 101 * sizeof(int));

        for (int i = thread_id; i < datos.size(); i += thread_num) {
            cc_local[datos[i]]++;
        }

        #pragma omp critical
        {
            for (int i = 0; i < cc.size(); i++) {
                cc[i] += cc_local[i];
            }
        }
    }

    return cc;
}

double paralelo_promedio() {
    int num_threads;
    long block_size;
    int suma_total = 0;

    #pragma omp parallel default(none) shared(datos,num_threads,block_size,suma_total)
    {
        int thread_id = omp_get_thread_num();
        int thread_num = omp_get_num_threads();

        int suma_parcial = 0;

        for (int i = thread_id; i < datos.size(); i += thread_num) {
            suma_parcial = suma_parcial + datos[i];
        }

        #pragma omp critical
        {
            suma_total = suma_total + suma_parcial;
        }
    }

    return suma_total / (double )datos.size();
}

std::tuple<int, int> paralelo_min_max() {
    int num_threads;
    long block_size;

    int min_total = datos[0];
    int max_total = datos[0];

    #pragma omp parallel default(none) shared(datos,num_threads,block_size,min_total,max_total)
    {
        #pragma omp master
        {
            num_threads = omp_get_num_threads();
            block_size = datos.size() / num_threads;
            //fmt::println("Numero hilos={}, block_size={}", num_threads, block_size);
        }

        #pragma omp barrier

        int thread_id = omp_get_thread_num();

        long start = thread_id*block_size;
        long end = (thread_id+1)*block_size;

        if(thread_id==num_threads-1) {
            end = datos.size();
        }

        //fmt::println("thread_{}, start={}, end={}", thread_id, start, end);

        int min = datos[start];
        int max = datos[start];
        for(int i=start;i<end;i++) {
            if(datos[i]<min)
                min = datos[i];
            if(datos[i]>max)
                max = datos[i];
        }

        #pragma omp critical
        {
            if(min<min_total) min_total = min;
            if(max>max_total) max_total  =max;
        }
    }

    return {min_total, max_total};
}

//-----------------------------------------------------------------

std::vector<int> paralelo2_frecuencias() {
    std::vector<int> cc(101);

    #pragma omp parallel default(none) shared(datos, cc)
    {
        std::vector<int> tmp_freqs(101);

        #pragma omp for
        for (int i = 0; i < datos.size(); i++) {
            tmp_freqs[datos[i]]++;
        }

        #pragma omp critical
        {
            for (int i = 0; i < 101; i++) {
                cc[i] = cc[i] + tmp_freqs[i];
            }
        }
    }

    return cc;
}

double paralelo2_promedio() {
    double suma_total = 0;

    #pragma omp parallel default(none) shared(datos, suma_total)
    {
        double suma_parcial = 0;

        #pragma omp for
        for (int i = 0; i < datos.size(); i++) {
            suma_parcial = suma_parcial + datos[i];
        }

        #pragma omp critical
        {
            suma_total = suma_total + suma_parcial;
        }
    }

    return suma_total/datos.size();
}

std::tuple<int, int> paralelo2_min_max() {
    int min_total = datos[0];
    int max_total = datos[0];

    #pragma omp parallel default(none) shared(datos, min_total, max_total)
    {
        int min = datos[0];
        int max = datos[0];

        #pragma omp for
        for (int i = 0; i < datos.size(); i++) {
            if(datos[i]<min)
                min = datos[i];
            if(datos[i]>max)
                max = datos[i];
        }

        #pragma omp critical
        {
            if(min<min_total)
                min_total = min;
            if(max>max_total)
                max_total = max;
        }
    }

    return {min_total, max_total};
}

int main() {
    datos = read_file();

    //-----------------------------------------------------------------
    //-- versiones seriales
    { // frecuencias
        std::vector<int> freqs;
        calcular_tiempo("SERIAL Frecuencias", [&]() {
            freqs = serial_frecuencias();
        });
        std::cout << crear_tabla(freqs) << std::endl;
    }

    { // promedios
        double promedio;
        calcular_tiempo("SERIAL Promedio", [&]() {
            promedio = serial_promedio();
        });
        fmt::println("SERIAL Promedio {}", promedio);
    }

    { // min-max
        std::tuple<int, int> min_max;
        calcular_tiempo("SERIAL Min/Max", [&]() {
            min_max = serial_min_max();
        });
        fmt::println("SERIAL Min/Max {}", min_max);
    }

    fmt::println("");
    fmt::println("");

    //-----------------------------------------------------------------
    //-- versiones paralelas1
    { // frecuencias
        std::vector<int> freqs;
        calcular_tiempo("PARALELO Frecuencias", [&]() {
            freqs = paralelo_frecuencias();
        });
        std::cout << crear_tabla(freqs) << std::endl;
    }

    { // promedios
        double promedio;
        calcular_tiempo("PARALELO Promedio", [&]() {
            promedio = paralelo_promedio();
        });
        fmt::println("PARALELO Promedio {}", promedio);
    }

    { // min-max
        std::tuple<int, int> min_max;
        calcular_tiempo("PARALELO Min/Max", [&]() {
            min_max = paralelo_min_max();
        });
        fmt::println("PARALELO Min/Max {}", min_max);
    }

    //-----------------------------------------------------------------
    //-- versiones paralelas2
    { // frecuencias
        std::vector<int> freqs;
        calcular_tiempo("PARALELO2 Frecuencias",  [&]() {
            freqs = paralelo2_frecuencias();
        });
        std::cout << crear_tabla(freqs) << std::endl;
    }

    { // promedios
        double promedio;
        calcular_tiempo("PARALELO2 Promedio", [&]() {
            promedio = paralelo2_promedio();
        });
        fmt::println("PARALELO2 Promedio {}", promedio);
    }

    { // min-max
        std::tuple<int, int> min_max;
        calcular_tiempo("PARALELO2 Min/Max", [&]() {
            min_max = paralelo2_min_max();
        });
        fmt::println("PARALELO2 Min/Max {}", min_max);
    }

    return 0;
}