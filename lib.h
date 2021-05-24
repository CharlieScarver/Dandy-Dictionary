#ifndef LIB_H_INCLUDED
#define LIB_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LEN 50
#define MAX_SENT_LEN 255
#define DATE_LEN 11 // +1 char for \0 terminator

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


// ### Writing ###

// String is prefixed by its length
void writeStringToBin(char *string, FILE *fr);
void writeTestDataToBin();

// ### Reading ###

// String is prefixed by its length
char* readStringFromBin(FILE *fr);
TranslationNode* readTranslationsFromBin(FILE *fr);

// ### Printing ###

void printWordEntry(WordEntry *we);
void printTranslationEntry(TranslationEntry *te);

#endif // LIB_H_INCLUDED
