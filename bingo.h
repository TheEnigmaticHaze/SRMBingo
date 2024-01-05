#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "./inputManager.h"


int randInt(int lower, int upper)
{
  return rand() % (upper - lower) + lower;
}

void shuffle(int *toShuffle, int count)
{
  for(int i = 0; i < count; i++)
  {
    int chosenIndex = randInt(i, count);

    int temp = toShuffle[i];
    toShuffle[i] = toShuffle[chosenIndex];
    toShuffle[chosenIndex] = temp;
  }
}

typedef struct
{
  unsigned int promptCompletedFlags;
  char *prompts;
  int *promptIndeces;
} Board;

const int BOARD_WIDTH  = 5;
const int BOARD_HEIGHT = 5;

Board boardCreate(const char *promptsFilePath, int promptCount)
{
  Board newBoard;
  newBoard.promptCompletedFlags = 0;

  if(promptCount < 25)
  {
    newBoard.prompts = NULL;
    return newBoard;
  }

  FILE *promptsFile = fopen(promptsFilePath, "r");

  if(promptsFile == NULL)
  {
    newBoard.prompts = NULL;
    return newBoard;
  }

  fseek(promptsFile, 0, SEEK_END);
  const int fileSize = ftell(promptsFile) + 1;
  rewind(promptsFile);

  newBoard.prompts = (char *)malloc(sizeof(char) * fileSize);
  fread(newBoard.prompts, sizeof(char), fileSize, promptsFile);

  newBoard.promptIndeces = (int *)malloc(sizeof(int) * promptCount);
  int currentIndex = 0;

  (newBoard.promptIndeces)[currentIndex] = 0;
  currentIndex++;

  for(int i = 0; i < fileSize; i++)
  {
    if((newBoard.prompts)[i] == '\n')
    {
      (newBoard.prompts)[i] = '\0';
      (newBoard.promptIndeces)[currentIndex] = i + 1;
      currentIndex++;
    }
  }

  shuffle(newBoard.promptIndeces, promptCount);

  return newBoard;
}

void boardFree(Board board)
{
  free(board.promptIndeces);
  free(board.prompts);
}

typedef struct
{
  int row;
  int column;
} Tile;

Tile parseTileName(char *str)
{
  Tile newTile;

  if('1' <= str[1] && str[1] <= '5')
  {
    newTile.row = str[1] - '1';
  }
  else
  {
    newTile.row = -1;
  }

  int column = -1;

  switch(str[0])
  {
    case 'b':
      column = 0;
      break;
    case 'i':
      column = 1;
      break;
    case 'n':
      column = 2;
      break;
    case 'g':
      column = 3;
      break;
    case 'o':
      column = 4;
      break;
  }

  newTile.column = column;

  return newTile;
}

void markDone(Board *board, int column, int row)
{
  board->promptCompletedFlags |= 1 << (column + row * 5);
}

void markUndone(Board *board, int column, int row)
{
  board->promptCompletedFlags &= !(1 << (column + row * 5));
}

void printRow(Board board, int rowNumber)
{
  char row[27];
  strcpy(row, "|   |   |   |   |   |   |\n");
  row[2] = '1' + rowNumber;

  for(int i = 0; i < 5; i++)
  {
    if(board.promptCompletedFlags & (1 << (rowNumber * 5 + i)))
    {
      row[6 + 4 * i] = 'X';
    }
    else
    {
      row[6 + 4 * i] = '-';
    }
  }

  printf(row);
}

void printBoard(Board board)
{
  printf("+---+---+---+---+---+---+\n");
  printf("|   | B | I | N | G | O |\n");
  for(int i = 0; i < 5; i++)
  {
    printf("+---+---+---+---+---+---+\n");
    printRow(board, i);
  }
  printf("+---+---+---+---+---+---+\n");
}

int checkWin(Board board)
{
  const int diag1  = 0b00000001000001000001000001000001;
  const int diag2  = 0b00000000000010001000100010001000;
  const int row    = 0b00000000000000000000000000011111;
  const int column = 0b00000000000100001000010000100001;

  if((board.promptCompletedFlags & diag1) == diag1){return 1;}
  if((board.promptCompletedFlags & diag2) == diag2){return 1;}

  if((board.promptCompletedFlags & (row << 0))  == row << 0  ){return 1;}
  if((board.promptCompletedFlags & (row << 5))  == row << 5  ){return 1;}
  if((board.promptCompletedFlags & (row << 10)) == row << 10 ){return 1;}
  if((board.promptCompletedFlags & (row << 15)) == row << 15 ){return 1;}
  if((board.promptCompletedFlags & (row << 20)) == row << 20 ){return 1;}

  if((board.promptCompletedFlags & (column << 0)) == column << 0){return 1;}
  if((board.promptCompletedFlags & (column << 1)) == column << 1){return 1;}
  if((board.promptCompletedFlags & (column << 2)) == column << 2){return 1;}
  if((board.promptCompletedFlags & (column << 3)) == column << 3){return 1;}
  if((board.promptCompletedFlags & (column << 4)) == column << 4){return 1;}

  return 0;
}

void playBingo(Board board, TrieNode *root)
{
  printBoard(board);

  char buffer[BUFFER_SIZE];

  while(1)
  {
    fgets(buffer, BUFFER_SIZE, stdin);

    char *end;
    int command = grabCommandBingo(buffer, root, &end);

    if(command == COMMAND_BINGO_QUIT)
    {
      printf("quitting\n");
      return;
    }

    Tile tile;

    switch(command)
    {
      case COMMAND_BINGO_NONE:
        printf("not a valid command...\n");
        break;
      case COMMAND_BINGO_VIEW:
        tile = parseTileName(end + 1);
        if(tile.column != -1 && tile.row != -1)
        {
          printf(board.prompts + (board.promptIndeces)[tile.column + tile.row * 5]);
          printf("\n");
        }
        else
        {
          printf("that is not a valid tile name\n");
        }
        break;
      case COMMAND_BINGO_CHECK:
        tile = parseTileName(end + 1);
        if(tile.column != -1 && tile.row != -1)
        {
          markDone(&board, tile.column, tile.row);
        }
        else
        {
          printf("that is not a valid tile name\n");
        }
        break;
      case COMMAND_BINGO_UNCHECK:
        tile = parseTileName(end + 1);
        if(tile.column != -1 && tile.row != -1)
        {
          markUndone(&board, tile.column, tile.row);
        }
        else
        {
          printf("that is not a valid tile name\n");
        }
        break;
      case COMMAND_BINGO_BOARD:
        printBoard(board);
        break;
      case COMMAND_BINGO_QUIT:
        printf("quitting\n");
        return;
        break;
      default:
        break;
    }

    if(checkWin(board))
    {
      printf("you win!\n");
      return;
    }
  }
}