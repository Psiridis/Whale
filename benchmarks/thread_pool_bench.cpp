#include <benchmark/benchmark.h>
#include <vector>
#include <future>
#include "utilities/thread_pool.hpp"


void do_work(unsigned int iterations = 1000000) {
    size_t counter = 0;
    for (size_t j = 0; j < iterations; ++j) {
        counter++;
    }
}

// Sequential benchmark
static void BM_Sequential(benchmark::State& state) {
    unsigned int tasks = state.range(0);
    for (auto _ : state) {
        for (unsigned int i = 0; i < tasks; i++) {
            do_work();
        }
    }
}


// Thread pool benchmar
static void BM_ThreadPool(benchmark::State& state) {
    unsigned int threads = state.range(0);
    unsigned int tasks   = state.range(1);

    for (auto _ : state) {
        util::thread_pool pool(threads);
        std::vector<std::future<void>> futures;
        futures.reserve(tasks);

        for (unsigned int i = 0; i < tasks; i++) {
            futures.push_back(pool.ExecuteTask([] { do_work(); }));
        }
        for (auto& f : futures) {
            f.get();
        }
    }
}

const     unsigned int total_threads   = std::max(1u, std::thread::hardware_concurrency());
constexpr unsigned int min_num_threads = 1;
constexpr unsigned int min_task_num    = 10*min_num_threads;
const     unsigned int max_task_num    = 2*total_threads;

BENCHMARK(BM_ThreadPool)
    ->ArgsProduct({
        benchmark::CreateDenseRange(min_num_threads, total_threads, 1), {min_task_num}
    });

BENCHMARK(BM_ThreadPool)
    ->ArgsProduct({
        {total_threads}, benchmark::CreateDenseRange(min_task_num+1, max_task_num, 1)
    });

BENCHMARK(BM_Sequential)
    ->DenseRange(min_task_num, max_task_num, 1);


BENCHMARK_MAIN();