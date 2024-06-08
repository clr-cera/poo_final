#include "register.h"
#include <stdio.h>

typedef struct _player PLAYER;

#define FIXED_FIELDS_SIZE 33
#define ERROR_CHAR '?'
#define ERROR_INT -100000
#define TOP_REGISTER (-4)

struct _player {
  int id;
  int age;
  int player_name_size;
  char *player_name;
  int nationality_size;
  char *nationality;
  int club_size;
  char *name_club;
  char removed;
  int register_size;
  long long int next;
};

// Deallocating the struct player
void Delete_Player(PLAYER *player) {
  free(player->name_club);
  free(player->nationality);
  free(player->player_name);
  free(player);
}
// Create the player register and return a boolean if it was successful
bool Create_Player(FILE *destiny, FILE *csv) {
  // creating the register
  PLAYER *player = (PLAYER *)malloc(sizeof(PLAYER));
  if (player == NULL) {
    return false;
  }
  // read each attributes from the player data

  char separators[3] = {',', '\n', '\r'};
  player->removed = '0';
  player->next = -1;

  int nothing; // if we do not need to store the size of the string
  char *field;

  // Read and parse the player ID
  field = Read_From_File(csv, separators, 1, &nothing);
  if (field == NULL) { // end of csv or empty field
    free(player);
    return false;
  }
  player->id = atoi(field);
  free(field);

  // Read and parse the player age
  field = Read_From_File(csv, separators, 1, &nothing);
  if (field == NULL) {
    player->age = -1; // If the age field is empty
  } else {
    player->age = atoi(field);
    free(field);
    if (player->age == 0) {
      player->age = -1;
    }
  }
  int player_name_size = 0;
  int nationality_size = 0;
  int club_size = 0;
  int register_size = FIXED_FIELDS_SIZE;

  // Read the player's name
  player->player_name = Read_From_File(csv, separators, 0, &player_name_size);
  // Read the player's nationality
  player->nationality = Read_From_File(csv, separators, 0, &nationality_size);
  // Read the player's club name
  player->name_club = Read_From_File(csv, separators, 0, &club_size);

  player->player_name_size = player_name_size;
  player->nationality_size = nationality_size;
  player->club_size = club_size;

  register_size += player_name_size;
  register_size += nationality_size;
  register_size += club_size;
  player->register_size = register_size;

  // if the fwrite write in the file sucesfully, return true
  fwrite(&player->removed, sizeof(char), 1, destiny);
  fwrite(&player->register_size, sizeof(int), 1, destiny);
  fwrite(&player->next, sizeof(long long int), 1, destiny);
  fwrite(&player->id, sizeof(int), 1, destiny);
  fwrite(&player->age, sizeof(int), 1, destiny);
  fwrite(&player->player_name_size, sizeof(int), 1, destiny);
  fwrite(player->player_name, player->player_name_size, 1, destiny);
  fwrite(&player->nationality_size, sizeof(int), 1, destiny);
  fwrite(player->nationality, player->nationality_size, 1, destiny);
  fwrite(&player->club_size, sizeof(int), 1, destiny);
  fwrite(player->name_club, player->club_size, 1, destiny);

  /* deleting the struct and the allocated memory because we already wrote the
   * data on the file */
  Delete_Player(player);

  return true;
}
// Print one player
void Print_Player(PLAYER *player) {
  // if we have fields missing
  if (player->player_name_size == 0) {
    strcpy(player->player_name, "SEM DADO");
  }
  if (player->nationality_size == 0) {
    strcpy(player->nationality, "SEM DADO");
  }
  if (player->club_size == 0) {
    strcpy(player->name_club, "SEM DADO");
  }
  // printf("ID: %d\n", player->id);
  printf("Nome do Jogador: %s\n", player->player_name);
  printf("Nacionalidade do Jogador: %s\n", player->nationality);
  printf("Clube do Jogador: %s\n\n", player->name_club);
}

