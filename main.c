#include "lib.h"

int main()
{
    writeTestDataToBin();

    // --- 2 --- Read data
    printf("=====\n");

    FILE *fr = fopen("bg.bin", "rb");
        printf("ftell: %u\n", ftell(fr));

    if (fr == NULL) {
        perror("File open 2");
        exit(EXIT_FAILURE);
    }

    WordNode *head = NULL;
    WordNode *previous = NULL;

    while (1) {
        WordNode *wordNode = malloc(sizeof(WordNode));
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
