#include <iostream>
#include <thread>
#include <vector>
#include <semaphore> // Requiere C++20
#include <chrono>

using namespace std;

const int N = 3;
const int i = 0;

counting_semaphore<1> sems[N] = {counting_semaphore<1>(0), counting_semaphore<1>(0),
                                 counting_semaphore<1>(0)};

void proceso(int id)
{
    while (1)
    {
        sems[id].acquire(); // Wait

        cout << "Proceso " << id << " ejecutando" << endl;

        int siguiente = (id + 1) % N;
        sems[siguiente].release();
    }
}

int main()
{
    vector<thread> threads;

    for (int id = 0; id < N; ++id)
    {
        threads.emplace_back(proceso, id);
    }

    sems[i].release(); // Despertar al primer hilo (Pi)

    // Esperar a que terminen
    for (auto &t : threads)
    {
        t.join();
    }

    return 0;
}