// Read a player from a file.bin and stores it on a struct and return the struct
PLAYER *Read_Player(FILE *file) {
  PLAYER *player = (PLAYER *)malloc(sizeof(PLAYER));
  if (player == NULL)
    return NULL;

  fread(&player->removed, sizeof(char), 1, file);

  fread(&player->register_size, sizeof(int), 1, file);
  // Decrementing from the total size to check if the entire record has been
  // read, even if there is a $ sign
  int reg_size = player->register_size - sizeof(player->register_size) -
                 sizeof(player->removed);

  fread(&player->next, sizeof(long long int), 1, file);
  reg_size = reg_size - sizeof(player->next);

  fread(&player->id, sizeof(int), 1, file);
  reg_size = reg_size - sizeof(player->id);

  fread(&player->age, sizeof(int), 1, file);
  reg_size = reg_size - sizeof(player->age);

  fread(&player->player_name_size, sizeof(int), 1, file);
  reg_size = reg_size - sizeof(player->player_name_size);

  player->player_name =
      (char *)malloc(sizeof(char) * (player->player_name_size + 1));
  fread(player->player_name, sizeof(char), player->player_name_size, file);
  reg_size = reg_size - player->player_name_size;

  /* adding '\0' at the end of the string in the struct field to enable the use
   * of functions from the <string.h> library */
  player->player_name[player->player_name_size] = '\0';

  fread(&player->nationality_size, sizeof(int), 1, file);
  reg_size = reg_size - sizeof(player->nationality_size);

  player->nationality =
      (char *)malloc(sizeof(char) * (player->nationality_size + 1));
  fread(player->nationality, sizeof(char), player->nationality_size, file);
  player->nationality[player->nationality_size] = '\0';
  reg_size = reg_size - player->nationality_size;

  fread(&player->club_size, sizeof(int), 1, file);
  reg_size = reg_size - sizeof(player->club_size);

  player->name_club = (char *)malloc(sizeof(char) * (player->club_size + 1));
  fread(player->name_club, sizeof(char), player->club_size, file);
  player->name_club[player->club_size] = '\0';
  reg_size = reg_size - player->club_size;

  if (reg_size != 0) {
    char trash[reg_size];
    fread(&trash, sizeof(char), reg_size, file);
  }

  if (player->removed == '1') {
    Delete_Player(player);
    return NULL;
  }
  return player;
}
// Read a player that is removed from a file.bin and stores it on a struct and
// return the struct
PLAYER *Read_Removed_Player(FILE *file) {
  PLAYER *player = (PLAYER *)malloc(sizeof(PLAYER));
  if (player == NULL)
    return NULL;

  fread(&player->removed, sizeof(char), 1, file);

  fread(&player->register_size, sizeof(int), 1, file);

  fread(&player->next, sizeof(long long int), 1, file);

  fread(&player->id, sizeof(int), 1, file);

  fread(&player->age, sizeof(int), 1, file);

  fread(&player->player_name_size, sizeof(int), 1, file);

  player->player_name =
      (char *)malloc(sizeof(char) * (player->player_name_size));
  fread(player->player_name, sizeof(char), player->player_name_size, file);

  fread(&player->nationality_size, sizeof(int), 1, file);

  player->nationality =
      (char *)malloc(sizeof(char) * (player->nationality_size));
  fread(player->nationality, sizeof(char), player->nationality_size, file);

  fread(&player->club_size, sizeof(int), 1, file);

  player->name_club = (char *)malloc(sizeof(char) * (player->club_size));
  fread(player->name_club, sizeof(char), player->club_size, file);

  return player;
}
// Read a player from the terminal and returns it as a struct
PLAYER *Read_Inserted_Player() {
  PLAYER *player = (PLAYER *)malloc(sizeof(PLAYER));
  if (player == NULL) {
    // failed memory allocation
    return (NULL);
  }
  // since the size of the fields is unknown, there are temporary strings to
  // receive the user input they are also used in case there is a field with
  // NULO
  char temporary_name[50];
  char temporary_club[50];
  char temporary_nationality[50];
  int temporary_id = 0;
  int temporary_age;
  char buffer;
  // with the exception of the Id, all fields can be NULO, with means they are
  // empty in that case, the returned string from the functions are empty, thus,
  // size = 0

  scanf("%d", &temporary_id);
  scanf("%c", &buffer);
  temporary_age = Read_Age_Insert_Player();
  scan_quote_string(temporary_name);
  scan_quote_string(temporary_nationality);
  scan_quote_string(temporary_club);

  player->id = temporary_id;
  player->age = temporary_age;
  // here is the name
  if (strlen(temporary_name) == 0) {
    player->player_name_size = 0;
    player->player_name = NULL;
  } else {
    player->player_name_size = strlen(temporary_name);
    player->player_name =
        (char *)malloc(sizeof(char) * (player->player_name_size));
    memcpy(player->player_name, temporary_name, player->player_name_size);
  }
  // here is the club
  if (strlen(temporary_club) == 0) {
    player->club_size = 0;
    player->name_club = NULL;
  } else {
    player->club_size = strlen(temporary_club);
    player->name_club = (char *)malloc(sizeof(char) * (player->club_size));
    memcpy(player->name_club, temporary_club, player->club_size);
  }
  // here is the nationality

  if (strlen(temporary_nationality) == 0) {
    player->nationality_size = 0;
    player->nationality = NULL;
  } else {
    player->nationality_size = strlen(temporary_nationality);
    player->nationality =
        (char *)malloc(sizeof(char) * (player->nationality_size));
    // the use of memcpy is necessary since the strings don't have "\0" in the
    // end
    memcpy(player->nationality, temporary_nationality,
           player->nationality_size);
  }
  // by default, registter have next = -1
  player->next = -1;
  player->removed = '0';
  // 33 is the total of the fields with a fixed size, such as ID, AGE, REMOVED,
  // NEXT and size of variable camps
  player->register_size = 33 + player->player_name_size + player->club_size +
                          player->nationality_size;

  return (player);
}
// Writes the new player into a removed register or last byteoffset
void Write_player(int extra_space, long long int present,
                  PLAYER *inserted_player, FILE *data_file, int true_size) {
  fseek(data_file, present,
        SEEK_SET); // aqui to reiniciando o ponteiro para o começo do registro
  fwrite(&inserted_player->removed, sizeof(char), 1, data_file);
  fwrite(&true_size, sizeof(int), 1, data_file);
  fwrite(&inserted_player->next, sizeof(long long int), 1, data_file);
  fwrite(&inserted_player->id, sizeof(int), 1, data_file);
  fwrite(&inserted_player->age, sizeof(int), 1, data_file);
  fwrite(&inserted_player->player_name_size, sizeof(int), 1, data_file);
  fwrite(inserted_player->player_name, inserted_player->player_name_size, 1,
         data_file);
  fwrite(&inserted_player->nationality_size, sizeof(int), 1, data_file);
  fwrite(inserted_player->nationality, inserted_player->nationality_size, 1,
         data_file);
  fwrite(&inserted_player->club_size, sizeof(int), 1, data_file);
  fwrite(inserted_player->name_club, inserted_player->club_size, 1,
         data_file); // escreve todos os registros com seus tamanhos apropriados
  if (extra_space != 0) {
    // most registers are bound to have extra space after the insertion of a new
    // player, since to be used for insertion, the removed register must be
    // bigger or equal to the new one, so the extra space is filled with "$"
    char *string = (char *)malloc(sizeof(char) * extra_space);
    for (int i = 0; i < extra_space; i++) {
      string[i] = '$';
    }
    fwrite(string, extra_space, 1,
           data_file); // writes the extra "$"
    free(string);
  }
}
// finds the place with the most appropriate size to fit the new register and
// returns its address(byteoffset)
long long int Best_Fit(FILE *data_file, PLAYER *inserted_player,
                       HEADER *header) {
  // size of the register that is going to be inserted
  int size = inserted_player->register_size;
  // the TRUE SIZE of the register, since after insertion, the size should stay
  // the same as it was before
  int size_register = 0;
  long long int present, prior = 0;
  present = Get_Top(header);
  int removed_registers = Get_Num_Rem(header);
  long long int next_byte_offset = Get_Next_Byteoffset(header);
  // runs through the removed registers trying to find one that can fit the new
  // register (inserted register)
  for (int j = 0; j < removed_registers; j++) {
    fseek(data_file, present, SEEK_SET);
    PLAYER *possible_space = Read_Removed_Player(data_file);
    size_register = possible_space->register_size;
    // the removed must have a bigger or equal size
    if (possible_space->register_size >= size) {
      long long int next = possible_space->next; // prior will use it
      // saves the next in the linked list, sice after the write_player this
      // byteofffset will be lost
      size_register = possible_space->register_size;
      int extra_space = possible_space->register_size - size;

      Write_player(extra_space, present, inserted_player, data_file,
                   size_register);

      if (prior == 0) {
        // case that it is inserted in the first register of the removed linked
        // list
        Set_Local_Top(header, next);
      } else {
        // makes the prior removed point to the next removed register,
        // basically removing the register in which the new one will be inserted
        // from the list
        fseek(data_file, prior + 5, SEEK_SET);
        fwrite(&next, sizeof(long long int), 1, data_file);
      }
      // updates the number of removed registers
      Set_Local_Rem(header, removed_registers - 1);
      return present;
    } else {
      // runs through the removed registers
      prior = present;
      present = possible_space->next;
    }
  }

  // specif case in which the inserted player doens't find a removed register
  // that fits it so it is inserted in the end of the file
  present = next_byte_offset;
  size_register = inserted_player->register_size;
  Write_player(0, present, inserted_player, data_file, size_register);
  next_byte_offset = ftell(data_file);
  // new byteoffset must be updated
  Set_Local_NextByteOffset(header, next_byte_offset);
  return present;
}

