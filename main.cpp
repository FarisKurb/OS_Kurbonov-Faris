#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <iomanip>

void sumPart(const std::vector<int>* data, size_t begin, size_t end, long long& result) {
    long long local_sum = 0;
    for (size_t i = begin; i < end; ++i) {
        local_sum += (*data)[i];
    }
    result = local_sum;
}

int main() {
    const size_t N = 1000000;
    const int THREADS = 4;

    std::vector<int> data(N);

    std::srand(static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count()
    ));

    for (size_t i = 0; i < N; ++i) {
        data[i] = (std::rand() % 100) + 1;
    }

    std::thread workers_single[THREADS];
    long long partial_single[THREADS] = {0};

    size_t chunk_single = N;
    size_t begin_single = 0;
    size_t end_single = N;

    auto t1_start = std::chrono::steady_clock::now();

    workers_single[0] = std::thread(sumPart, &data, begin_single, end_single, std::ref(partial_single[0]));

    for (int i = 1; i < THREADS; ++i) {
        workers_single[i] = std::thread([](){});
    }

    for (int i = 0; i < THREADS; ++i) {
        workers_single[i].join();
    }

    auto t1_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> dur_single = t1_end - t1_start;

    long long sum_single = partial_single[0];

    std::thread workers_multi[THREADS];
    long long partial_multi[THREADS] = {0};

    size_t chunk = N / THREADS;

    auto t2_start = std::chrono::steady_clock::now();

    for (int i = 0; i < THREADS; ++i) {
        size_t begin = static_cast<size_t>(i) * chunk;
        size_t end = (i == THREADS - 1) ? N : begin + chunk;

        workers_multi[i] = std::thread(sumPart, &data, begin, end, std::ref(partial_multi[i]));
    }

    for (int i = 0; i < THREADS; ++i) {
        workers_multi[i].join();
    }

    auto t2_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> dur_multi = t2_end - t2_start;

    long long sum_multi = 0;
    for (int i = 0; i < THREADS; ++i) {
        sum_multi += partial_multi[i];
    }
    std::cout << std::fixed << std::setprecision(6);

    std::cout << "Sum (1 thread): " << sum_single << std::endl;
    std::cout << "Time (1 thread): " << dur_single.count() << " s" << std::endl << std::endl;

    std::cout << "Sum (4 threads): " << sum_multi << std::endl;
    std::cout << "Time (4 threads): " << dur_multi.count() << " s" << std::endl << std::endl;

    if (sum_single != sum_multi) {
        std::cout << "Warning: sums differ!" << std::endl;
    }

    if (dur_multi.count() > 0.0) {
        std::cout << "Speedup: ~" << (dur_single.count() / dur_multi.count()) << "x faster" << std::endl;
    } else {
        std::cout << "Speedup: (multi-thread time too small to measure)" << std::endl;
    }

    return 0;
}
