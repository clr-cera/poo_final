#include "./binary_files/header.h"
#include "./binary_files/register.h"
#include "./utilities/funcoes_fornecidas.h"
#include "./utilities/tools.h"

void Create(char *file_in, char *file_out);
void Select(char *file_in);
void Select_Conditional(char *file_in);
void Create_Index(char *bin_data, char *bin_idx, bool print_binary);
void Delete(char *bin_data, char *bin_index);
void Insert(char *bin_data, char *bin_index);