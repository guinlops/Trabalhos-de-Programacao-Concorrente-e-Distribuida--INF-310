
/*
    Comentários:
        1- Ordem da impressão pode não estar na *ordem* correta, mas é suficiente para se ter noção do andamento da execução
        2- Impressões com cout_mtx podem interferir no desempenho.
        2- N definida para ser tanto o número de "pessoas" ao entrar no barco quanto o número de vezes que 
        3- Tempo de entrada no barco definido como 500ms ou 0.5 segundos
        4- 

*/
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <map>
#include <queue>
#include <barrier>
using namespace std;

const int pesoMAX = 500;
int somaPesos = 0;
int pesoAtual = 0;
int time_ms=500;
bool quaseTodas=false;
const int N = 10;
std::mutex mtx;      // Mutex para sincronização
std::mutex cout_mtx; // Mutex exclusivo para proteger printf

struct Pessoa {
    int _id;
    int _peso;
    int _entradasAoBarco=0;

    Pessoa(int id, int peso) : _id(id), _peso(peso) {}
};

mutex mux;
mutex muxRequisita;
mutex muxLibera;
mutex muxwup;
mutex muxblock;
map<thread::id, int> A;
map<int,int> entradas;
queue<pair<thread::id, Pessoa>> bloqueadas;

void wakeup(thread::id t) {
    std::lock_guard lock(muxwup); // Substituído por lock_guard
    A[t]++;
}

void p_pesoTotal() {
    std::lock_guard lock(cout_mtx); // Substituído por lock_guard
    printf("Peso total: %d\n", pesoAtual);
}

void apos_liberacao() {
    printf("Todas chegaram\n");
    p_pesoTotal();
}

barrier bar(N, apos_liberacao); 

void block() {
    thread::id meu_id = this_thread::get_id();
    while (true) {
        {
            std::lock_guard lock(muxblock); // Substituído por lock_guard
            if (A[meu_id] > 0) {
                A[meu_id]--;
                break;
            }
        }
    }
}

void requisita(Pessoa &pessoa, bool &entrou) {
    printf("Pessoa %d de peso %d tenta entrar no barco\n", pessoa._id, pessoa._peso);
    {
        std::lock_guard lock(muxRequisita); // Substituído por lock_guard
        if (pesoAtual + pessoa._peso <= pesoMAX) {
            pesoAtual += pessoa._peso;
            entradas[pessoa._id]++;
            entrou = true;
            printf("Pessoa %d entra no barco\n", pessoa._id);
            return;
        } else {
            printf("%d não conseguiu entrar no barco %d+%d extrapola %d\n", pessoa._id, pessoa._peso, pesoAtual, pesoMAX);
            bloqueadas.push(std::make_pair(this_thread::get_id(), pessoa));
        }
    }
    block();
}

void libera(Pessoa &pessoa) {
    {
        std::lock_guard lock(muxLibera); // Substituído por lock_guard
        pesoAtual -= pessoa._peso;
        printf("%d saiu do barco reduzindo %d do peso total\n", pessoa._id, pessoa._peso);

        if (!bloqueadas.empty()) {
            wakeup(bloqueadas.front().first);
            printf("%d retirada da fila de bloqueadas\n", bloqueadas.front().second._id);
            bloqueadas.pop();
        }
    }
}

void processo(Pessoa &pessoa) {
    while (true) {
        if ((entradas[pessoa._id] + 1) == N && !quaseTodas) {
            printf("Pessoa %d entrou %d vezes, esperando o resto entrar n-1 vezes\n", pessoa._id, entradas[pessoa._id]);
            bar.arrive_and_wait();
            quaseTodas = true;
        }
        if (entradas[pessoa._id] == N) {
            printf("Pessoa %d entrou %d vezes, esperando o resto entrar n vezes\n", pessoa._id, entradas[pessoa._id]);
            //bar.arrive_and_wait();
            break;
        }
        bool entrou = false;
        requisita(pessoa, entrou);

        if (!entrou) {
            continue;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
        printf("\nPessoa %d de peso %d dentro do barco ~bebeu por %d milisegundos\n\n", pessoa._id, pessoa._peso, time_ms);
        libera(pessoa);
        p_pesoTotal();
    }
}

int main() {
    std::vector<std::thread> threads;
    std::srand(std::time(0));

    for (int i = 0; i < N; ++i) {
        int peso = std::rand() % 111 + 40;
        Pessoa *pessoa = new Pessoa(i, peso);
        threads.push_back(std::thread(processo, std::ref(*pessoa)));
    }

    for (auto &th : threads) {
        th.join();
    }

    for (auto &entrada : entradas) {
        cout << entrada.first << " entrou " << entrada.second << " vezes\n";
    }
    return 0;
}