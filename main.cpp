#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main() {
  // ler as intruções
  auto inputFile = std::ifstream("./instrucoes.txt");
  // abrir arquivo de log para uso futuro
  auto outputFile = std::ofstream("./log.txt");

  std::string instrucoes[32];

  // inicializar pc (próxima instrução) como 0,
  // ir será atualizado dentro do loop do programa
  int pc = 0;
  int ir;

  std::string in1 = "00000000000000000000000000000000";
  std::string in2 = "00000000000000000000000000000000";
  char inva, ena, enb, f0, f1, inc, cin, a, b;
  // cin pode não ser definido se a instrução
  // for decodificada como diferente de soma,
  // então inicializaremos como 0 para escrever no log
  cin = '0';

  // carregar todas as intruções para o array
  for (int i = 0; i < 32; i++) {
    std::getline(inputFile, instrucoes[i]);
  }

  inputFile.close();

  // valores serão inseridos inversamente,
  // para permitir que o usuário dê input 1
  // como uma string de 32 bits
  std::string p1, p2;

  std::cout << "Valor de A:" << std::endl;
  std::cin >> p1;

  for (int i = 0; i < p1.length(); i++) {
    in1[31 - i] = p1[p1.length() - i - 1];
  }

  std::cout << "Valor de B:" << std::endl;
  std::cin >> p2;

  for (int i = 0; i < p2.length(); i++) {
    in2[31 - i] = p2[p2.length() - i - 1];
  }

  // executar o loop até que não hajam mais instruções a serem executadas.
  while (pc != -1) {
    // IR = PC
    ir = pc;
    // PC => PC + 1, para a próxima instrução
    if (instrucoes[ir + 1].compare("") != 1) {
      pc = -1;
    } else {
      pc++;
    }

    // carregar a instrução atual
    std::string inst = instrucoes[ir];

    // carregar o valor das entradas
    inva = inst[0];
    ena = inst[1];
    enb = inst[2];
    f0 = inst[3];
    f1 = inst[4];
    inc = inst[5];

    // carregaremos os inputs em variáveis diferentes,
    // pois podemos alterá-la para a presente instrução
    // e não queremos alterá-la para instruções futuras.
    auto in1l = in1;
    auto in2l = in2;

    // resultado provisório, 32 bits
    std::string result = "00000000000000000000000000000000";

    // se INC for 1, forçar o valor de vem-um como 1
    if (inc == '1') {
      cin = '1';
    }

    // ignorar input de A se ENA for 0
    if (ena == '0') {
      in1l = "00000000000000000000000000000000";
    }

    // ignorar input de B se ENB for 0
    if (enb == '0') {
      in2l = "00000000000000000000000000000000";
    }

    // valor de INVA e A são utilizados como input
    // de um XOR, logo, se ambos forem 1, deixarei
    // o primeiro input como sendo 0
    for (int i = 0; i < in1l.length(); i++) {
      in1l[i] = '0' + (in1l[i] ^ inva);
    }

    // simulação de um decodificador
    // F0  -  F1  -  Resultado
    // 0   -  0   -  AND
    // 0   -  1   -  OR
    // 1   -  0   -  !B
    // 1   -  1   -  SOMA
    if (f0 == '0') {
      // F0:0 F1:0
      if (f1 == '0') {
        // AND
        for (int i = 0; i < 32; i++) {
          // resultado será o resultado da operação AND
          // em todos os bits dos operandos
          a = in1l[31 - i];
          b = in2l[31 - i];
          result[31 - i] = (a & b);
        }
        // F0:0 F1:1
      } else {
        // OR
        for (int i = 0; i < 32; i++) {
          // resultado será o resultado da operação OR
          // em todos os bits dos operandos
          a = in1l[31 - i];
          b = in2l[31 - i];
          result[31 - i] = (a | b);
        }
      }
    } else {
      // F0:1 F1:0
      if (f1 == '0') {
        // !B
        for (int i = 0; i < 32; i++) {
          // bem simples, resultado será o inverso
          // de B
          result[31 - i] = '0' + !('0' - in2l[31 - i]);
        }
      } else {
        // F0:1 F1:1
        for (int i = 0; i < 32; i++) {
          // pegaremos o bit menos significante
          // de cada um dos operandos
          a = in1l[31 - i];
          b = in2l[31 - i];

          // transformaremos os caracteres em números e somá-los
          int s = ((a - '0') + (b - '0') + (cin - '0'));

          // utilizarei a mesma variável para vai-um
          // e vem-um, sendo o vai-um de uma operação
          // utilizada como vem-um da próxima
          if (s == 3) {
            // nesse caso, 1 + 1 + 1 = 11
            cin = '1';
            s = 1;
          } else if (s == 2) {
            // 1 + 1 = 10
            cin = '1';
            s = 0;
          } else {
            // 0 + 0 = 00
            cin = '0';
          }

          // escrevemos no bit menos significante
          result[31 - i] = '0' + s;
        }
      }
    }

    // escrever no log.
    outputFile << "PC:" << pc << (pc == -1 ? "" : " ") << " IR:" << ir
               << " A:" << in1l << " B:" << in2l << " S:" << result
               << " VAI-UM:" << cin << std::endl;

    // resetar o valor do vem-um
    cin = '0';
  }

  std::cout << "Resultados gravados em log.txt" << std::endl;
  outputFile.close();

  return 0;
}