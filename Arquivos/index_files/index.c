#include "index.h"

typedef struct _idx_header IDX_HEADER;
typedef struct _idx_player IDX_PLAYER;

#define ERROR_CHAR '?'
#define ERROR_INT -100000
#define NOT_FOUND (-1)

struct _idx_header {
  char status;
};

struct _idx_player {
  int id;
  long long int byteoffset;
};
// Create the header of the index and return the struct that was used to create
// the header
IDX_HEADER *Create_Idx_Header(FILE *index) {
  IDX_HEADER *idx_header = (IDX_HEADER *)malloc(sizeof(IDX_HEADER));
  if (idx_header == NULL)
    return NULL;

  idx_header->status = '0';

  if (!(fwrite(&idx_header->status, sizeof(char), 1, index)))
    return NULL;

  return idx_header;
}

bool Set_Idx_Status(IDX_HEADER *h, FILE *f, char new_status) {
  if (h == NULL) {
    return false;
  }
  h->status = new_status;
  // positioning the pointer to the appropriate field
  fseek(f, 0, SEEK_SET);
  // if fwrite returns 1, this means the writing was successful
  return fwrite(&h->status, sizeof(char), 1, f) == 1;
}

char Get_Idx_Status(IDX_HEADER *header) {
  if (header == NULL) {
    return ERROR_CHAR; // error
  }
  return header->status;
}
// Delete the index header
void Delete_Idx_header(IDX_HEADER *header) {
  free(header);
  header = NULL;
}
// Read index header
IDX_HEADER *Read_Idx_Header(FILE *index_file) {
  IDX_HEADER *idx_header = (IDX_HEADER *)malloc(sizeof(IDX_HEADER));
  if (idx_header == NULL)
    return NULL;
  fread(&idx_header->status, sizeof(char), 1, index_file);
  return idx_header;
}
// Read index player
IDX_PLAYER *Read_Idx_Player(FILE *index_file) {
  IDX_PLAYER *idx_player = (IDX_PLAYER *)malloc(sizeof(IDX_PLAYER));
  if (idx_player == NULL)
    return NULL;
  fread(&idx_player->id, sizeof(int), 1, index_file);
  fread(&idx_player->byteoffset, sizeof(long long int), 1, index_file);
  return idx_player;
}

// Create the index players
IDX_PLAYER *Create_Idx_Player(FILE *file_bin) {
  // as the players are read, we get their byteoffset
  long long int byteoffset = ftell(file_bin);
  IDX_PLAYER *idx_player = (IDX_PLAYER *)malloc(sizeof(IDX_PLAYER));
  // Read player
  PLAYER *player = Read_Player(file_bin);

  if (player == NULL) {
    return NULL;
  }
  // the index only has the byteoffset and id of the players
  idx_player->id = Get_Id(player);
  if (idx_player->id == ERROR_INT) {
    return NULL;
  }
  idx_player->byteoffset = byteoffset;

  return idx_player;
}
// Function that compares two players by id. Used to order the players on index
// if id.playerA > id.playerB returns a positive number
// if id.playerA < id.playerB returns a negative number
// if id.playerA == id.playerB returns 0
int compareById(const void *a, const void *b) {
  IDX_PLAYER *playerA = *(IDX_PLAYER **)a;
  IDX_PLAYER *playerB = *(IDX_PLAYER **)b;
  return playerA->id - playerB->id;
}
// Function thar creates and return a array of players from the data file
IDX_PLAYER **Load_Binary(FILE *data_file, int num_reg) {
  if (data_file == NULL || num_reg < 0) {
    return NULL;
  }

  // Allocate array of IDX_PLAYER pointers
  IDX_PLAYER **players_array =
      (IDX_PLAYER **)malloc(sizeof(IDX_PLAYER *) * num_reg);
  if (players_array == NULL) {
    // Memory allocation failed
    return NULL;
  }

  int i = 0;
  int player_count = 0;

  // Fill each element of the array
  while (player_count < num_reg) {
    IDX_PLAYER *idx_player = Create_Idx_Player(data_file);
    if (idx_player != NULL) {
      players_array[player_count] = idx_player;
      player_count++;
    } else {
      // Handle the case where Create_Idx_Player fails
      // a player fail does not mean the index should be discarded
      // since a fail simply means a player is either removed or wrongly written
      continue;
    }
    i++;
  }

  // Only sort if we successfully created the expected number of players
  if (player_count == num_reg) {
    qsort(players_array, num_reg, sizeof(IDX_PLAYER *), compareById);
  } else {
    // Clean up partially allocated array
    for (int j = 0; j < player_count; j++) {
      free(players_array[j]);
    }
    free(players_array);
    return NULL;
  }

  return players_array;
}

