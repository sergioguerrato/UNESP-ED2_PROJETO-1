/*** Sérgio Paulo Guerrato Alves - BCC UNESP - Rio Claro ***/
/*** Project 1: Files - Compression and Dinamic Recovery ***/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**Functions**/
void f_insert(FILE *);
void f_remove(FILE *);
void f_compress(void);

/*=*=*= MAIN =*=*=*/
int main() {
  bool menu = 1;
  int option;
  FILE *insert, *remove;

  /*** Load Files ***/
  /*system("clear"); // for Linux */
  system("cls"); // for windows
  printf("\n\n\n    (insere.bin)\n    (remove.bin)\n\n    Loading Files...");

  if ((insert = fopen("insere.bin", "rb")) == NULL) {
    fprintf(stderr, "Insert file, opening error.\n\n");
    exit(-1);
  }

  if ((remove = fopen("remove.bin", "rb")) == NULL) {
    fprintf(stderr, "Remove file, opening error.\n\n");
    exit(-2);
  }
  getchar();
  /*** End Loading ***/

  /**Main Menu**/
  while (menu) {
    /*system("clear"); // for Linux */
    system("cls"); // for windows
    printf("\n\n     .=================== MENU ===================.\n");
    printf("     |                                            |\n");
    printf("     |    1. Insert                               |\n");
    printf("     |    2. Remove                               |\n");
    printf("     |    3. Compress                             |\n");
    printf("     |    4. Exit                                 |\n");
    printf("     |                                            |\n");
    printf("     |    Choose and option...                    |\n");
    printf("     |                                            |\n");
    printf("     '============================================'\n");
    printf("\n\n     Option: ");
    scanf("%d", &option);
    fflush(stdin);

    switch (option) {
    case 1:
      f_insert(insert);
      break;
    case 2:
      f_remove(remove);
      break;
    case 3:
      f_compress();
      break;
    case 4:
      menu = 0;
      break;
    }
  }
  fclose(insert);
  fclose(remove);
  /*system("clear"); // for Linux */
  system("cls"); // for windows
  return 0;
}
/*=*=*= END MAIN =*=*=*/

