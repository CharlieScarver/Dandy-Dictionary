#include "lib.h"

// ### Writing ###

// String is prefixed by its length
void writeStringToBin(char *str, FILE *fr) {
    unsigned length = strlen(str);
    // Write the length
    if (fwrite(&length, sizeof(unsigned), 1, fr) != 1) {
        if (!feof(fr)) {
            perror("Read error");
            exit(EXIT_FAILURE);
        }
    }

    // Write the string
    if (fwrite(str, sizeof(char), length, fr) != length) {
        if (!feof(fr)) {
            perror("Read error");
            exit(EXIT_FAILURE);
        }
    }
}

void writeTestDataToBin() {
    char word[MAX_WORD_LEN] = "kniga";
    char engTranslation[MAX_WORD_LEN] = "book";
    char gerTranslation[MAX_WORD_LEN] = "buch";
    char type = 'n';
    char engSentence[MAX_SENT_LEN] = "I'm reading a book.";
    char gerSentence[MAX_SENT_LEN] = "Ich lese ein Buch.";
    char addedOn[DATE_LEN] = "23.05.2021";
    unsigned rating = 10;
    // fgets(word, 20, stdin);

    FILE *fw = fopen("bg.bin", "wb");

    if (fw == NULL) {
        perror("File open to write");
        exit(EXIT_FAILURE);
    }

    unsigned wordLen = strlen(word);

    // Remove the extra newline after fgets()
    if (word[wordLen-1] == '\n') {
        word[wordLen-1] = '\0';
        wordLen--;
    }

    printf("%s-\n", word);
    printf("len = %d\n", wordLen);

    // -- Write the word --
    writeStringToBin(word, fw);

    // -- Write the EN translations --
    unsigned numberOfTranslations = 1;
    if (fwrite(&numberOfTranslations, sizeof(unsigned), 1, fw) != 1) {
        perror("Write error");
        exit(EXIT_FAILURE);
    }

    // Write the translation
    writeStringToBin(engTranslation, fw);
    // Write the type
    if (fwrite(&type, sizeof(char), 1, fw) != 1) {
        if (!feof(fw)) {
            perror("Read error");
            exit(EXIT_FAILURE);
        }
    }
    // Write the sentence
    writeStringToBin(engSentence, fw);

    // -- Write the GR translations --
    numberOfTranslations = 1;
    if (fwrite(&numberOfTranslations, sizeof(unsigned), 1, fw) != 1) {
        perror("Write error");
        exit(EXIT_FAILURE);
    }

    // Write the translation
    writeStringToBin(gerTranslation, fw);
    // Write the type
    if (fwrite(&type, sizeof(char), 1, fw) != 1) {
        if (!feof(fw)) {
            perror("Read error");
            exit(EXIT_FAILURE);
        }
    }
    // Write the sentence
    writeStringToBin(gerSentence, fw);

    // -- Write the added on date --
    writeStringToBin(addedOn, fw);

    // -- Write the rating --
    if (fwrite(&rating, sizeof(unsigned), 1, fw) != 1) {
        perror("Write error");
        exit(EXIT_FAILURE);
    }

    fclose(fw);
}

// ### Reading ###

// String is prefixed by its length
char *readStringFromBin(FILE *fr) {
    unsigned length;
    // Read the length
    if (fread(&length, sizeof(unsigned), 1, fr) != 1) {
        if (!feof(fr)) {
            perror("Read error");
            exit(EXIT_FAILURE);
        } else {
            // If the file has ended, return NULL
            // We assume this can only happen here
            // Either all fields are present or they're all missing and the first thing that we'll try to read is a string
            // So here is the first-most point of failure, if this reading is successful all readings after it will be too (until the next word entry)
            return NULL;
        }
    }

    // Won't persist outside. Stack frame cleared after function ends?
    // Memory not accessible
    // char* buffer[length+1];

    char* buffer = malloc(sizeof(char)*(length+1));
    // Read the string
    if (fread(buffer, sizeof(char), length, fr) != length) {
        if (!feof(fr)) {
            perror("Read error");
            exit(EXIT_FAILURE);
        }
    }

    buffer[length] = '\0';

    printf("len = %u\n", strlen(buffer));
    printf("%s-\n", buffer);

    // Won't persist outside. Can't change pointers in functions because they're copies (pointers are passed by value);
    // Memory not accessible
    //output = buffer;

    // Passing a pointer to a function is basically passing the Value by Address but passing the Pointer Variable by Value.
    // This means reassigning the pointer will not persist outside the function.

    return buffer;
}

TranslationNode *readTranslationsFromBin(FILE *fr) {
    unsigned length;
    // Read the length
    if (fread(&length, sizeof(unsigned), 1, fr) != 1) {
        if (!feof(fr)) {
            perror("Read error");
            exit(EXIT_FAILURE);
        }
    }

    if (length == 0) {
        printf("No translations\n");
        return NULL;
    }

    // If pointers are not initialized they can receive a garbage value (which is != NULL) causing unwanted behavior
    TranslationNode* head = NULL;
    TranslationNode* previous = NULL;

    for (int i = 0; i < length; i++) {
        // Data becomes garbage outside of the function (but accessible)
        // TranslationNode transNode;

        TranslationNode *transNode = malloc(sizeof(TranslationNode));
        // If malloc-ed pointers are not initialized they receive placeholder value like 0xbaadf00dbaadf00d (which is != NULL) causing unwanted behavior
        transNode->next = NULL;

        if (previous == NULL) {
            head = transNode;
            previous = transNode;
        } else {
            previous->next = transNode;
        }

        // Read the translation
        strcpy(transNode->value.translation, readStringFromBin(fr));

        // Read the type
        char type;
        if (fread(&type, sizeof(char), 1, fr) != 1) {
            if (!feof(fr)) {
                perror("Read error");
                exit(EXIT_FAILURE);
            }
        }
        transNode->value.type = type;

        // Read the example sentence
        strcpy(transNode->value.sentence, readStringFromBin(fr));
    }

    return head;
}

