#include "tools.h"
#include <ctype.h>
#include <stdio.h>

/* read and return a string that is separated with separators from any file,
 * including stdin and csv */
char *Read_From_File(FILE *file, const char *separators, int as_string,
                     int *string_len) {
  int capacity = 10;                           // initial buffer size
  char *str = malloc(capacity * sizeof(char)); // initially allocate memory
  if (str == NULL) {
    perror("Memory allocation failed");
    return NULL;
  }
  int i = 0;
  char c;

  do {
    c = fgetc(file); // read character from file
    if (c == EOF || strchr(separators, c) != NULL) {
      // Skip \r\n sequences at the end of a line
      if (c == '\r') {
        c = fgetc(file);
        if (c != '\n') {
          ungetc(c, file);
        }
      }
      break;
    }
    str[i++] = c;
    // expand buffer capacity if necessary
    if (i >= capacity) {
      capacity *= 2;
      char *temp = realloc(str, capacity * sizeof(char));
      if (temp == NULL) {
        free(str); // free previously allocated memory
        perror("Memory reallocation failed");
        return NULL;
      }
      str = temp; // update pointer to new memory block
    }
  } while (1);

  // Check for empty string
  if (i == 0) {
    free(str);
    *string_len = 0;
    return NULL;
  }

  // select 1 if the string will end with /0, else do not end with /0
  if (as_string) {
    str = realloc(str, (i + 1) * sizeof(char)); // resize to exact size
    if (str == NULL) {
      *string_len = 0;
      return NULL;
    }
    str[i] = '\0';
  } else {
    str = realloc(str, i * sizeof(char));
    if (str == NULL) {
      *string_len = 0;
      return NULL;
    }
  }
  *string_len = i;
  return str;
}

// consume csv header
void Consume_Line(FILE *csv, int columns) {
  char s[3] = {',', '\n', '\r'};
  int nothing;
  for (int i = 0; i < columns; i++) {
    Read_From_File(csv, s, 0, &nothing);
  }
}
// function with the express purpose of reading the age inserted by the user,
// considereing the fact that it can be written as NULO
int Read_Age_Insert_Player() {
  char c = getchar();

  char str[4];
  int i = 0;
  while (c != ' ') {
    // if the first char is "N", it can not be a valid number
    if (c == 'N' || c == 'n') {
      c = getchar();
      c = getchar();
      c = getchar();
      return -1;
    } else {
      // the numbers are put into  a string;
      str[i] = c;
      c = getchar();
      i++;
    }
  }

  // converts the string into an int
  return atoi(str);
}