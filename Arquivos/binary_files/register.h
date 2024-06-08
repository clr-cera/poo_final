#include "../utilities/funcoes_fornecidas.h"
#include "../utilities/tools.h"
#include "header.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _player PLAYER;

// defines to make the conditions vector clearer
#define ID 0
#define AGE 1
#define NAME_PLAYER 2
#define NATIONALITY 3
#define CLUB 4

// Player Functions
bool Create_Player(FILE *destiny, FILE *csv);
void Print_Player(PLAYER *player);
PLAYER *Read_Player(FILE *file);
void Delete_Player(PLAYER *player);
PLAYER *Read_Removed_Player(FILE *file);
int Pass_Conditions(int conditions[], PLAYER *target_player, PLAYER *player);
void Conditions(int *conditions, PLAYER *target_player, char *str_name,
                char *str_nationality, char *str_club);

// Inserted Player Funcitons
PLAYER *Read_Inserted_Player();
long long int Best_Fit(FILE *data_file, PLAYER *inserted_player,
                       HEADER *header);

// Target Player Functions
PLAYER *Create_Target_Player();
bool Set_Id_Target_Player(int id, PLAYER *target_player);
bool Set_Age_Target_Player(int age, PLAYER *target_player);
bool Set_Name_Target_Player(char *name, PLAYER *target_player);
bool Set_Nationality_Target_Player(char *nationality, PLAYER *target_player);
bool Set_Club_Target_Player(char *club, PLAYER *target_player);

// Get functions
char Get_Removed(PLAYER *p);
int Get_Id(PLAYER *p);
int Get_Age(PLAYER *p);
char *Get_Name(PLAYER *p);
char *Get_Nationality(PLAYER *p);
char *Get_Club(PLAYER *p);
long long int Get_Next(PLAYER *p);
int Get_Register_Size(PLAYER *p);
