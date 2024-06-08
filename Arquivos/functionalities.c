#include "functionalities.h"
#include "./binary_files/register.h"
#include "./index_files/index.h"
#include "./utilities/tools.h"
#include <stdio.h>

// Create a binary file from a data csv file
void Create(char *file_in, char *file_out) {
  int player_counter = 0;
  // create the binary file f
  FILE *f = fopen(file_out, "wb");
  if (f == NULL) {
    printf("Falha no processamento do arquivo.");
    fclose(f);
    return;
  }
  // create a header to be written on binary file f
  HEADER *header_f = Create_Header(f);
  if (header_f == NULL) {
    printf("Falha no processamento do arquivo.");
    fclose(f);
    return;
  }
  // open csv file for reading player data
  FILE *csv = fopen(file_in, "r");
  if (csv == NULL) {
    printf("Falha no processamento do arquivo.");
    fclose(f);
    fclose(csv);
    return;
  }
  // consume CSV header
  Consume_Line(csv, 5);
  // create players until there is no data left in CSV file
  while (Create_Player(f, csv)) {
    player_counter++;
  };
  // store the last pointer position
  long long int byteoffset = ftell(f);
  Set_Local_Reg(header_f, player_counter);
  Set_Local_NextByteOffset(header_f, byteoffset);

  // updates the header
  if (!Update_Header(header_f, f, '1')) {
    printf("Falha no processamento do arquivo.");
    fclose(f);
    fclose(csv);
  }

  fclose(f);
  fclose(csv);
  binarioNaTela(file_out);
  Delete_header(header_f);
}

// Prints all players
void Select(char *file_in) {
  int num_reg = 0;
  // open the binary file f for reading
  FILE *f = fopen(file_in, "rb");
  if (f == NULL) {
    printf("Falha no processamento do arquivo.");
    fclose(f);
    return;
  }
  // read header from binary file f
  HEADER *header = Read_Header(f);
  if (header == NULL) {
    printf("Falha no processamento do arquivo.");
    fclose(f);
    return;
  }
  // terminate if status is set to 0 (incoherent)
  if (Get_Status(header) == '0') {
    printf("Falha no processamento do arquivo.");
    fclose(f);
    return;
  }
  // terminate if there are no registers to be read
  num_reg = Get_Num_Reg(header);
  if (num_reg == 0) {
    printf("Registro inexistente.\n\n");
    fclose(f);
    return;
  }
  // read all players, printing the ones not removed (null player pointer)
  while (num_reg != 0) {
    PLAYER *player = Read_Player(f);
    if (player != NULL) {
      Print_Player(player);
      num_reg--;
      Delete_Player(player);
    }
  }
  Delete_header(header);
  fclose(f);
}

// Prints only certain selected players
void Select_Conditional(char *file_in) {
  // variable initialization
  int player_counter = 0, num_reg = 0, num;
  char *str_name = (char *)malloc(sizeof(char) * 50);
  char *str_nationality = (char *)malloc(sizeof(char) * 50);
  char *str_club = (char *)malloc(sizeof(char) * 50);
  int conditions[5] = {0, 0, 0, 0, 0};

  // open binary file f for reading player data
  FILE *f = fopen(file_in, "rb");
  if (f == NULL) {
    printf("Falha no processamento do arquivo.");
    fclose(f);
    return;
  }
  // read header from binary file f
  HEADER *header = Read_Header(f);
  if (header == NULL) {
    printf("Falha no processamento do arquivo.");
    fclose(f);
    return;
  }
  // terminate if status is set to 0 (incoherent)
  if (Get_Status(header) == '0') {
    printf("Falha no processamento do arquivo.");
    fclose(f);
    return;
  }
  // terminate if there are no registers to be read
  num_reg = Get_Num_Reg(header);
  if (num_reg == 0) {
    printf("Registro inexistente.\n\n");
    fclose(f);
    return;
  }
  // reading and setting the conditions to select the target players
  PLAYER *target_player = Create_Target_Player();
  Conditions(conditions, target_player, str_name, str_nationality, str_club);

  // comparing the target player to each player.
  // if the current player corresponds to the conditions, we print it
  while (num_reg != 0) {
    PLAYER *player = Read_Player(f);
    // if the player is NULL because a error or it is removed
    if (player == NULL) {
      Delete_Player(player);
      continue; // skip this player
    }
    // print player if conditions are met
    if (Pass_Conditions(conditions, target_player, player)) {
      Print_Player(player);
      player_counter++;
      if (conditions[ID] == 1) {
        fclose(f);
        return;
      }
    }
    num_reg--;
    Delete_Player(player);
  }

  // tell if no register met the criteria
  if (player_counter == 0) {
    printf("Registro inexistente.\n\n");
  }
  Delete_header(header);
  free(str_club);
  free(str_name);
  free(str_nationality);
  free(target_player);
  fclose(f);
}

