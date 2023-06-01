#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <vector>
#include <chrono>

using namespace std;


class Filosofo {
private:
    int id;
    int estomago;
    int estomagoAnterior;
    chrono::steady_clock::time_point inicioPensar;
    chrono::steady_clock::time_point inicioComer;

public:
    Filosofo(int id) : id(id), estomago(0), estomagoAnterior(0) {}

    void pensar() {
        cout << "Filosofo" << id << " está pensando..." << endl;
        inicioPensar = chrono::steady_clock::now();
        this_thread::sleep_for(chrono::milliseconds(getTiempoAleatorio(1000, 3000)));
        auto finPensar = chrono::steady_clock::now();
        chrono::duration<double> duracionPensar = finPensar - inicioPensar;
        cout << "Filosofo" << id << " ha pensado durante " << duracionPensar.count() << " segundos." << endl;
    }