// Function that writes all of the idx_players
void Write_Players(FILE *data_file, FILE *index_file, int num_reg) {
  // Load_Binary creates a local array of index players
  IDX_PLAYER **players_array = Load_Binary(data_file, num_reg);
  if (players_array == NULL) {
    // Array error
    free(players_array);
    return;
  }
  // here the file index is written, assuring the number of players written
  // is equal to the number of VALID players on the data file
  for (int i = 0; i < num_reg; i++) {
    if (!(fwrite(&players_array[i]->id, sizeof(int), 1, index_file) &&
          fwrite(&players_array[i]->byteoffset, sizeof(long long int), 1,
                 index_file))) {
      // Error to write the array
      return;
    }
  }
}

// Load index to memory
IDX_PLAYER **Load_Index(FILE *index_file, int num_reg) {
  if (index_file == NULL) {
    return NULL;
  }
  // alocating the vector of players, which is the local index
  IDX_PLAYER **local_index =
      (IDX_PLAYER **)malloc((sizeof(IDX_PLAYER *)) * num_reg);
  if (local_index == NULL) {
    // Memory allocation failed
    return NULL;
  }
  // reads header

  fseek(index_file, 0, SEEK_SET);

  IDX_HEADER *idx_header = Read_Idx_Header(index_file);
  if (idx_header == NULL || Get_Idx_Status(idx_header) == '0') {
    free(local_index);
    // Failed to read index header
    return NULL;
  }

  for (int i = 0; i < num_reg; i++) {
    local_index[i] = Read_Idx_Player(index_file);

    if (local_index[i] == NULL) {
      // Failed to read index header
      // frees allocated memory
      for (int j = 0; j < i; j++) {
        free(local_index[j]);
      }
      return NULL;
    }
  }
  return local_index;
}
// Makes a binary search through the index to find the index
int Binary_Search(IDX_PLAYER **arr, int start, int end, int id) {

  if (start <= end) {
    int mid = (start + (end)) / 2;
    if (arr[mid]->id == id) {
      return mid;
    }
    if (arr[mid]->id < id) {
      return Binary_Search(arr, mid + 1, end, id);
    }
    return Binary_Search(arr, start, mid - 1, id);
  }
  return NOT_FOUND;
}
// uses the local index to find the desired id and byteoffset
long long int Search_Index(int id, int num_reg, IDX_PLAYER **local_index) {
  if (local_index == NULL) {
    printf("Error at the local index");
  }

  int index_position = Binary_Search(local_index, 0, num_reg - 1, id);
  if (index_position != NOT_FOUND) {
    return local_index[index_position]->byteoffset;
  }
  return NOT_FOUND;
}
// Print a player on the local index
void Print_Local_Index(IDX_PLAYER **local_index, int num_reg) {
  if (local_index == NULL)
    return;

  for (int i = 0; i < num_reg; i++) {
    printf("\nRegistro número %d\n", i);
    printf("Byteoffset:%lld\n", local_index[i]->byteoffset);
    printf("Id: %d\n", local_index[i]->id);
  }
}
// Removes the register from the index
bool Remove_Index(IDX_PLAYER ***local_index, int id, int num_reg) {
  if (local_index == NULL || *local_index == NULL) {
    return false;
  }
  // finds the position of the removed register
  int index_position = Binary_Search(*local_index, 0, num_reg, id);

  if (index_position == NOT_FOUND) {
    // Error to find the index position
    return false;
  }
  // removed all the registers
  if (num_reg == 1) {
    *local_index = NULL;
    return true;
  }

  // remove the element e rearrange the list
  for (int i = index_position; i < num_reg - 1; i++) {
    (*local_index)[i] = (*local_index)[i + 1];
  }
  // Reduce array size
  IDX_PLAYER **temp =
      realloc(*local_index, sizeof(IDX_PLAYER *) * (num_reg - 1));
  if (temp == NULL) {
    // Error reallocating memory
    return false;
  }
  *local_index = temp;
  return true;
}
// Rewrite index on memory to index file
void Rewrite_Index(char *bin_index, IDX_PLAYER **local_index, char *bin_data,
                   int num_reg) {
  FILE *index_file = fopen(bin_index, "wb");
  if (index_file == NULL) {
    printf("Falha no processamento do arquivo.");
    fclose(index_file);
    return;
  }

  if (index_file == NULL) {
    printf("Falha no processamento do arquivo.");
    fclose(index_file);
    return;
  }
  // create index header
  IDX_HEADER *idx_header = Create_Idx_Header(index_file);

  if (local_index == NULL) {
    free(local_index);
    return;
  }
  // writes all the registers on the index_file
  for (int i = 0; i < num_reg; i++) {
    if (!(fwrite(&local_index[i]->id, sizeof(int), 1, index_file) &&
          fwrite(&local_index[i]->byteoffset, sizeof(long long int), 1,
                 index_file))) {
      // Error to write the array
      return;
    }
  }

  for (int j = 0; j < num_reg; j++) {
    free(local_index[j]);
  }
  free(local_index);

  Set_Idx_Status(idx_header, index_file, '1');

  // close files
  Delete_Idx_header(idx_header);
  fclose(index_file);
}

