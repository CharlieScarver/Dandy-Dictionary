#include "lib.h"

// ### Writing ###

// String is prefixed by its length
void writeStringToBin(char *string, FILE *fr) {
    unsigned length = strlen(string);
    // Write the length
    if (fwrite(&length, sizeof(unsigned), 1, fr) != 1) {
        if (!feof(fr)) {
            perror("Read error");
            exit(EXIT_FAILURE);
        }
    }

    // Write the string
    if (fwrite(string, sizeof(char), length, fr) != length) {
        if (!feof(fr)) {
            perror("Read error");
            exit(EXIT_FAILURE);
        }
    }
}

void writeTestDataToBin() {
    char word[MAX_WORD_LEN] = "kniga";
    char translation[MAX_WORD_LEN] = "book";
    char type = 'n';
    char engSentence[MAX_SENT_LEN] = "I'm reading a book.";
    char gerSentence[MAX_SENT_LEN] = "Ich lese ein Buch.";
    char addedOn[DATE_LEN] = "23.05.2021";
    unsigned rating = 10;
    // fgets(word, 20, stdin);

    FILE *fw = fopen("bg.bin", "wb");

    if (fw == NULL) {
        perror("File open");
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
    writeStringToBin(translation, fw);
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
    numberOfTranslations = 0;
    if (fwrite(&numberOfTranslations, sizeof(unsigned), 1, fw) != 1) {
        perror("Write error");
        exit(EXIT_FAILURE);
    }

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
char* readStringFromBin(FILE *fr) {
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

TranslationNode* readTranslationsFromBin(FILE *fr) {
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

// ### Printing ###

void printWordEntry(WordEntry *we) {
    printf("{\n  word: %s,\n  translationsInLang1: [", we->word);
    if(we->translationsInLang1 != NULL) {
        for (TranslationNode *p = we->translationsInLang1; p != NULL; p = p->next) {
            printTranslationEntry(&p->value);
        }
    }
    printf("],\n  translationsInLang2: [");
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
