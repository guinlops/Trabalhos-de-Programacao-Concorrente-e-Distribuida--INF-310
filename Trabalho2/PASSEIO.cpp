#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>
using namespace std;

const int N = 30; // Numero de clientes
const int C = 3; // Numero de cadeiras;
const int MAXENTRADAS = 9;
const int MAXINGRESSOS = N * MAXENTRADAS; // Total de passeios esperados

class MonitorPasseio{
private:
    condition_variable Clientes, Barco;
    int lotacao = 0;
    int rodada=0; // Número da rodada atual (mínimo de passeios realizados por qualquer cliente)
    bool emPasseio=true;
public:
    mutex mux;
    bool entrou[N]; // Array que indica se o cliente está atualmente no barco
    int entradas[N] = {0}; // Contador de passeios realizados por cada cliente
    int ingressos = 0; // Controle do número total de entradas no barco
    void espera_encher(){   
        unique_lock<mutex> lck(mux);
        for(int i=0;i <N;++i){
            printf("%d ",entradas[i]);
        }
        printf("\n");
   
        rodada=*std::min_element(entradas, entradas + N); // Define a rodada como o menor número de passeios realizados
        printf("Esperando encher....\n");
        emPasseio=false;
        Clientes.notify_all();
        Barco.wait(lck);
        printf("Barco cheio\n");
    }

    void espera_esvaziar(){
        unique_lock<mutex> lck(mux);
        //Garantia que os clientes só podem sair se 
        printf("esperando esvaziar....\n");
        Clientes.notify_all(); //Libera os clientes dentro do barco e os clientes que nao conseguiram entrar;
        //Enquanto esvazia, os clientes liberados vao continuar tentando entrar;
        Barco.wait(lck); // Aguarda até que todos os clientes saiam do barco
    }

    void entra_no_barco(int id)
    {
        unique_lock<mutex> lck(mux);
        if(rodada!=entradas[id]){
            printf("                            Cliente %d tenta entrar mas nao consegue pois ainda há clientes a fazer o mesmo número de passeios q ~this\n",id);
        }else if(lotacao==C){
            printf("                            CLiente %d tenta entrar mas nao consegue. Barco ainda está cheio...\n",id);
        }else if (!emPasseio){
            printf("                            Cliente %d tenta entrar mas nao consegue. Barco está em passeio ou o próximo passeio ainda não comecou\n",id);

        }
        // Espera que as condições de entrada sejam satisfeitas: barco com espaço, fora de passeio, rodada correta
        Clientes.wait(lck, [this, id]() -> bool
                        {return lotacao <C && !emPasseio && rodada==entradas[id];}); 

        entrou[id]=true;
        entradas[id]++;
        lotacao++;
        ingressos++;
        printf("%d entra no barco\n",id);
        printf("    lotacao: %d\n",lotacao);
        
        if(lotacao==C){
            emPasseio=true; //Lotacao C, Barco já está em passeio ( portas fechadas!!)
            Barco.notify_one();
        
        }
        //Cliente espera o passeio acabar
        Clientes.wait(lck);
        printf("%d saiu do wait e vai sair do barco\n",id);

    }

    void sai_do_barco(int id)
    {
        unique_lock<mutex> lck(mux);

        if (entrou[id] == true)
        {
            lotacao--;
            printf("                            clinte %d saiu do barco\n", id);
            if(entradas[id]!=MAXENTRADAS){
                entrou[id]=false;
            }
            if (lotacao == 0)
            { // Ultimo sair notifica o Barco para entrar denovo
                printf("todos sairam\n");
                Barco.notify_one();
            }
        }else{
            printf("ERRO! CLIENTE NAO ENTROU NO BARCO MAS SAIU \n");
        }
    }

    MonitorPasseio() {}
    ~MonitorPasseio() {}
};

MonitorPasseio monitor; // monitor

void barco()
{
    int qntd_passeios = 0;

    while (!(monitor.ingressos == MAXINGRESSOS)) { // Enquanto todos os ingressos nao tenham sido usados;

        monitor.espera_encher();
        // faz passeio
        printf("Passeando...............\n");
        this_thread::sleep_for(std::chrono::milliseconds(100));
        monitor.espera_esvaziar();
        
        qntd_passeios++;
       
    }

    
    printf("%d Ingressos foram usados!!\n", monitor.ingressos);
    printf("O barco realizou %d passeios sendo %d = %d Pessoas / %d cadeiras * %d entradas \n", qntd_passeios, qntd_passeios, N, C,MAXENTRADAS);
}

void cliente(int id){
    int n_passeios;

    while (true) {
    {
        unique_lock<mutex> lck(monitor.mux); 
        if (monitor.entrou[id]) break;
    }
    monitor.entra_no_barco(id);
    monitor.sai_do_barco(id); 
    }
}

int main()
{
    // Cria threads para barco e clientes
    thread t_barco(barco);
    vector<thread> threads;

    for (int i = 0; i < N; i++)
    {
        threads.push_back(thread(cliente, i));
    }

    // Aguarda todas as threads concluírem
    t_barco.join();
    for (auto &t : threads)
    {
        t.join();
    }

    for (int i = 0; i < N; i++)
    {
        printf("%d entrou %d vezes\n", i, monitor.entradas[i]);
    }
    return 0;
}