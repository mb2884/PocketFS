#include "file.h"
#include "dir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to create a file
File *createFile(char *name, Directory *parentDirectory) {
    File *file = (File *)malloc(sizeof(File));
    if (file == NULL)
        return NULL;
    strcpy(file->name, name);
    file->size = 0;
    file->capacity = MAX_SIZE;
    file->cursor_position = 0;
    file->parent_directory = parentDirectory;
    if (parentDirectory != NULL)
        parentDirectory->files[parentDirectory->file_count++] = file;
    return file;
}

// Function to write data to a file
void writeFile(File *file, const char *data) {
    if (file == NULL || data == NULL)
        return;
    strcpy(file->data, data);
    file->size = strlen(data);
}

// Function to append data to a file
void appendFile(File *file, const char *data) {
    if (file == NULL || data == NULL)
        return;
    strcat(file->data, data);
    file->size += strlen(data);
}

// Function to insert data at a specific position in a file
void insertAtPosition(File *file, const char *data, int position) {
    if (file == NULL || data == NULL || position < 0 || position > file->size)
        return;
    memmove(file->data + position + strlen(data), file->data + position, strlen(file->data) - position + 1);
    memcpy(file->data + position, data, strlen(data));
    file->size += strlen(data);
}

// Function to delete a file
void deleteFile(File *file) {
    if (file == NULL)
        return;
    Directory* parentDirectory = file->parent_directory;
    if (parentDirectory != NULL) {
        for (int i = 0; i < parentDirectory->file_count; i++) {
            if (parentDirectory->files[i] == file) {
                for (int j = i; j < parentDirectory->file_count - 1; j++) {
                    parentDirectory->files[j] = parentDirectory->files[j + 1];
                }
                parentDirectory->files[parentDirectory->file_count - 1] = NULL;
                parentDirectory->file_count--;
                break;
            }
        }
    }
    free(file);
}

// Function to rename a file
void renameFile(File *file, char *newName) {
    if (file != NULL && newName != NULL)
        strcpy(file->name, newName);
}

// Function to copy a file to a destination directory
void copyFile(File *sourceFile, Directory *destinationDirectory) {
    if (sourceFile == NULL || destinationDirectory == NULL)
        return;
    File *copy = createFile(sourceFile->name, destinationDirectory);
    writeFile(copy, sourceFile->data);
}

// Function to move a file to a new parent directory
void moveFile(File *file, Directory *newParentDirectory) {
    if (file == NULL || newParentDirectory == NULL)
        return;
    Directory *oldParent = file->parent_directory;
    if (oldParent != NULL) {
        for (int i = 0; i < oldParent->file_count; i++) {
            if (oldParent->files[i] == file) {
                for (int j = i; j < oldParent->file_count - 1; j++) {
                    oldParent->files[j] = oldParent->files[j + 1];
                }
                oldParent->files[oldParent->file_count - 1] = NULL;
                oldParent->file_count--;
                break;
            }
        }
    }
    file->parent_directory = newParentDirectory;
    newParentDirectory->files[newParentDirectory->file_count++] = file;
}
