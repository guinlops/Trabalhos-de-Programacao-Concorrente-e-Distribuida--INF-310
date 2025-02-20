# Projeto: Simulação de Controle de Acesso a um Barco com Threads em C++

## Descrição
Este projeto implementa diferentes abordagens para o controle de acesso a um barco utilizando técnicas de programação concorrente e paralela. Cada trabalho explora diferentes mecanismos de sincronização e gestão de concorrência, como mutexes, monitores, barriers e blocks para garantir um funcionamento correto e eficiente.

## Trabalho 1: Controle de Acesso por Peso com Fila de Espera
Neste trabalho, o acesso ao barco é regulado com base em um limite de peso máximo. O sistema utiliza uma fila de espera para aqueles que não conseguem embarcar imediatamente e um mecanismo de bloqueio e liberação de threads para gerenciar o fluxo de entrada e saída.

### Técnicas Utilizadas
- **Mutexes (`std::mutex`)**: Protegem seções críticas, evitando condições de corrida.
- **Fila de espera (`std::queue`)**: Mantém a ordem de chegada das pessoas que não conseguiram entrar.
- **Wakeup (`wakeup`)**: Libera threads bloqueadas para permitir novas entradas.
- **Barrier (`std::barrier`)**: Garante que todas as pessoas tenham embarcado um número mínimo de vezes antes do término.
- **Blocks (`block`)**: Impede que uma thread prossiga antes de ser liberada.

### Compilação e Execução
```sh
# Compilar o código
g++ -std=c++2b barco1.cpp -o barco1

# Executar
./barco1

# Para salvar a saída em um arquivo de debug
./barco1 > output.txt
```

---

## Trabalho 2: Controle de Acesso com Monitores
Neste trabalho, foi adotado um monitor para gerenciar a entrada e saída dos passageiros. O acesso ao barco segue uma política de rodadas, garantindo que todos tenham um número mínimo de passeios.

### Técnicas Utilizadas
- **Monitores (`MonitorPasseio`)**: Gerencia o acesso concorrente ao barco, controlando a entrada e saída dos passageiros.
- **Mutexes (`std::mutex`)**: Protegem a estrutura de controle do monitor.
- **Variáveis de condição (`std::condition_variable`)**: Gerenciam o bloqueio e desbloqueio das threads conforme o barco enche e esvazia.
- **Controle de rodadas**: Garante que todos os clientes embarquem um número equilibrado de vezes.

### Compilação e Execução
```sh
# Compilar o código
g++ -std=c++20 barco2.cpp -o barco2

# Executar
./barco2

# Para salvar a saída em um arquivo de debug
./barco2 > output.txt
```

---

## Trabalho 3: Processamento Paralelo com CUDA
Este trabalho explora a paralelização de cálculos utilizando CUDA. A abordagem busca otimizar a soma de elementos em uma janela de alcance (`RANGE`) usando multiprocessamento em GPU.

### Técnicas Utilizadas
- **CUDA Threads**: Paraleliza a soma de elementos em múltiplos núcleos da GPU.
- **Memória compartilhada**: Reduz acessos redundantes à memória global, melhorando a eficiência.
- **Sincronização de threads (`__syncthreads()`)**: Garante a correta execução paralela sem conflitos de acesso.

### Compilação e Execução
```sh
# Compilar o código CUDA
nvcc -o soma_cuda soma.cu

# Executar
./soma_cuda
```

---

## Observações Gerais
- A execução dos códigos pode gerar saídas em ordens diferentes devido à concorrência.
- Mensagens de debug podem impactar o desempenho.
- Para melhor visualização dos resultados, recomenda-se salvar a saída em arquivos de log.

## Autor
Desenvolvido como um experimento de programação concorrente e paralela utilizando C++ moderno e CUDA.