// Create the index file based on a binary file
void Create_Index(char *bin_data, char *bin_idx, bool print_binary) {
  // create index file
  FILE *index_file = fopen(bin_idx, "wb");
  if (index_file == NULL) {
    printf("Falha no processamento do arquivo.");
    fclose(index_file);
    return;
  }
  // create index header
  IDX_HEADER *idx_header = Create_Idx_Header(index_file);
  // read register on data file
  FILE *data_file = fopen(bin_data, "rb");
  if (data_file == NULL) {
    printf("Falha no processamento do arquivo.");
    fclose(index_file);
    fclose(data_file);
    return;
  }
  // read the header so we can get the number of registers
  HEADER *header = Read_Header(data_file);
  if (Get_Status(header) == '0') {
    printf("Falha no processamento do arquivo.");
    fclose(index_file);
    fclose(data_file);
    return;
  }
  int num_reg = Get_Num_Reg(header);
  // writes the players on the index
  Write_Players(data_file, index_file, num_reg);
  // update header, localy and on the file
  Set_Idx_Status(idx_header, index_file, '1');

  // close files
  Delete_header(header);         // frees the header
  Delete_Idx_header(idx_header); // frees the idx_header
  fclose(data_file);
  fclose(index_file);
  if (print_binary) {
    // since others functions calls create_index, not always
    // binarioNaTela has to print
    binarioNaTela(bin_idx);
  }
}

// Delete seeks the players trought the data file and, if they exist, marks them
// as removed
void Delete(char *bin_data, char *bin_index) {
  // how many seeks the user wants to do
  int num_search;
  scanf("%d", &num_search);

  // opening the files
  FILE *data_file = fopen(bin_data, "r+b");
  if (data_file == NULL) {
    printf("Falha no processamento do arquivo.");
    fclose(data_file);
    return;
  }
  FILE *index_file = fopen(bin_index, "r+b");
  if (index_file == NULL) {
    printf("Falha no processamento do arquivo.");
    fclose(data_file);
    fclose(index_file);
    return;
  }
  // reads the header to sse if the files are consistent and trustworthy
  HEADER *header = Read_Header(data_file);
  if (Get_Status(header) == '0') {
    printf("Falha no processamento do arquivo.");
    fclose(data_file);
    fclose(index_file);
    return;
  }
  // number of VALID registers
  int num_reg = Get_Num_Reg(header);

  // load index, its useful if we are seaching by id
  IDX_PLAYER **local_index = Load_Index(index_file, num_reg);
  if (local_index == NULL) {
    // Fail to load index
    fclose(data_file);
    fclose(index_file);
  }
  // execute as many conditional selections as necessary
  for (int i = 1; i <= num_search; i++) {
    num_reg = Get_Num_Reg(header); // update every time the num reg
    Delete_Line(data_file, index_file, bin_data, local_index, num_reg, header);
    fseek(data_file, 25, SEEK_SET);
  }
  // updates header, we are altering the data file
  Update_Header(header, data_file, '1');

  fclose(index_file);
  fclose(data_file);

  num_reg = Get_Num_Reg(header);
  Rewrite_Index(bin_index, local_index, bin_data, num_reg);

  binarioNaTela(bin_data);
  binarioNaTela(bin_index);
}
// insert a new player into a removed space or the end of the data file
void Insert(char *bin_data, char *bin_index) {
  int num_insert;
  scanf("%d", &num_insert);
  // opening the files
  // the data file is the same throught the function
  FILE *data_file = fopen(bin_data, "r+b");
  if (data_file == NULL) {
    printf("Falha no processamento do arquivo.");
    fclose(data_file);
    return;
  }

  FILE *index_file = fopen(bin_index, "r+b");
  if (index_file == NULL) {
    printf("Falha no processamento do arquivo.");
    fclose(data_file);
    fclose(index_file);
    return;
  }
  HEADER *header = Read_Header(data_file);
  if (Get_Status(header) == '0') {
    printf("Falha no processamento do arquivo.");
    fclose(data_file);
    fclose(index_file);
    return;
  }
  // both number of registers and number of removed registers are necessary
  // for going through the index file and the data file
  int num_reg = Get_Num_Reg(header);
  int removed_registers = Get_Num_Rem(header);
  // loads index to find if the user is trying to insert an Id that already
  // exists
  IDX_PLAYER **local_index = Load_Index(index_file, num_reg);
  if (local_index == NULL) {
    // Fail to load index
    fclose(data_file);
    fclose(index_file);
    return;
  }

  for (int i = 0; i < num_insert; i++) {
    num_reg = Get_Num_Reg(header); // update header for each insert
    // reads the player given by the user in every iteration
    PLAYER *inserted_player = Read_Inserted_Player();
    if (inserted_player == NULL) {
      // invalid player, continues the program
      continue;
    }

    int id = Get_Id(inserted_player);
    // verify if the player is already on the data file
    int veryfier = Search_Index(id, num_reg, local_index);
    if (veryfier != NOT_FOUND) {
      // there can't be two players with identical IDs
      // continue because the user may want to insert other players in this
      // cycle
      continue;
    }

    removed_registers = Get_Num_Rem(header);

    long long int top = Get_Top(header);
    long long int prox_byte_offset = Get_Next_Byteoffset(header);
    long long int byteoffset = Best_Fit(data_file, inserted_player, header);

    Insert_Local_Index(&local_index, id, byteoffset, num_reg);
    Set_Local_Reg(header, num_reg + 1);
  }
  // with less removed regitsers, new registers, possible different top and
  // byteoffset the header needs to be updated, also so it can now have a valid
  // status
  Update_Header(header, data_file, '1');

  fclose(data_file);
  fclose(index_file);
  num_reg = Get_Num_Reg(header);
  // index file is rewritten, since it changed
  Rewrite_Index(bin_index, local_index, bin_data, num_reg);
  binarioNaTela(bin_data);
  binarioNaTela(bin_index);
}