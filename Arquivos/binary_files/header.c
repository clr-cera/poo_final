#include "header.h"

#define ERROR_CHAR '?'
#define ERROR_INT -100000

typedef struct _header HEADER;

struct _header {
  char status;
  long long int top;
  long long int next_byte_offset;
  int num_reg_file;
  int num_rem_file;
};

// Create a header for the binary file and return the struct header
HEADER *Create_Header(FILE *destiny) {
  HEADER *header = (HEADER *)malloc(sizeof(HEADER));
  if (header == NULL)
    return header;
  // registers if the file is complete/corrupt or not
  header->status = '0';
  // registers byte offset of an logic removed register
  header->top = -1;
  // tells the next byte offset available
  header->next_byte_offset = 0;
  // counter for how many registers are not removed
  header->num_reg_file = 0;
  // counter for how many registers are removed
  header->num_rem_file = 0;

  /* if anything goes wrong, fwrite will return a number different from 1, so it
   * returns NULL */
  if (!(fwrite(&header->status, sizeof(char), 1, destiny) &&
        fwrite(&header->top, sizeof(long long int), 1, destiny) &&
        fwrite(&header->next_byte_offset, sizeof(long long int), 1, destiny) &&
        fwrite(&header->num_reg_file, sizeof(int), 1, destiny) &&
        fwrite(&header->num_rem_file, sizeof(int), 1, destiny))) {
    return NULL;
  }
  return header;
}
// writes the local header into the file
bool Update_Header(HEADER *h, FILE *f, char new_status) {
  if (h == NULL) {
    return false;
  }
  h->status = new_status;

  fseek(f, 0, SEEK_SET);

  if (!(fwrite(&h->status, sizeof(char), 1, f) &&
        fwrite(&h->top, sizeof(long long int), 1, f) &&
        fwrite(&h->next_byte_offset, sizeof(long long int), 1, f) &&
        fwrite(&h->num_reg_file, sizeof(int), 1, f) &&
        fwrite(&h->num_rem_file, sizeof(int), 1, f))) {
    return false;
  }
  return true;
}

// Read the header of a binary file and return the struct header
HEADER *Read_Header(FILE *file) {
  HEADER *header = (HEADER *)malloc(sizeof(HEADER));
  if (header == NULL)
    return NULL;

  fread(&header->status, sizeof(char), 1, file);
  fread(&header->top, sizeof(long long int), 1, file);
  fread(&header->next_byte_offset, sizeof(long long int), 1, file);
  fread(&header->num_reg_file, sizeof(int), 1, file);
  fread(&header->num_rem_file, sizeof(int), 1, file);

  return header;
}
char Get_Status(HEADER *header) {
  if (header == NULL) {
    return ERROR_CHAR; // error
  }
  return header->status;
}
int Get_Num_Reg(HEADER *header) {
  if (header == NULL) {
    return ERROR_INT;
  }
  return header->num_reg_file;
}

int Get_Num_Rem(HEADER *header) {
  if (header == NULL) {
    return ERROR_INT;
  }
  return header->num_rem_file;
}

long long int Get_Top(HEADER *header) {
  if (header == NULL) {
    return ERROR_INT;
  }
  return header->top;
}

long long int Get_Next_Byteoffset(HEADER *header) {
  if (header == NULL) {
    return ERROR_INT;
  }
  return header->next_byte_offset;
}

bool Set_Local_Rem(HEADER *header, int new_rem_file) {
  if (header == NULL) {
    return false;
  }
  header->num_rem_file = new_rem_file;
  return true;
}
bool Set_Local_Reg(HEADER *header, int new_reg_file) {
  if (header == NULL) {
    return false;
  }
  header->num_reg_file = new_reg_file;
  return true;
}
bool Set_Local_Top(HEADER *header, long long int new_top) {
  if (header == NULL) {
    return false;
  }
  header->top = new_top;
  return true;
}
bool Set_Local_NextByteOffset(HEADER *header,
                              long long int new_nextbyteoffset) {
  if (header == NULL) {
    return false;
  }
  header->next_byte_offset = new_nextbyteoffset;
  return true;
}

// Deallocating the struct header
void Delete_header(HEADER *header) {
  free(header);
  header = NULL;
}