/*=*=*= INSERT =*=*=*/
void f_insert(FILE *insert) {
  /*system("clear"); // for Linux */
    system("cls"); // for windows

  /** Open data.bin to update, if don't exist, create **/
  FILE *data;
  if ((data = fopen("data.bin", "r+b")) == NULL) {
    if ((data = fopen("data.bin", "w+b")) == NULL) {
      fprintf(stderr, "Data file, opening error.\n\n");
      exit(-3);
    }
  }

  /** Registers Struct **/
  typedef struct vehicle {
    char len;
    char cod_cli[12];
    char cod_vei[8];
    char client[50];
    char veiculo[50];
    char dias[4];
  };

  /** Search for number of regs (n)**/
  fseek(insert, 0, SEEK_SET);
  int n = 0;
  char c[124];
  while (!feof(insert)) {
    fread(&c, sizeof(char), 124, insert);
    if (!feof(insert))
      n++;
  }
  fseek(insert, 0, SEEK_SET);
  /** found n value! **/

  /** Store all data of insere.bin in a struct buffer**/
  int i = 0;
  struct vehicle reg[n];
  while (i < n) {

    fread(reg[i].cod_cli, sizeof(char) * 12, 1, insert);
    fread(reg[i].cod_vei, sizeof(char) * 8, 1, insert);
    fread(reg[i].client, sizeof(char) * 50, 1, insert);
    fread(reg[i].veiculo, sizeof(char) * 50, 1, insert);
    fread(reg[i].dias, sizeof(char) * 4, 1, insert);

    printf("\n\n%s,%s,%s,%s,%s\n\n", reg[i].cod_cli, reg[i].cod_vei,
           reg[i].client, reg[i].veiculo, reg[i].dias);
    i++;
  }
  getchar();

  fseek(insert, 0, SEEK_SET);
  bool menu = 1;
  int num;
  int len; /* length */

  while (menu) {
    /*system("clear"); // for Linux */
    system("cls"); // for windows
    printf("\n\n ===== INSERT =====\n");
    printf("\n\n  Which register must be included?");
    printf("\n  (Type 0 to exit)");
    printf("\n  Number: ");
    scanf("%d", &num);
    getchar();
    if (num == 0)
      menu = 0;

    else if (num <= n) {

      /** Stores the length of the register on 'len' **/
      len = strlen(reg[num - 1].cod_cli) + strlen(reg[num - 1].cod_vei) +
            strlen(reg[num - 1].client) + strlen(reg[num - 1].veiculo) +
            strlen(reg[num - 1].dias);
      len = len + 6;

      /** Search for a empty space **/
      fseek(data, 0, SEEK_END);
      int position = ftell(data);
      getchar();
      int header;

      /*** IF FILE IS EMPTY ***/
      if (position == 0) {
        fseek(data, 4, SEEK_SET); /** Skip Header **/
        position = ftell(data);
      }
      /*** IF FILE IS NOT EMPTY ***/
      else {
        fseek(data, 0, SEEK_SET);
        fread(&header, sizeof(int) * 1, 1, data); /** Looks for header **/

        /*** IF HEADER IS 0, MEANS NO ONE HAS BEEN REMOVED YET ***/
        if (header == 0) {
          fseek(data, 0, SEEK_END); /** Write on the end **/
        }
        /*** IF HEADER IS DIFF. OF 0, MEANS THAT REMOTION HAPPENED ***/
        else {
          fseek(data, header, SEEK_SET); /** Skip to the position of removed reg
                                            to check size available **/
          position = ftell(
              data); /** Store the 'Byte of set' -> similar to lookahead **/
          char lenflag;
          fread(&lenflag, sizeof(char) * 1, 1,
                data); /** Check if size is enough **/
          lenflag = lenflag - '0';
          printf("\n\nlenflag: %d, len: %d\n", lenflag, len);
          getchar();

          /*** IF THE SIZE AVAILABLE IS NOT ENOUGHT ***/
          if (lenflag < len)
            fseek(data, 0, SEEK_END); /** Write on the end **/
          /*** IF THE SIZE AVAILABLE IS ENOUGHT ***/
          else {
            fseek(data, 0, SEEK_SET);
            header = 0;
            fwrite(&header, sizeof(int) * 1, 1, data);
            fseek(data, position, SEEK_SET); /** Returns to the 'Byte of Set'
                                                that stores the length **/
          }
        }
      }

      /*** WRITE ***/
      num--;

      char c_ = len + '0';
      fwrite(&c_, 1, sizeof(char), data);

      len = strlen(reg[num].cod_cli);
      fwrite(reg[num].cod_cli, len, sizeof(char), data);
      c_ = '|';
      fwrite(&c_, 1, sizeof(char), data);

      len = strlen(reg[num].cod_vei);
      fwrite(reg[num].cod_vei, len, sizeof(char), data);
      fwrite(&c_, 1, sizeof(char), data);

      len = strlen(reg[num].client);
      fwrite(reg[num].client, len, sizeof(char), data);
      fwrite(&c_, 1, sizeof(char), data);

      len = strlen(reg[num].veiculo);
      fwrite(reg[num].veiculo, len, sizeof(char), data);
      fwrite(&c_, 1, sizeof(char), data);

      len = strlen(reg[num].dias);
      fwrite(reg[num].dias, len, sizeof(char), data);
      fwrite(&c_, 1, sizeof(char), data);

      printf("\n\n Data saved!\n");
      getchar();
    }
  }

  fclose(data);
}

