#include "../binary_files/header.h"
#include "../binary_files/register.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _idx_header IDX_HEADER;
typedef struct _idx_player IDX_PLAYER;
#define NOT_FOUND (-1)

// Index File Header
IDX_HEADER *Create_Idx_Header(FILE *index);
bool Set_Idx_Status(IDX_HEADER *h, FILE *f, char new_status);
char Get_Idx_Status(IDX_HEADER *header);
void Delete_Idx_header(IDX_HEADER *header);
IDX_HEADER *Read_Idx_Header(FILE *index_file);

// Index File Player
IDX_PLAYER *Create_Idx_Player(FILE *file_bin);
IDX_PLAYER *Read_Idx_Player(FILE *index_file);
void Write_Players(FILE *data_file, FILE *index_file, int num_reg);
void Rewrite_Index(char *bin_index, IDX_PLAYER **local_index, char *bin_data,
                   int num_reg);

// Local Index
IDX_PLAYER **Load_Index(FILE *index_file, int num_reg);
void Print_Local_Index(IDX_PLAYER **local_index, int num_reg);
long long int Search_Index(int id, int num_reg, IDX_PLAYER **local_index);
bool Remove_Index(IDX_PLAYER ***local_index, int id, int num_reg);
bool Insert_Local_Index(IDX_PLAYER ***local_index, int new_id,
                        long long int new_byteoffset, int num_reg);

// List of Removed Players
void Find_Next(FILE *data_file, HEADER *header, long long int byteoffset,
               int size);
void Remove(FILE *data_file, long long int byteoffset, HEADER *header, int id,
            IDX_PLAYER **local_index);
void Delete_Line(FILE *data_file, FILE *index_file, char *bin_data,
                 IDX_PLAYER **local_index, int num_reg, HEADER *header);