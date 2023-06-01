#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <vector>
#include <chrono>

using namespace std;

// Clase para representar a un filósofo
class Filosofo {
private:
    int id;
    int estomago;
    int estomagoAnterior;// Nuevo miembro para guardar el valor anterior del estómago
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

    bool comer(int& fideos) {
        if (fideos <= 0) {
            return false; // No hay más fideos, salir sin comer
        }

        cout << "Filosofo" << id << " está comiendo..." << endl;
        inicioComer = chrono::steady_clock::now();
        int cantidadComida = min(getTiempoAleatorio(10, 50), fideos); // Comer hasta un máximo de los fideos disponibles
        if (estomago + cantidadComida > 100) {
            cantidadComida = 100 - estomago;  // Ajustar la cantidad de comida para llenar el estómago
        }
        estomagoAnterior = estomago; // Guardar el valor anterior del estómago
        estomago += cantidadComida;
        fideos -= cantidadComida;
        this_thread::sleep_for(chrono::milliseconds(getTiempoAleatorio(1000, 2000)));
        auto finComer = chrono::steady_clock::now();
        chrono::duration<double> duracionComer = finComer - inicioComer;
        cout << "Filosofo" << id << " ha comido durante " << duracionComer.count() << " segundos." << endl;
        cout << "Filosofo" << id << " incremento el valor de estómago (" << estomagoAnterior << " + " << cantidadComida << " = " << estomago << ")" << endl;

        return true;
    }

    int getEstomago() const {
        return estomago;
    }

private:
    int getTiempoAleatorio(int min, int max) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }
};

int main() {
    int numFilosofos;
    int cantidadFideos;

    cout << "Ingrese cantidad de fideos: ";
    cin >> cantidadFideos;
    cout << "Ingrese cantidad de filósofos: ";
    cin >> numFilosofos;

    vector<mutex> tenedores(numFilosofos); // Tenedores como mutex

    vector<Filosofo> filosofos;
    filosofos.reserve(numFilosofos); // Reservar espacio en el vector para los filósofos

     // Crear los filósofos uno por uno y agregarlos al vector
    for (int i = 0; i < numFilosofos; ++i) {
        filosofos.emplace_back(i + 1);
    }

    vector<thread> filosofoThreads;
    // Crear hilos para los filósofos
    for (int i = 0; i < numFilosofos; ++i) {
        filosofoThreads.emplace_back([&](int idx) {
            while (true) {
                filosofos[idx].pensar();
                 // Bloquear tenedores
                tenedores[idx].lock();
                tenedores[(idx + 1) % numFilosofos].lock();

                if (!filosofos[idx].comer(cantidadFideos)) {
                    // No hay más fideos, salir del bucle
                    tenedores[(idx + 1) % numFilosofos].unlock();
                    tenedores[idx].unlock();
                    break;
                }
                 // Desbloquear tenedores
                tenedores[(idx + 1) % numFilosofos].unlock();
                tenedores[idx].unlock();
            }
        }, i);
    }
    // Esperar a que todos los hilos de los filósofos terminen
    for (auto& thread : filosofoThreads) {
        thread.join();
    }
    // Mostrar estadísticas
    cout << "Estadísticas:" << endl;
    for (int i = 0; i < numFilosofos; ++i) {
        cout << "Estomago filosofo" << i + 1 << " = " << filosofos[i].getEstomago() << endl;
    }
    cout << "Fideos restantes = " << cantidadFideos << endl;

    return 0;
}
