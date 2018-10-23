//
// Created by sebb on 21.05.18.
//

#ifndef MEDIENINFO_PARALLELFOR_H
#define MEDIENINFO_PARALLELFOR_H

#include <thread>
#include <array>
#include <functional>
#include <mutex>
#include <list>
#include <queue>
#include <condition_variable>
#include <future>

template<int threads = 4>
class ParallelFor {
public:
    class Task {
    public:
        std::promise<void>& awake;
        const int min, max;

        Task(std::promise<void> &awake, const int min, const int max)
                : awake(awake), min(min), max(max) {}

        virtual void Run() = 0;
    };

private:
    class LoopTask : public Task {
    public:
        LoopTask(std::promise<void> &awake, const int min, const int max, const std::function<void(int)> &fn) :
                Task(awake, min, max), fn(fn) {}

        void Run() {
            for(int i = Task::min; i < Task::max; i++) {
                fn(i);
            }
        }

    private:
        const std::function<void(int)>& fn;
    };

    class ParallelTask : public Task {
    public:
        ParallelTask(std::promise<void> &awake, const int min, const int max, const std::function<void(int, int)> &fn) :
                Task(awake, min, max), fn(fn) {}

        void Run() {
            fn(Task::min, Task::max);
        }

    private:
        const std::function<void(int, int)>& fn;
    };

public:
    ParallelFor() {
        for (int i = 0; i < threads; ++i) {
            workers[i] = std::thread(&ParallelFor::ThreadEnter, this, i);
        }
    }

    ~ParallelFor() {
        exit = true;
        notifier.notify_all();

        for (int i = 0; i < threads; ++i) {
            workers[i].join();
        }
    }

    void RunL(const std::function<void(int)>& fn, const int min, const int max) {
        const int range = max - min;
        const int singleRange = range/threads;

        if(singleRange < singleThreadLimit) {
            // for a small number of task the overhead isn't worth it
            for (int i = min; i < max; ++i) {
                fn(i);
            }
        }
        else {
            std::array<std::promise<void>, threads> promises;

            std::unique_lock<std::mutex> lck(lock);
            for (int i = 0; i < threads; ++i) {
                tasks[i].push(new LoopTask(promises[i],  min + (i * singleRange),
                                   (i == threads - 1) ? max : min + (i + 1) * singleRange, fn
                ));
            }
            lck.unlock();
            notifier.notify_all();

            for (int j = 0; j < threads; ++j) {
                promises[j].get_future().wait();
            }
        }
    }

    void RunP(const std::function<void(int, int)>& fn, const int min, const int max) {
        const int range = max - min;
        const int singleRange = range/threads;

        if(singleRange < singleThreadLimit) {
            // for a small number of task the overhead isn't worth it
            fn(min, max);
        }
        else {
            std::array<std::promise<void>, threads> promises;

            std::unique_lock<std::mutex> lck(lock);
            for (int i = 0; i < threads; ++i) {
                tasks[i].push(new ParallelTask(promises[i], min + (i * singleRange),
                                   (i == threads - 1) ? max : min + (i + 1) * singleRange, fn
                ));
            }
            lck.unlock();
            notifier.notify_all();

            for (int j = 0; j < threads; ++j) {
                promises[j].get_future().wait();
            }
        }
    }

private:
    std::array<std::thread, threads> workers;
    std::array<std::queue<ParallelFor::Task*>, threads> tasks;
    std::condition_variable notifier;
    std::mutex lock;
    const int singleThreadLimit = 200;
    bool exit = false;

    void ThreadEnter(int n) {
        std::unique_lock<std::mutex> accessRight(lock);

        while (!exit) {

            if(!tasks[n].empty()) {
                Task* tsk = tasks[n].front();
                tasks[n].pop();
                accessRight.unlock();

                tsk->Run();

                tsk->awake.set_value();
                delete tsk;

                accessRight.lock();
            }
            else {
                notifier.wait(accessRight);
            }
        }
    }
};


#endif //MEDIENINFO_PARALLELFOR_H
