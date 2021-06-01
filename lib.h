#ifndef LIB_H_INCLUDED
#define LIB_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_WORD_LEN 50
#define MAX_SENT_LEN 255
#define DATE_LEN 11 // +1 char for \0 terminator

typedef enum { BG, EN, DE } Language;

typedef struct {
    char translation[MAX_WORD_LEN];
    char type;
    char sentence[MAX_SENT_LEN];
} TranslationEntry;

typedef struct TransNode {
    TranslationEntry value;
    struct TransNode *next;
}  TranslationNode;

typedef struct {
    char word[MAX_WORD_LEN];
    TranslationNode *translationsInLang1;
    TranslationNode *translationsInLang2;
    char addedOn[DATE_LEN];
    unsigned rating; // 0-10
} WordEntry;

typedef struct WNode {
    WordEntry value;
    struct WNode *next;
} WordNode;

typedef struct {
    Language chosenLang;
    Language lang1;
    Language lang2;
} Context;

// ### Writing ###

// String is prefixed by its length
void writeStringToBin(char *str, FILE *fr);
void writeTranslationsToBin(TranslationNode *head, FILE *fw);
void writeDictionaryToBin(WordNode *head, char *filename, char *writeMode);
void writeTestDataToBin();

// ### Reading ###

// String is prefixed by its length
char *readStringFromBin(FILE *fr);
TranslationNode* readTranslationsFromBin(FILE *fr);
char *readStringFromStdin(unsigned maxLength);

// ### Printing ###

void printWordEntry(WordEntry *we, Context *ctx);
void printTranslationEntry(TranslationEntry *te);
void clearTheScreen();

// ### User Input ###

int inputNumber(int min, int max);
TranslationNode *inputTranslations(Language lang);

// ### User Accessible Commands ###

void addWord(WordEntry *we, WordNode *head);
void getWord(char *word, WordNode *head, Context *ctx);
void listWordsWithoutTranslations(WordNode *head, Context *ctx);
void listAllWords(WordNode *head, Context *ctx);

// ### Utility ###

char *languageToString(Language lang);
unsigned countTranslationNodes(TranslationNode *head);

#endif // LIB_H_INCLUDED
