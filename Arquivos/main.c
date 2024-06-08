/*
Trabalho Introdutório - Arquivos
-------------------------------------------
Integrantes:

Eduarda Tuboy Nardin nUSP 13732495
Rafael Brazolin Alves Mansur nUSP 14604020

Grupo: 11
*/

#include "./binary_files/header.h"
#include "./binary_files/register.h"
#include "./utilities/funcoes_fornecidas.h"
#include "./utilities/tools.h"
#include "functionalities.h"

int main() {
  // Variable inicialization
  char op, c, file_in[50], file_out[50];
  int num_search;
  bool flag = true;

  do {
    op = getchar();
    switch (op) {

    case '1':
      // Create Table
      c = getchar(); // consume the buffer
      scanf("%s", file_in);
      scanf("%s", file_out);
      Create(file_in, file_out);
      break;

    case '2':
      // Select Table
      c = getchar(); // consume the buffer
      scanf("%s", file_in);
      Select(file_in);
      break;

    case '3':
      // Select Conditional
      c = getchar(); // consume the buffer
      scanf("%s", file_in);
      c = getchar(); // consume the buffer
      scanf("%d", &num_search);
      // Execute as many conditional selections as necessary
      for (int i = 1; i <= num_search; i++) {
        printf("Busca %d\n\n", i);
        Select_Conditional(file_in);
      }
      break;

    case '4':
      //Create an index from the data file
      c = getchar(); // consume the buffer
      scanf("%s", file_in);
      scanf("%s", file_out);
      Create_Index(file_in, file_out, true);
      // printf("\nFim funcionalidade 4\ns");
      break;

    case '5':
      // Delete
      c = getchar(); // consume the buffer
      scanf("%s", file_in);
      scanf("%s", file_out);
      c = getchar(); // consume the buffer
      Create_Index(file_in, file_out, false);//Delete needs an index, so Create_Index does this function
      Delete(file_in, file_out);
      break;

    case '6':
      c = getchar(); // consume the buffer
      scanf("%s", file_in);
      scanf("%s", file_out);
      c = getchar(); // consume the buffer
      Create_Index(file_in, file_out, false);
      Insert(file_in, file_out);
      break;

      break;

    case '\n': // if is \n, ignore and continues reading
      break;

    default:
      // exit the program
      flag = false;
      break;
    }
  } while (flag);

  return 0;
}
