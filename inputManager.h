#include <stdio.h>
#include <stdlib.h>

const int BUFFER_SIZE = 1024;

#define MAX_COMMAND_LENGTH 6
#define MAX_BINGO_COMMAND_LENGTH 7

const char commands[6][MAX_COMMAND_LENGTH + 1] = 
{
  "help",
  "add",
  "remove",
  "play",
  "show",
  "quit"
};

typedef enum
{
  COMMAND_NONE,
  COMMAND_HELP,
  COMMAND_ADD,
  COMMAND_REMOVE,
  COMMAND_PLAY,
  COMMAND_SHOW,
  COMMAND_QUIT,
  COMMAND_COUNT,
} Command;

const char bingoCommands[6][MAX_BINGO_COMMAND_LENGTH + 1] = 
{
  "view",
  "check",
  "uncheck",
  "board",
  "quit"
};

typedef enum
{
  COMMAND_BINGO_NONE,
  COMMAND_BINGO_VIEW,
  COMMAND_BINGO_CHECK,
  COMMAND_BINGO_UNCHECK,
  COMMAND_BINGO_BOARD,
  COMMAND_BINGO_QUIT,
  COMMAND_BINGO_COUNT
} CommandBingo;

typedef struct TrieNode
{
  Command type;
  CommandBingo bingoType;
  struct TrieNode *next[26];
} TrieNode;

TrieNode *trieNodeCreate(Command type, CommandBingo bingoType)
{
  TrieNode *newNode = (TrieNode *)malloc(sizeof(TrieNode));
  newNode->type = type;
  newNode->bingoType = bingoType;

  for(int i = 0; i < 26; i++)
  {
    (newNode->next)[i] = NULL;
  }

  return newNode;
}

void trieAddWord(const char *str, Command command, CommandBingo bingoCommand, TrieNode *root)
{
  if(*str == '\0')
  {
    if(command != COMMAND_NONE)
    {
      root->type = command;
    }
    if(bingoCommand != COMMAND_BINGO_NONE)
    {
      root->bingoType = bingoCommand;
    }
    return;
  }

  if((root->next)[*str - 'a'] == NULL)
  {
    (root->next)[*str - 'a'] = trieNodeCreate(COMMAND_NONE, COMMAND_BINGO_NONE);
  }

  trieAddWord(str + 1, command, bingoCommand, (root->next)[*str - 'a']);
}

TrieNode *buildTrie()
{
  TrieNode *root = trieNodeCreate(COMMAND_NONE, COMMAND_BINGO_NONE);

  for(int i = 0; i < (int)COMMAND_COUNT - 1; i++)
  {
    trieAddWord(commands[i], (Command)(i + 1), COMMAND_BINGO_NONE, root);
  }

  for(int i = 0; i < (int)COMMAND_BINGO_COUNT - 1; i++)
  {
    trieAddWord(bingoCommands[i], COMMAND_NONE, (CommandBingo)(i + 1), root);
  }

  return root;
}

const char commandHelp[6][128] = 
{
  "\'help\': help help (help)",
  "\'add\': add new prompt to the bingo list (add <prompt>)",
  "\'remove\': remove prompt from the bingo list (remove <prompt number>)",
  "\'play\': play bingo (play)",
  "\'show\': show all prompts (show)",
  "\'quit\': quit (quit)"
};

const char commandBingoHelp[5][128] = 
{
  "\'view\': views the prompt located at a tile (view <tile>)",
  "\'check\': checks off a tile that you completed (check <tile>)",
  "\'uncheck\': unchecks a tile (uncheck <tile>)",
  "\'board\': shows the bingo board (board)",
  "\'quit\': quit (quit)"
};

Command grabCommand(char *string, TrieNode *root, char **end)
{
  if(root == NULL)
  {
    return COMMAND_NONE;
  }
  if('a' > *string || *string > 'z')
  {
    *end = string;
    return root->type;
  }
  return grabCommand(string + 1, (root->next)[*string - 'a'], end);
}

CommandBingo grabCommandBingo(char *string, TrieNode *root, char **end)
{
  if(root == NULL)
  {
    return COMMAND_BINGO_NONE;
  }
  if('a' > *string || *string > 'z')
  {
    *end = string;
    return root->bingoType;
  }
  return grabCommandBingo(string + 1, (root->next)[*string - 'a'], end);
}