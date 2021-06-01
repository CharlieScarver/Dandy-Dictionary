#include "main.h"
#include "lib.h"

/*
The word “frobnicate” in IT is a kind of techie slang that describes tweaking or fiddling with physical parts of a technology.
Frobnicate has been regularly abbreviated as “frob” or related to a piece of hardware architecture called a “frobnitz.”
*/

int main()
{
    writeTestDataToBin();

    // --- Choose language ---
    printf("Choose a language:\n  1. Bulgarian\n  2. English\n  3. German\n");
    printf("\n> ");

    int languageInput = inputNumber(1, 3);
    // Create the context
    Context *ctx = malloc(sizeof(Context));

    char filename[10];
    switch (languageInput) {
        case 1:
            strcpy(filename, "bg.bin");
            ctx->chosenLang = BG;
            ctx->lang1 = EN;
            ctx->lang2 = DE;
            break;
        case 2:
            strcpy(filename, "eng.bin");
            ctx->chosenLang = EN;
            ctx->lang1 = BG;
            ctx->lang2 = DE;
            break;
        case 3:
            strcpy(filename, "ger.bin");
            ctx->chosenLang = DE;
            ctx->lang1 = BG;
            ctx->lang2 = EN;
            break;
    }

    // --- Read data ---
    printf("=====\n");

    FILE *fr = fopen(filename, "rb");
    printf("ftell: %u\n", ftell(fr));

    if (fr == NULL) {
        perror("File open to read");
        exit(EXIT_FAILURE);
    }

    // The head of the list of words
    WordNode *head = NULL;
    WordNode *previous = NULL;

    while (1) {
        WordNode *wordNode = malloc(sizeof(WordNode));
        // If malloc-ed pointers are not initialized they receive placeholder value like 0xbaadf00dbaadf00d (which is != NULL) causing unwanted behavior
        wordNode->next = NULL;
        wordNode->value.translationsInLang1 = NULL;
        wordNode->value.translationsInLang2 = NULL;

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
    listAllWords(head, ctx);
    printf("=====\n");

    // --- Choose a command ---
    int commandInput = 5;
    do {
        printf("\nChoose a command:\n  1. Add a new word\n  2. Get the information for a specific word\n  3. List all words without any translations\n  4. List all words (in this dictionary)\n  5. Exit\n");
        printf("\n> ");

        commandInput = inputNumber(1, 5);

        switch (commandInput) {
            case 1: {
                // Labels can only be followed by statements, and declarations do not count as statements in C.
                // The language standard simply doesn't allow for it.
                // One way to solve this is to add {} around the case code
                WordNode *newNode = malloc(sizeof(WordNode));
                // Initialize malloc pointers! (0xbaadf00d != NULL)
                newNode->next = NULL;
                newNode->value.translationsInLang1 = NULL;
                newNode->value.translationsInLang2 = NULL;

                printf("\nEnter the word you want to add:\n");
                printf("\n> ");
                strcpy(newNode->value.word, readStringFromStdin(MAX_WORD_LEN));

                printf("\nEnter the date in the following format \"dd.mm.yyyy\":\n");
                printf("\n> ");
                strcpy(newNode->value.addedOn, readStringFromStdin(MAX_WORD_LEN));

                printf("\nEnter the translation rating (0-10):\n");
                printf("\n> ");
                newNode->value.rating = inputNumber(0, 10);

                // Enter translations in language #1
                newNode->value.translationsInLang1 = inputTranslations(ctx->lang1);
                // Enter translations in language #2
                newNode->value.translationsInLang2 = inputTranslations(ctx->lang2);

                clearTheScreen();
                addWord(newNode, head);
                break;
            }
            case 2: {
                // Labels can only be followed by statements, and declarations do not count as statements in C.
                // The language standard simply doesn't allow for it.
                // One way to solve this is to add {} around the case code
                printf("\nEnter the word you want to inspect:\n");
                printf("\n> ");
                char word[MAX_WORD_LEN];
                strcpy(word, readStringFromStdin(MAX_WORD_LEN));

                clearTheScreen();
                getWord(word, head, ctx);
                break;
            }
            case 3:
                clearTheScreen();
                listWordsWithoutTranslations(head, ctx);
                break;
            case 4:
                clearTheScreen();
                listAllWords(head, ctx);
                break;
        }
    } while (commandInput != 5);

    return 0;
}
