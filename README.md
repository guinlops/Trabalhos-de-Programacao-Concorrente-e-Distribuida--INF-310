# Projeto: Simulação de Controle de Acesso a um Barco com Threads em C++

## Descrição
Este projeto implementa uma simulação de controle de acesso a um barco utilizando concorrência com `std::thread`, `std::mutex`, `std::barrier` e outras estruturas de sincronização da STL do C++. O objetivo é garantir que um determinado número de pessoas possa entrar e sair do barco respeitando um peso máximo permitido.

## Funcionalidades
- Gerenciamento de um barco com capacidade de peso limitada.
- Controle de acesso utilizando mutexes para evitar condições de corrida.
- Uso de uma barreira para sincronizar a finalização de todas as threads.
- Implementação de uma fila de espera para pessoas que não conseguem embarcar de imediato.
- Simulação do tempo de permanência no barco antes da liberação.

## Requisitos
- Compilador C++ com suporte a C++20 ou superior (devido ao uso de `std::barrier`).
- Sistema operacional compatível com `std::thread` e sincronização de threads.

## Compilação e Execução
```sh
# Compilar o código
g++ -std=c++2b barco.cpp -o barco

# Executar
./barco

# Para salvar a saída em um arquivo de debug
./barco > output.txt
```

## Observações
- A ordem das mensagens no console pode variar devido à concorrência.
- Mensagens de depuração podem impactar o desempenho da execução.
- O tempo de permanência no barco é fixado em 100ms.

## Estrutura do Código
- **`Pessoa`**: Estrutura representando cada pessoa que tenta embarcar.
- **`requisita`**: Controla o acesso ao barco, garantindo que o peso limite não seja excedido.
- **`libera`**: Remove uma pessoa do barco e libera espaço para outra.
- **`processo`**: Função executada por cada thread para gerenciar a entrada e saída de uma pessoa.
- **`barrier`**: Sincroniza todas as threads após um número determinado de entradas no barco.

## Trabalho 3: Processamento Paralelo com CUDA
Este trabalho envolve a implementação de um algoritmo de soma utilizando CUDA para paralelização da computação. O código utiliza memória compartilhada para otimizar o acesso aos dados e melhorar o desempenho.

### Funcionalidades
- Implementação de um kernel CUDA para processamento paralelo da soma de elementos em uma janela de alcance (`RANGE`).
- Uso de memória compartilhada para reduzir acessos redundantes à memória global.
- Comparação de tempos de execução entre CPU e GPU.

### Compilação e Execução
```sh
# Compilar o código CUDA
nvcc -o soma_cuda soma.cu

# Executar
./soma_cuda
```

### Observações
- O código utiliza `cudaMalloc`, `cudaMemcpy` e `cudaFree` para gerenciar a memória da GPU.
- A sincronização entre threads é feita com `__syncthreads()`.
- A configuração de blocos e threads afeta o desempenho da execução.

## Autor
Desenvolvido como um experimento de programação concorrente utilizando C++ moderno e CUDA.

