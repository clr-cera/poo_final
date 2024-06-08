#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _header HEADER;

HEADER *Create_Header(FILE *destiny);

bool Set_Local_Reg(HEADER *header, int new_reg_file);
bool Set_Local_Rem(HEADER *header, int new_rem_file);
bool Set_Local_Top(HEADER *header, long long int new_top);
bool Set_Local_NextByteOffset(HEADER *header, long long int new_nextbyteoffset);
bool Update_Header(HEADER *h, FILE *f, char new_status);

HEADER *Read_Header(FILE *file);

char Get_Status(HEADER *header);
int Get_Num_Reg(HEADER *header);
int Get_Num_Rem(HEADER *header);
long long int Get_Top(HEADER *header);
long long int Get_Next_Byteoffset(HEADER *header);

void Delete_header(HEADER *header);
