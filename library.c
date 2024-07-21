#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "uthash.h"

#define MAX_PATH 260

typedef struct {
    char fileName[MAX_PATH];  // key
    unsigned char* content;   // file content
    size_t contentSize;       // size of content
    UT_hash_handle hh;        // makes this structure hashable
} FileEntry;

void ListFilesInDirectory(const char* folderPath);
void PrintFilesToFile(const char* folderPath, const char* outputFilePath);
void SaveBinaryContentToHashmap(const char* folderPath, FileEntry** fileMap);
void CompareAndPrintDuplicates(FileEntry* fileMap);
void FreeHashmap(FileEntry* fileMap);
unsigned char* ReadFileContent(const char* filePath, size_t* size);

int main() {
    char folderPath[MAX_PATH];
    char outputFilePath[MAX_PATH];
    char matchingBinaryFilePath[MAX_PATH];

    printf("Enter the folder path: ");
    fgets(folderPath, MAX_PATH, stdin); // PIPES user input form STDIN

    // Remove the newline character at the end if it exists
    size_t len = strlen(folderPath);
    if (folderPath[len - 1] == '\n') {
        folderPath[len - 1] = '\0';
    }

    printf("[?] Enter the output file path (e.g., C:\\output.txt): ");
    fgets(outputFilePath, MAX_PATH, stdin); // 1. OUTPUT FP. 2. THE UNIQUE KEY. 3 THE PIPE

    // Remove the newline character at the end if it exists
    len = strlen(outputFilePath);
    if (outputFilePath[len - 1] == '\n') {
        outputFilePath[len - 1] = '\0';
    }

    /// FUNCTION: --> Prints Duplicate "FILE NAMES" To specified Output use (ListFilesInDirectory) during debug
    /// TODO: REFACTOR THE (SaveBinaryContentToHashmap) to FIT inside of (PrintFilesToFile) function. NEED TO PASS &fileMap
    /// TODO: I SHOULD BE ABLE TO USE THE UNIQUE KEY from (PrintFilesToFile) to hash the binary contents found in (SaveBinaryContentToHashmap) w, in the function
    /// TODO: @fileMap is the hashmap that will hold the binary contents.
    // ListFilesInDirectory(folderPath);
    PrintFilesToFile(folderPath, outputFilePath);

    // Save binary contents to hashmap
    FileEntry* fileMap = NULL;
    SaveBinaryContentToHashmap(folderPath, &fileMap);

    // Compare and print duplicates
    CompareAndPrintDuplicates(fileMap);

    // Free hashmap
    FreeHashmap(fileMap);

    return 0;
}

/// CALLS WIN32 API (WIN32_FIND_DATA) TO HANDLE FILES(struct w, function accesiblity). Struct is stored in (findFileData)
/// USES: (FindFirstFile) to ascertain PWD; uses (FindNextFile) to traverse
void ListFilesInDirectory(const char* folderPath) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    // Create the search pattern
    char searchPattern[MAX_PATH];
    snprintf(searchPattern, MAX_PATH, "%s\\*.*", folderPath);

    // Find the first file in the directory
    hFind = FindFirstFile(searchPattern, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error: Unable to open directory %s\n", folderPath);
        return;
    }

    printf("Files in directory %s:\n", folderPath);
    do {
        const char* fileName = findFileData.cFileName;

        // Skip the current and parent directory entries
        if (strcmp(fileName, ".") != 0 && strcmp(fileName, "..") != 0) {
            printf("%s\n", fileName);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
}

void PrintFilesToFile(const char* folderPath, const char* outputFilePath) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    FILE* outputFile = fopen(outputFilePath, "w");

    if (outputFile == NULL) {
        printf("Error: Unable to open output file %s\n", outputFilePath);
        return;
    }

    // Create the search pattern
    char searchPattern[MAX_PATH];
    snprintf(searchPattern, MAX_PATH, "%s\\*.*", folderPath);

    // Find the first file in the directory
    hFind = FindFirstFile(searchPattern, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error: Unable to open directory %s\n", folderPath);
        fclose(outputFile);
        return;
    }

    do {
        const char* fileName = findFileData.cFileName;

        // Skip the current and parent directory entries
        if (strcmp(fileName, ".") != 0 && strcmp(fileName, "..") != 0) {
            fprintf(outputFile, "%s\n", fileName);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
    fclose(outputFile);
}

unsigned char* ReadFileContent(const char* filePath, size_t* size) {
    FILE* file = fopen(filePath, "rb");
    if (file == NULL) {
        printf("Error: Unable to open file %s\n", filePath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char* content = (unsigned char*)malloc(*size);
    if (content == NULL) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    fread(content, 1, *size, file);
    fclose(file);
    return content;
}

void SaveBinaryContentToHashmap(const char* folderPath, FileEntry** fileMap) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    // Create the search pattern
    char searchPattern[MAX_PATH];
    snprintf(searchPattern, MAX_PATH, "%s\\*.*", folderPath);

    // Find the first file in the directory
    hFind = FindFirstFile(searchPattern, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error: Unable to open directory %s\n", folderPath);
        return;
    }

    do {
        const char* fileName = findFileData.cFileName;
        if (strcmp(fileName, ".") != 0 && strcmp(fileName, "..") != 0) {
            char filePath[MAX_PATH];
            snprintf(filePath, MAX_PATH, "%s\\%s", folderPath, fileName);

            size_t contentSize;
            unsigned char* content = ReadFileContent(filePath, &contentSize);
            if (content == NULL) {
                continue;
            }

            FileEntry* newEntry = (FileEntry*)malloc(sizeof(FileEntry));
            if (newEntry == NULL) {
                printf("Error: Memory allocation failed\n");
                free(content);
                FindClose(hFind);
                return;
            }

            strncpy(newEntry->fileName, fileName, MAX_PATH);
            newEntry->content = content;
            newEntry->contentSize = contentSize;
            HASH_ADD_STR(*fileMap, fileName, newEntry);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
}

void CompareAndPrintDuplicates(FileEntry* fileMap) {
    FileEntry* currentFile;
    FileEntry* tmp;
    int duplicateCount = 0;

    // Create a secondary hashmap to check for duplicates
    FileEntry* duplicateMap = NULL;

    HASH_ITER(hh, fileMap, currentFile, tmp) {
        FileEntry* entry;
        int isDuplicate = 0;

        HASH_ITER(hh, fileMap, entry, tmp) {
            if (entry != currentFile &&
                entry->contentSize == currentFile->contentSize &&
                memcmp(entry->content, currentFile->content, currentFile->contentSize) == 0) {
                isDuplicate = 1;
                break;
            }
        }

        if (isDuplicate) {
            printf("Duplicate found: %s\n", currentFile->fileName);
            duplicateCount++;
        } else {
            // Add to duplicate map
            FileEntry* newEntry = (FileEntry*)malloc(sizeof(FileEntry));
            if (newEntry == NULL) {
                printf("Error: Memory allocation failed\n");
                FreeHashmap(duplicateMap);
                return;
            }
            strncpy(newEntry->fileName, currentFile->fileName, MAX_PATH);
            HASH_ADD_STR(duplicateMap, fileName, newEntry);
        }
    }

    if (duplicateCount == 0) {
        printf("No duplicates found.\n");
    }

    // Free the duplicate map
    FreeHashmap(duplicateMap);
}

void FreeHashmap(FileEntry* fileMap) {
    FileEntry* currentFile;
    FileEntry* tmp;

    HASH_ITER(hh, fileMap, currentFile, tmp) {
        HASH_DEL(fileMap, currentFile);
        free(currentFile->content);
        free(currentFile);
    }
}