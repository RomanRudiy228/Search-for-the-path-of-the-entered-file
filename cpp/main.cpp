#include "../header/search.h"
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadsP
{
public:
    ThreadsP(size_t numThreads) : stop(false)
    {
        for (size_t i = 0; i < numThreads; ++i)
        {
            workers.emplace_back([this]
            {
                while (true)
                {
                    std::function<void()> task;
                    {
                        unique_lock<mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty())
                        {
                            return;
                        }
                        task = move(tasks.front());
                        tasks.pop_front();
                    }
                    task();
                }
            });
        }
    }

    template <class F, class... Args>
    void queue(F&& f, Args&&... args)
    {
        {
            unique_lock<mutex> lock(queueMutex);
            tasks.emplace_back([f = std::forward<F>(f), args = std::make_tuple(std::forward<Args>(args)...)] 
            {
                std::apply(f, args);
            });
        }
        condition.notify_one();
    }

    void stopExecution()
    {
        stop = true;
        condition.notify_all();
    }

    ~ThreadsP()
    {
        {
            unique_lock<mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (thread& worker : workers)
        {
            worker.join();
        }
    }

private:
    vector<thread> workers;
    deque<std::function<void()>> tasks;

    mutex queueMutex;
    condition_variable condition;
    bool stop;
};

bool recurs(ThreadsP& pool, string filePath, string fileName)
{
    vector<string> filePaths = FindInDirectory(filePath, fileName);
    if (filePaths.size() != 0)
    {
        if (filePaths[1] == fileName)
        {
            cout << filePaths[0] + filePaths[2] << endl;
            pool.stopExecution();
            return true;
        } 
        else
        {
            for (int i = 1; i < filePaths.size(); i++)
            {
                if (recurs(pool, filePaths[0] + filePaths[i], fileName))
                    return true;
            }
        }
    }
    return false;
}

int main() {

    string fileName;
    cout << "Enter a Filename: ";
    cin >> fileName;

    cout << "Searching file with name: " << fileName << endl;

    constexpr int MAX_THREADS = 8;
    int numThreads = thread::hardware_concurrency();
    ThreadsP pool(numThreads > MAX_THREADS ? MAX_THREADS : numThreads);

    if(!recurs(pool, "C:\\", fileName))
    {
        cout << "File not found" << endl;
    }

    cin.ignore();

    return 0;
}