// Create and return the struct target player to be used as a reference
PLAYER *Create_Target_Player() {
  PLAYER *target_player = (PLAYER *)malloc(sizeof(PLAYER));
  if (target_player == NULL)
    return NULL;

  // initialize the values that are not conditions
  target_player->removed = '0';
  target_player->register_size = 0;
  target_player->next = 0;
  return target_player;
}

// Check if the player pass the conditions
int Pass_Conditions(int conditions[], PLAYER *target_player, PLAYER *player) {
  bool pass_conditions = true;
  // if ID is a condition and player's is different from target's, condition
  // is not met
  if (conditions[ID] && Get_Id(target_player) != Get_Id(player)) {
    pass_conditions = false;
  }
  // if AGE is a condition and player's is different from target's,
  // condition is not met
  if (conditions[AGE] && Get_Age(target_player) != Get_Age(player)) {
    pass_conditions = false;
  }
  // if NAME_PLAYER is a condition and player's is different from target's,
  // condition is not met
  if (conditions[NAME_PLAYER] &&
      0 != (strcmp(Get_Name(target_player), Get_Name(player)))) {
    pass_conditions = false;
  }
  // if NATIONALITY is a condition and player's is different from target's,
  // condition is not met
  if (conditions[NATIONALITY] &&
      0 != (strcmp(Get_Nationality(target_player), Get_Nationality(player)))) {
    pass_conditions = false;
  }
  // if CLUB is a condition and player's is different from target's,
  // condition is not met
  if (conditions[CLUB] &&
      0 != (strcmp(Get_Club(target_player), Get_Club(player)))) {
    pass_conditions = false;
  }
  return pass_conditions;
}

