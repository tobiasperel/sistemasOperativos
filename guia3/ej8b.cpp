#include <iostream>
#include <thread>
#include <vector>
#include <semaphore> // Requiere C++20
#include <chrono>

using namespace std;

const int N = 3;
const int i = 1;
bool segundaVez = true; 

counting_semaphore<1> sems[N] = {counting_semaphore<1>(0), counting_semaphore<1>(0),
                                 counting_semaphore<1>(0)};

void proceso(int id)
{

    sems[id].acquire(); // Wait

    cout << "Proceso " << id << " ejecutando" << endl;

    if (id == 1 && segundaVez)
    {
        segundaVez = false;
        sems[1].release();
    }
    else
    {
        segundaVez = true;
        int siguiente = (id + 1) % N;
        sems[siguiente].release();
    }

}

int main()
{
    sems[i].release(); // Primer hilo
    vector<thread> threads;

    while(true) {    
        for (int id = 0; id < N; ++id) {
            threads.emplace_back(proceso, id);
        }
        
    }
    for (auto& t : threads) {
        t.join();
    }
    return 0;
}