char *readStringFromStdin(unsigned maxLength) {
    char *word = malloc(sizeof(char)*(maxLength+1));
    fgets(word, MAX_WORD_LEN, stdin);

    unsigned wordLen = strlen(word);
    // Remove the extra newline after fgets()
    if (word[wordLen-1] == '\n') {
        word[wordLen-1] = '\0';
        wordLen--;
    }

    return word;
}

// ### Printing ###

void printWordEntry(WordEntry *we, Context *ctx) {
    printf("{\n  word: %s,\n  translationsIn%s: [", we->word, languageToString(ctx->lang1));
    if(we->translationsInLang1 != NULL) {
        for (TranslationNode *p = we->translationsInLang1; p != NULL; p = p->next) {
            printTranslationEntry(&p->value);
        }
    }
    printf("],\n  translationsIn%s: [", languageToString(ctx->lang2));
    if(we->translationsInLang2 != NULL) {
        for (TranslationNode *p = we->translationsInLang2; p != NULL; p = p->next) {
            printTranslationEntry(&p->value);
        }
    }
    printf("],\n  addedOn: %s,\n  rating: %u\n}, ", we->addedOn, we->rating);
}

void printTranslationEntry(TranslationEntry *te) {
    printf("{\n    translation: %s,\n    type: %c,\n    sentence: %s\n  }", te->translation, te->type, te->sentence);
}

void clearTheScreen() {
    // Clear the screen
    system("cls");
}

// ### User Input ###

int inputNumber(int min, int max) {
    bool inputAgain = false;
    int input = -1;
    do {
        scanf("%d", &input);
        getchar(); // Consume the newline
        if (input < min || input > max) {
            printf("\nInvalid input. Valid input is %d <= X <= %d.\n", min, max);
            printf("Enter a valid number:\n\n");
            printf("\n> ");
            inputAgain = true;
        } else {
            inputAgain = false;
        }
    } while(inputAgain);

    return input;
}

TranslationNode *inputTranslations(Language lang) {
    printf("\nEnter the number of translations in %s (0-20):\n", languageToString(lang));
    printf("\n> ");
    int numberOfTranslationsLang1 = inputNumber(0, 20);

    TranslationNode *head = NULL;
    TranslationNode *previousTranslationsLang1 = NULL;
    for (int i = 0; i < numberOfTranslationsLang1; i++) {
        TranslationNode *newTransNode = malloc(sizeof(TranslationNode));
        // Initialize malloc pointers! (0xbaadf00d != NULL)
        newTransNode->next = NULL;

        if (previousTranslationsLang1 == NULL) {
            previousTranslationsLang1 = newTransNode;
            // Head of the translations
            head = newTransNode;
        } else {
            previousTranslationsLang1->next = newTransNode;
        }

        printf("\nTranslation number %d):\n", i+1);
        printf("\nEnter a translation:\n", i);
        printf("\n> ");
        strcpy(newTransNode->value.translation, readStringFromStdin(MAX_WORD_LEN));

        printf("\nEnter a type:\n");
        printf("\n> ");
        newTransNode->value.type = *readStringFromStdin(1);

        printf("\nEnter a sentence:\n");
        printf("\n> ");
        strcpy(newTransNode->value.sentence, readStringFromStdin(MAX_SENT_LEN));

        previousTranslationsLang1 = newTransNode;

        printf("\nTranslation %d added successfuly.\n\n", i+1);
    }

    return head;
}

// ### User Accessible Commands ###

void addWord(WordEntry *we, WordNode *head) {
    for (WordNode *p = head; p != NULL; p = p->next) {
        if (p->next == NULL) {
            WordNode *newNode = malloc(sizeof(WordNode));
            // Initialize malloc pointers!
            newNode->next = NULL;
            newNode->value = *we;
            p->next = newNode;
            break;
        }
    }

    // TODO: Write to file as well??
}

void getWord(char *word, WordNode *head, Context *ctx) {
    printf("\nThe word entry for \"%s\" is:\n", word);
    for (WordNode *p = head; p != NULL; p = p->next) {
        if (strcmp(p->value.word, word) == 0) {
            printWordEntry(&p->value, ctx);
        }
    }
    printf("\n");
}

void listWordsWithoutTranslations(WordNode *head, Context *ctx) {
    printf("\nThe word entries without any translations are:\n");
    for (WordNode *p = head; p != NULL; p = p->next) {
        if (p->value.translationsInLang1 == NULL && p->value.translationsInLang2 == NULL) {
            printWordEntry(&p->value, ctx);
        }
    }
    printf("\n");
}

void listAllWords(WordNode *head, Context *ctx) {
    printf("\nListing all word entries in the dictionary:\n");
    for (WordNode *p = head; p != NULL; p = p->next) {
        printWordEntry(p, ctx);
    }
    printf("\n");
}

// ### Utility ###

char *languageToString(Language lang) {
    char langName[MAX_WORD_LEN];
    switch (lang) {
    case BG:
        return "Bulgarian";
    case EN:
        return "English";
    case DE:
        return "German";
    }
}
