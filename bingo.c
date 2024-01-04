#include <string.h>
#include "./bingo.h"


const char PROMPTS_FILE_PATH[] = "prompts.txt";

int promptCount = 0;

void countPrompts()
{
  FILE *promptsFile = fopen(PROMPTS_FILE_PATH, "r");

  if(fgetc(promptsFile) == EOF){return;}
  promptCount++;

  while(1)
  {
    char currentChar = fgetc(promptsFile);
    if(currentChar == EOF){break;}
    if(currentChar == '\n'){promptCount++;}
  }
}

void printHelp()
{
  printf("----- STANDARD HELP ------\n\n");
  for(int i = 0; i < COMMAND_COUNT - 1; i++)
  {
    printf(commandHelp[i]);
    printf("\n");
  }

  printf("\n----- BINGO HELP ------\n\n");
  printf("tiles are formatted by the letter (lowercase), then the number (b1, n4, o2)\n\n");

  for(int i = 0; i < COMMAND_BINGO_COUNT - 1; i++)
  {
    printf(commandBingoHelp[i]);
    printf("\n");
  }
}

void printFileContents()
{
  FILE *prompts = fopen(PROMPTS_FILE_PATH, "r");
  char c;
  while(c != EOF)
  {
    c = fgetc(prompts);
    putc(c, stdout);
  }
  printf("\n");
  fclose(prompts);
}

void addNewPrompt(char *newPrompt)
{
  FILE *prompts = fopen(PROMPTS_FILE_PATH, "a");
  char *c = newPrompt + 1;
  fputs(c, prompts);
  fclose(prompts);
  printf("prompt has been added to the prompts list!\n");
}

void removePrompt(unsigned int index)
{
  if(index == 0)
  {
    return;
  }

  const char* TEMP_FILE_PATH = tmpnam(NULL);
  char tempPath[strlen(TEMP_FILE_PATH) + 3];
  strcpy(tempPath, TEMP_FILE_PATH);
  strcpy(tempPath + strlen(TEMP_FILE_PATH), "txt");
  
  FILE *tempPrompts = fopen(tempPath + 1, "w");
  if(!tempPrompts)
  {
    printf("could not open tempPrompts at %s\n", tempPath);
    return;
  }
  FILE *prompts = fopen(PROMPTS_FILE_PATH, "r");
  int newLines = 1;

  char currentChar = 0;

  while(currentChar != EOF)
  {
    currentChar = fgetc(prompts);
    if(currentChar == '\n')
    {
      newLines++;
    }
    if(newLines == index)
    {
      continue;
    }
    fputc(currentChar, tempPrompts);
  }

  fclose(prompts);
  fclose(tempPrompts);

  tempPrompts = fopen(tempPath + 1, "r");
  prompts = fopen(PROMPTS_FILE_PATH, "w");

  if(!tempPrompts)
  {
    printf("could not open tempPrompts at %s\n", tempPath);
    return;
  }

  currentChar = 0;
  while(currentChar != EOF)
  {
    currentChar = fgetc(tempPrompts);
    fputc(currentChar, prompts);
  }

  fclose(prompts);
  fclose(tempPrompts);

  remove(tempPath + 1);
}

int main()
{
  printf("Hello, Welcome to My Stress-Resiliency & Mindfulness Project!\n");
  printf("type in \'help\' for help\n\n");

  srand(time(NULL));

  countPrompts();
  
  char buffer[BUFFER_SIZE];
  TrieNode *root = buildTrie();

  while(1)
  {
    fgets(buffer, BUFFER_SIZE, stdin);

    char *end;
    int command = grabCommand(buffer, root, &end);

    if(command == COMMAND_QUIT)
    {
      break;
    }

    switch(command)
    {
      case COMMAND_NONE:
        break;
      case COMMAND_HELP:
        printHelp();
        break;
      case COMMAND_ADD:
        addNewPrompt(end);
        break;
      case COMMAND_REMOVE:
        end++;
        removePrompt(strtol(end, NULL, 10)); 
        break;
      case COMMAND_PLAY:
      {
        Board board = boardCreate(PROMPTS_FILE_PATH, promptCount);
        playBingo(board, root);
        printf("type anything to quit\n");
        fgets(buffer, 1, stdin);
        return 0;
      }
      case COMMAND_SHOW:
        printFileContents();
        break;
      case COMMAND_QUIT:
        break;
      default:
        break;
    }
  }
  return 0;
}