// Completes the array conditions based on the necessary conditions that the
// user wants
void Conditions(int *conditions, PLAYER *target_player, char *str_name,
                char *str_nationality, char *str_club) {
  int num_parameters, num;
  char str_parameter[50];

  /* vector to store which conditions I want to use, so I add 1 to the index
  that represent the condition I want, and 0 if opposite*/

  // how many parameters does the user wants to use in order to search
  scanf("%d", &num_parameters);
  for (int i = 0; i < num_parameters; i++) {
    // read which parameter is being searched, putting them in target player
    // and setting condition boolean array
    scanf("%s", str_parameter);
    if (!(strcmp(str_parameter, "id"))) {
      scanf("%d", &num);
      Set_Id_Target_Player(num, target_player);
      conditions[ID] = 1;
    }
    if (!(strcmp(str_parameter, "idade"))) {
      scanf("%d", &num);
      Set_Age_Target_Player(num, target_player);
      conditions[AGE] = 1;
    }
    if (!(strcmp(str_parameter, "nomeJogador"))) {
      scan_quote_string(str_name);
      Set_Name_Target_Player(str_name, target_player);
      conditions[NAME_PLAYER] = 1;
    }
    if (!(strcmp(str_parameter, "nacionalidade"))) {
      scan_quote_string(str_nationality);
      Set_Nationality_Target_Player(str_nationality, target_player);
      conditions[NATIONALITY] = 1;
    }
    if (!(strcmp(str_parameter, "nomeClube"))) {
      scan_quote_string(str_club);
      Set_Club_Target_Player(str_club, target_player);
      conditions[CLUB] = 1;
    }
  }
}

bool Set_Id_Target_Player(int id, PLAYER *target_player) {
  if (target_player == NULL)
    return false;
  target_player->id = id;
  return true;
}

bool Set_Age_Target_Player(int age, PLAYER *target_player) {
  if (target_player == NULL)
    return false;
  target_player->age = age;
  return true;
}

bool Set_Name_Target_Player(char *name, PLAYER *target_player) {
  if (target_player == NULL)
    return false;
  target_player->player_name = name;
  return true;
}

bool Set_Nationality_Target_Player(char *nationality, PLAYER *target_player) {
  if (target_player == NULL)
    return false;
  target_player->nationality = nationality;
  return true;
}

bool Set_Club_Target_Player(char *club, PLAYER *target_player) {
  if (target_player == NULL)
    return false;
  target_player->name_club = club;
  return true;
}

char Get_Removed(PLAYER *p) {
  if (p == NULL)
    return ERROR_CHAR;
  return p->removed;
}

int Get_Id(PLAYER *p) {
  if (p == NULL)
    return ERROR_INT;
  return p->id;
}

int Get_Age(PLAYER *p) {
  if (p == NULL)
    return ERROR_INT;
  return p->age;
}

int Get_Register_Size(PLAYER *p) {
  if (p == NULL)
    return ERROR_INT;
  return p->register_size;
}

long long int Get_Next(PLAYER *p) {
  if (p == NULL)
    return ERROR_INT;
  return p->next;
}

char *Get_Name(PLAYER *p) {
  if (p == NULL)
    return NULL;
  return p->player_name;
}

char *Get_Nationality(PLAYER *p) {
  if (p == NULL)
    return NULL;
  return p->nationality;
}

char *Get_Club(PLAYER *p) {
  if (p == NULL)
    return NULL;
  return p->name_club;
}