/*=*=*= REMOVE =*=*=*/
void f_remove(FILE *remove) {
  /*system("clear"); // for Linux */
    system("cls"); // for windows

  FILE *data;
  if ((data = fopen("data.bin", "r+b")) == NULL) {
    fprintf(stderr, "Data file, opening error.\n\n");
    exit(-3);
  }

  typedef struct vehicle {
    char len;
    char cod_cli[12];
    char cod_vei[8];
    char client[50];
    char veiculo[50];
    char dias[4];
  };

  /** Search for number of regs (n)**/
  fseek(remove, 0, SEEK_SET);
  int n = 0;
  char c[20];
  while (!feof(remove)) {
    fread(&c, sizeof(char), 20, remove);
    if (!feof(remove))
      n++; /* counting */
  }
  fseek(remove, 0, SEEK_SET);
  /** found n value! **/
  getchar();

  struct vehicle reg_r[n];
  struct vehicle reg_d;
  int i = 0;
  /*system("clear"); // for Linux */
    system("cls"); // for windows
  printf("\n\n Loading data... \n");
  getchar();
  /*system("clear"); // for Linux */
    system("cls"); // for windows
  while (i < n) {

    fread(reg_r[i].cod_cli, 12 * sizeof(char), 1, remove);
    fread(reg_r[i].cod_vei, 8 * sizeof(char), 1, remove);

    printf("\n\nClient: %s, Vehicle: %s", reg_r[i].cod_cli, reg_r[i].cod_vei);
    i++;
  }
  fseek(remove, 0, SEEK_SET);
  getchar();
  bool menu = 1, search = 1;
  int num;
  int len; /* length */
  int flag = 0;

  /*system("clear"); // for Linux */
  system("cls"); // for windows
  printf("\n\n ===== REMOVE =====\n");
  printf("\n\n  Which register must be removed?");
  printf("\n  (Type 0 to exit)");
  printf("\n  Number: ");
  fflush(stdin);
  scanf("%d", &num);
  getchar();
  if (num == 0)
    menu = 0;

  num--;
  
  if (num < n) {
    /* Statements */
    int header, bos, aux, aux2;
    char ast = '*', pipe;
    
    fseek(data, 4, SEEK_SET); /** Skip Header **/

    /*** SEARCH FOR SELECTED REG ON DATA FILE ***/
    printf("\n\nSearching...\n");
    while (search) {
      /*system("clear"); // for Linux */
      system("cls"); // for windows

      bos = ftell(data); /** Store atual location **/
      fread(&reg_d.len, sizeof(char) * 1, 1, data); /** Read the first byte (length) **/
      aux = ftell(data); /** Store next byte **/
      aux2 = reg_d.len - '0'; /** Convert char to int **/
      fread(reg_d.cod_cli, 11 * sizeof(char), 1, data); /** Read Client Code **/
      pipe = fgetc(data); /** Skip pipe **/
      fread(reg_d.cod_vei, 7 * sizeof(char), 1, data); /** Read Vehicle Code **/
      reg_d.cod_cli[11] = '\0';
      reg_d.cod_vei[7] = '\0';
      getchar();

      /*** COMPARE IF THE REG. IS SAME OF SELECTED BY USER ***/
      if ((strcmp(reg_d.cod_cli, reg_r[num].cod_cli) == 0) &&
          (strcmp(reg_d.cod_vei, reg_r[num].cod_vei) == 0)) {
        printf("\n\nRemoving... \n");
        getchar();

	/** SET ON HEADER THE BYTE OF SET **/
        header = bos;
        fseek(data, 0, SEEK_SET);
        fwrite(&header, sizeof(int), 1, data);

	/*** RETURN TO LOCATION OF REMOVED REG. IN DATA FILE AND SET THE MARK ***/
        fseek(data, bos + 1, SEEK_SET);
        fputc(ast, data); /* mark */
        search = 0; /** Stop Search **/

      } 
        /*** IF REG. IS DIFF. GO NEXT ***/
        else {
        bos = bos + (reg_d.len - '0'); /** Skip to next byte of set **/
        getchar();
        fseek(data, bos, SEEK_SET);
        flag++;
        if (flag > n) {
          search = 0; /** Stop Searching **/
          /*system("clear"); // for Linux */
          system("cls"); // for windows
          printf("\n\n Sorry, but selected registration was not found. Try again! \n");
          getchar();
        }
        if (feof(data))
          search = 0; /** Stop Searching **/
      }
    }
  }
}

void f_compress() {
  /*system("clear"); // for Linux */
  system("cls"); // for windows
  printf("\n\n Compressing Data...");

  FILE *data, *data_c;
  if ((data = fopen("data.bin", "rb")) == NULL) {
    fprintf(stderr, "Data file, opening error.\n\n");
    exit(-3);
  }

  if ((data_c = fopen("data_compressed.bin", "w+b")) == NULL) {
    fprintf(stderr, "Data file, opening error.\n\n");
    exit(-3);
  }

  /** Search for number of regs  (n)***/
  fseek(data, 4, SEEK_SET);
  int n = 0;
  char pipe;
  while (!feof(data)) {
    pipe = fgetc(data);
    if (pipe == '|')
      n++; /* Counting */
  }
  fseek(data, 0, SEEK_SET);
  n = n / 6;
  /** Found n value! **/
  getchar();

  fseek(data, 4, SEEK_SET); /** Skip header in data file **/
  fseek(data_c, 4, SEEK_SET); /** Skip header in compressed file **/

  int i = 0, aux, len;
  char c,  buffer[200];
  while (i <= n) {
    aux = ftell(data); /** Store atual location **/
    fread(&c, 1 * sizeof(char), 1, data); /** Read length **/
    len = c - '0'; /** Convert to int **/
    fseek(data, aux, SEEK_SET); /** Retun to previus byte of set **/
    fread(buffer, sizeof(char) * len, 1, data); /** Read the registration on a buffer **/
    if (buffer[1] != '*') /** Check the mark exists **/
      fwrite(buffer, len, sizeof(char), data_c); /** If don't write on compressed file **/
    else { /** If mark exists, skip reg **/
      fseek(data, aux + len, SEEK_SET); /** Skip reg **/
    }
    i++;
  }
  getchar();
  fclose(data);
  fclose(data_c);
}
