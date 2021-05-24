#include "lib.h"

int main()
{
    writeTestDataToBin();

    // -- Choose language --
    printf("Choose a language:\n  1. Bulgarian\n  2. English\n  3. German\n");
    printf("\n> ");

    int input = inputNumber(1, 3);

    char filename[10];
    switch (input) {
        case 1:
            strcpy(filename, "bg.bin");
            break;
        case 2:
            strcpy(filename, "eng.bin");
            break;
        case 3:
            strcpy(filename, "ger.bin");
            break;
    }

    // -- Read data --
    printf("=====\n");

    FILE *fr = fopen(filename, "rb");
        printf("ftell: %u\n", ftell(fr));

    if (fr == NULL) {
        perror("File open to read");
        exit(EXIT_FAILURE);
    }

    WordNode *head = NULL;
    WordNode *previous = NULL;

    while (1) {
        WordNode *wordNode = malloc(sizeof(WordNode));
        // If malloc-ed pointers are not initialized they receive placeholder value like 0xbaadf00dbaadf00d (which is != NULL) causing unwanted behavior
        wordNode->next = NULL;

        // Read the word
        char *word = readStringFromBin(fr);
        if (word == NULL) {
            break;
        }
        strcpy(wordNode->value.word, word);

        // Read the translations
        wordNode->value.translationsInLang1 = readTranslationsFromBin(fr);
        wordNode->value.translationsInLang2 = readTranslationsFromBin(fr);

        // Read the added on date
        strcpy(wordNode->value.addedOn, readStringFromBin(fr));

        // Read the rating
        unsigned rating = 0;
        if (fread(&rating, sizeof(unsigned), 1, fr) != 1) {
            if (feof(fr)) {
                break;
            } else {
                perror("Read error");
                exit(EXIT_FAILURE);
            }
        }
        wordNode->value.rating = rating;

        // This has to happen after at least the first reading
        // to make sure we detect EOF before assigning previous->next
        if (previous == NULL) {
            head = wordNode;
            previous = wordNode;
        } else {
            previous->next = wordNode;
        }

        printf("ftell: %u\n", ftell(fr));
        // position 67 (of 532 bits) => 1 position = 1 byte = 8 bits
    }

    printf("=====\n");
    for (WordNode *p = head; p != NULL; p = p->next) {
        printWordEntry(p);
    }

    return 0;
}
