#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *Read_From_File(FILE *file, const char *separators, int as_string,
                     int *string_len);
void Consume_Line(FILE *csv, int columns);
int Read_Age_Insert_Player();
