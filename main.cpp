#include <iostream>
#include <thread>
#include <chrono>

int main() {
    const size_t N = 1000000;
    const int THREADS = 4;

    //выделяется память под масссив
    int* data = new int[N];

    //генерация случайных чисел каждый запуск
    unsigned x = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
    for (size_t i = 0; i < N; ++i) {
        x = x * 1103515245u + 12345u;
        data[i] = static_cast<int>(x % 100u) + 1; // числа от 1 до 100
    }

    //начало замера времени для одного потока
    auto start1 = std::chrono::high_resolution_clock::now();

    long long sum1 = 0;
    //суммирование для одного потока
    for (size_t i = 0; i < N; ++i)
        sum1 += data[i];

    //конец замера времени для одного потока
    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> t1 = end1 - start1;

    //вывод
    std::cout << "Sum (1 potok): " << sum1 << std::endl;
    std::cout << "Time (1 potok): " << t1.count() << " s" << std::endl << std::endl;

    //начало замера времени для 4-х потоков
    auto start2 = std::chrono::high_resolution_clock::now();

    std::thread threads[THREADS];
    long long partial[THREADS] = {0};
    size_t chunk = N / THREADS;

    //каждый поток обрабатывает свой участок массива
    for (int i = 0; i < THREADS; ++i) {
        size_t begin = static_cast<size_t>(i) * chunk;
        size_t end = (i == THREADS - 1) ? N : begin + chunk;

        threads[i] = std::thread([&data, &partial](int idx, size_t b, size_t e) {
            long long local_sum = 0;
            for (size_t j = b; j < e; ++j)
                local_sum += data[j];
            partial[idx] = local_sum;
        }, i, begin, end);
    }

    //ожидание завершения всех потоков
    for (int i = 0; i < THREADS; ++i)
        threads[i].join();

    //суммирование для 4-х потоков
    long long sum2 = 0;
    for (int i = 0; i < THREADS; ++i)
        sum2 += partial[i];

    //конец замера времени для 4-ч потоков
    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> t2 = end2 - start2;

    //вывод
    std::cout << "Sum (4 potoka): " << sum2 << std::endl;
    std::cout << "Time (4 potoka): " << t2.count() << " s" << std::endl << std::endl;
    std::cout << "Speedup: ~" << t1.count() / t2.count() << "x faster" << std::endl;

    //очищаем память
    delete[] data;

    return 0;
}