// inserts the new player into the local index
bool Insert_Local_Index(IDX_PLAYER ***local_index, int new_id,
                        long long int new_byteoffset, int num_reg) {
  if (local_index == NULL || *local_index == NULL) {
    return false;
  }

  // increases array size
  IDX_PLAYER **temp =
      realloc(*local_index, sizeof(IDX_PLAYER *) * (num_reg + 1));
  if (temp == NULL) {
    // Error reallocating memory insert
    return false;
  }
  *local_index = temp;

  IDX_PLAYER *new_player = (IDX_PLAYER *)malloc(sizeof(IDX_PLAYER));
  if (new_player == NULL) {
    // Error reallocating memory insert
    return false;
  }
  // gives the new idx_player its information
  new_player->id = new_id;
  new_player->byteoffset = new_byteoffset;
  // puts it in the index
  (*local_index)[num_reg] = new_player;
  // reorders it
  qsort(*local_index, num_reg + 1, sizeof(IDX_PLAYER *), compareById);

  return true;
}

// reorders the removed linked list, setting the byteoffset of the affected
// registers
void Find_Next(FILE *data_file, HEADER *header, long long int byteoffset,
               int size) {
  // the first register of the linked list in the top
  long long int next = Get_Top(header);
  // its called next because as soon the register size is bigger than the new
  // removed register, it becomes the one that comes after the new removed
  // register
  long long int prior = 0;
  int num_rem = Get_Num_Rem(header);

  // doens't have any removed players yet
  if (next == -1) {
    Set_Local_Top(header, byteoffset);
    return;
  }

  // using to navegate throughout all removed players
  PLAYER *removed_player;
  // searching the position in the linked list
  while (next != (-1)) {
    fseek(data_file, next, SEEK_SET);
    removed_player = Read_Removed_Player(data_file);
    // finds the one that comes after in the linked list
    if (Get_Register_Size(removed_player) > size) {
      //+5 because its the exact position of "next" in the register
      fseek(data_file, byteoffset + 5, SEEK_SET);
      fwrite(&next, sizeof(long long int), 1, data_file);
      if (prior == 0) {
        // if there is no prior, it means it is the first removed register of
        // the linked list
        Set_Local_Top(header, byteoffset);

      } else {
        // a normal case, removed is in the middle of the linked list
        fseek(data_file, prior + 5, SEEK_SET);
        fwrite(&byteoffset, sizeof(long long int), 1, data_file);
      }

      return;
    }
    // goes trough the removed registers
    prior = next;
    next = Get_Next(removed_player);
  }
  // if next == -1, the removed register is the last one on the list
  fseek(data_file, prior + 5, SEEK_SET);
  fwrite(&byteoffset, sizeof(long long int), 1, data_file);
  return;
}
// marks the registers on the data file as removed (removed: 1)
void Remove(FILE *data_file, long long int byteoffset, HEADER *header, int id,
            IDX_PLAYER **local_index) {
  char is_removed = '1';
  long long int next = -1;
  int reg_size = 0;
  // go to the register that will be removed
  fseek(data_file, byteoffset, SEEK_SET);
  fwrite(&is_removed, sizeof(char), 1, data_file);
  fread(&reg_size, sizeof(int), 1, data_file);

  // Insert the removed register on the right position at the removed list
  Find_Next(data_file, header, byteoffset, reg_size);

  fseek(data_file, byteoffset, SEEK_SET);
  PLAYER *skip_removed_player = Read_Removed_Player(data_file);

  // update the local header to fwrite after all removals
  int num_reg = Get_Num_Reg(header);
  // remove at local index
  Remove_Index(&local_index, id, num_reg);

  Set_Local_Reg(header, (num_reg - 1));

  int num_rem = Get_Num_Rem(header);
  Set_Local_Rem(header, (num_rem + 1));
  // printf(" e %d registros removidos\n", Get_Num_Rem(header));
}
// Deletes the player from the data file
void Delete_Line(FILE *data_file, FILE *index_file, char *bin_data,
                 IDX_PLAYER **local_index, int num_reg, HEADER *header) {
  // variable initialization
  int num;
  char *str_name = (char *)malloc(sizeof(char) * 50);
  char *str_nationality = (char *)malloc(sizeof(char) * 50);
  char *str_club = (char *)malloc(sizeof(char) * 50);
  int conditions[5] = {0, 0, 0, 0, 0};
  // read the conditions
  PLAYER *target_player = Create_Target_Player();
  Conditions(conditions, target_player, str_name, str_nationality, str_club);

  // comparing the target player to each player.
  // if the current player corresponds to the conditions we delete it

  // if one of the conditions is by ID, search with binary search on the local
  // index
  if (conditions[ID] == 1) {
    int id_target_player = Get_Id(target_player);
    num_reg = Get_Num_Reg(header);
    long long int byteoffset =
        Search_Index(id_target_player, num_reg, local_index);

    if (byteoffset != NOT_FOUND) {
      fseek(data_file, byteoffset, SEEK_SET);
      PLAYER *current_player = Read_Player(data_file);

      // If the player found by id pass the other conditions
      if (Pass_Conditions(conditions, target_player, current_player)) {

        Remove(data_file, byteoffset, header, id_target_player, local_index);
        return;
      }
    }
    printf("Registro Inexistente\n\n");
    return;
  }
  // long long int save_position;
  while (num_reg != 0) {
    int byteoffset = ftell(data_file);
    // printf("\nbyteoffset: %d\n\n", byteoffset);
    PLAYER *player = Read_Player(data_file);
    // if the player is NULL because a error or it is removed
    if (player == NULL) {
      continue; // skip this player
    }

    // print player if conditions are met
    if (Pass_Conditions(conditions, target_player, player)) {
      int id = Get_Id(player);
      Remove(data_file, byteoffset, header, id, local_index);
    }

    num_reg--;
    // frees the memory
    Delete_Player(player);
  }
  return;
}
