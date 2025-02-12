/*
    Comentários:
        1- Devido ao uso de barrier: g++ -std=c++2b barco.cpp -o barco, sendo b qualquer versão acima do c++ 20
        2- Ordem da impressão pode não estar na *ordem* correta, mas é suficiente para se ter noção do andamento da execução
        3- Impressões de debug interferem no desempenho geral.
        4- N definida para ser tanto o número de "pessoas" ao entrar no barco quanto o número de vezes que 
        5- Tempo de entrada no barco definido como 100ms ou 0.1 segundos
        6- ./arquivo > output.txt para debug
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

const int pesoMAX = 500; // Peso máximo permitido no barco
int somaPesos = 0;       // Soma total de pesos, usada para controle
int pesoAtual = 0;       // Peso atual no barco
int time_ms = 100;       // Tempo de permanência no barco em milissegundos
bool quaseTodas = false; // Flag para indicar que quase todas as pessoas entraram
const int N = 10;        // Número de pessoas e número de vezes que cada uma entra no barco
std::mutex mtx;          // Mutex para sincronização geral
std::mutex cout_mtx;     // Mutex exclusivo para proteger a saída (printf)

struct Pessoa {
    int _id;               // Identificador da pessoa
    int _peso;             // Peso da pessoa
    int _entradasAoBarco = 0; // Contador de quantas vezes entrou no barco

    Pessoa(int id, int peso) : _id(id), _peso(peso) {} // Construtor
};

// Variáveis para controle de sincronização e acesso
mutex mux;
mutex muxRequisita;
mutex muxLibera;
mutex muxwup;
mutex muxblock;
map<thread::id, int> A;            // Contador para threads bloqueadas
map<int, int> entradas;            // Contador de entradas de cada pessoa
queue<pair<thread::id, Pessoa>> bloqueadas; // Fila de threads bloqueadas

void wakeup(thread::id t) {
    // Função para acordar uma thread bloqueada
    std::lock_guard lock(muxwup);
    A[t]++;
}

void p_pesoTotal() {
    // Função para imprimir o peso total atual do barco
    std::lock_guard lock(cout_mtx);
    printf("Peso total: %d\n", pesoAtual);
}

void apos_liberacao() {
    // Função chamada após todas as threads sincronizarem na barreira
    printf("Todas chegaram\n");
    p_pesoTotal();
}

// Barreira para sincronizar threads após todas entrarem no barco N vezes
barrier bar(N, apos_liberacao); 

void block() {
    // Função para bloquear uma thread até que seja liberada
    thread::id meu_id = this_thread::get_id();
    while (true) {
        {
            std::lock_guard lock(muxblock);
            if (A[meu_id] > 0) {
                A[meu_id]--;
                break; // Sai do loop quando liberada
            }
        }
    }
}

void requisita(Pessoa &pessoa, bool &entrou) {
    // Tenta colocar a pessoa no barco se o peso total permitir
    printf("Pessoa %d de peso %d tenta entrar no barco\n", pessoa._id, pessoa._peso);
    {
        std::lock_guard lock(muxRequisita);
        if (pesoAtual + pessoa._peso <= pesoMAX) {
            pesoAtual += pessoa._peso;
            entradas[pessoa._id]++;
            entrou = true;
            printf("Pessoa %d entra no barco\n", pessoa._id);
            return;
        } else {
            // Se não puder entrar, adiciona a pessoa à fila de bloqueadas
            printf("%d não conseguiu entrar no barco %d+%d extrapola %d\n", pessoa._id, pessoa._peso, pesoAtual, pesoMAX);
            bloqueadas.push(std::make_pair(this_thread::get_id(), pessoa));
        }
    }
    block(); // Bloqueia a thread até que seja liberada
}

void libera(Pessoa &pessoa) {
    // Libera a pessoa do barco, reduzindo seu peso do total
    {
        std::lock_guard lock(muxLibera);
        pesoAtual -= pessoa._peso;
        printf("%d saiu do barco reduzindo %d do peso total\n", pessoa._id, pessoa._peso);

        // Acorda a próxima pessoa na fila de bloqueadas, se houver
        if (!bloqueadas.empty()) {
            wakeup(bloqueadas.front().first);
            printf("%d retirada da fila de bloqueadas\n", bloqueadas.front().second._id);
            bloqueadas.pop();
        }
    }
}

void processo(Pessoa &pessoa) {
    // Função principal de cada thread/pessoa
    while (true) {
        // Checa se a pessoa já entrou o número suficiente de vezes no barco
        if ((entradas[pessoa._id] + 1) == N && !quaseTodas) {
            printf("Pessoa %d entrou %d vezes, esperando o resto entrar n-1 vezes\n", pessoa._id, entradas[pessoa._id]);
            bar.arrive_and_wait(); // Espera todas as threads chegarem
            quaseTodas = true;
        }
        if (entradas[pessoa._id] == N) {
            printf("Pessoa %d entrou %d vezes, esperando o resto entrar n vezes\n", pessoa._id, entradas[pessoa._id]);
            break; // Sai do loop se todas as entradas foram concluídas
        }
        
        bool entrou = false;
        requisita(pessoa, entrou); // Tenta entrar no barco

        if (!entrou) {
            continue; // Se não entrou, tenta novamente
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(time_ms)); // Tempo dentro do barco
        printf("\nPessoa %d de peso %d dentro do barco ~bebeu por %d milisegundos\n\n", pessoa._id, pessoa._peso, time_ms);
        libera(pessoa); // Libera a pessoa do barco
        p_pesoTotal();  // Imprime o peso total
    }
}

int main() {
    std::vector<std::thread> threads;
    std::srand(std::time(0)); // Inicializa o gerador de números aleatórios

    // Cria threads para cada pessoa, com peso aleatório entre 40 e 150
    for (int i = 0; i < N; ++i) {
        int peso = std::rand() % 111 + 40;
        Pessoa *pessoa = new Pessoa(i, peso);
        threads.push_back(std::thread(processo, std::ref(*pessoa)));
    }

    // Espera todas as threads finalizarem
    for (auto &th : threads) {
        th.join();
    }

    // Imprime o número de entradas para cada pessoa
    for (auto &entrada : entradas) {
        cout << entrada.first << " entrou " << entrada.second << " vezes\n";
    }
    return 0;
}