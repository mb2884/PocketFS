#include "file.h"
#include "dir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

File *createFile(char *name, Directory *parentDirectory)
{
    File *file = (File *)malloc(sizeof(File));
    if (file)
    {
        file->name = name;
        file->data = NULL;
        file->size = 0;
        file->capacity = 0;
        file->cursorPosition = 0;
        file->parentDirectory = parentDirectory;

        // Update the number of files in the parent directory
        if (parentDirectory)
        {
            parentDirectory->files = (File **)realloc(parentDirectory->files, (parentDirectory->num_files + 1) * sizeof(File *));
            if (!parentDirectory->files)
            {
                fprintf(stderr, "Failed to allocate memory for files\n");
                free(file); // Free allocated memory for file
                return NULL;
            }
            parentDirectory->files[parentDirectory->num_files] = file;
            parentDirectory->num_files++;
        }
    }
    return file;
}

void writeFile(File *file, const char *data)
{
    file->size = 0;
    file->cursorPosition = 0;

    int dataLength = strlen(data);
    if (file->size + dataLength >= file->capacity)
    {
        int newCapacity = file->capacity + dataLength + 1;
        char *newData = (char *)realloc((void *)file->data, newCapacity);
        if (!newData)
        {
            fprintf(stderr, "Failed to allocate memory for file data\n");
            return;
        }
        file->data = newData;
        file->capacity = newCapacity;
    }
    strcpy((char *)(file->data) + file->size, data);
    file->size += dataLength;
}

void appendFile(File *file, const char *data)
{
    int dataLength = strlen(data);
    if (file->size + dataLength >= file->capacity)
    {
        int newCapacity = file->capacity + dataLength + 1;
        char *newData = (char *)realloc((void *)file->data, newCapacity);
        if (!newData)
        {
            fprintf(stderr, "Failed to allocate memory for file data\n");
            return;
        }
        file->data = newData;
        file->capacity = newCapacity;
    }
    strcpy((char *)(file->data) + file->size, data);
    file->size += dataLength;
}

void insertAtPosition(File *file, const char *data, int position)
{
    if (position < 0 || position > file->size)
    {
        fprintf(stderr, "Invalid position for insertion\n");
        return;
    }

    int dataLength = strlen(data);
    int newSize = file->size + dataLength;

    // Reallocate memory to accommodate the new data
    char *newData = (char *)realloc(file->data, newSize + 1); // +1 for the null terminator
    if (!newData)
    {
        fprintf(stderr, "Failed to allocate memory for file data\n");
        return;
    }

    // Shift existing data to make space for the new data
    memmove(newData + position + dataLength, newData + position, file->size - position);

    // Copy the new data into the file at the specified position
    memcpy(newData + position, data, dataLength);

    // Update file properties
    file->data = newData;
    file->size = newSize;
    file->data[newSize] = '\0'; // Null-terminate the string
}

void deleteFile(File *file)
{
    if (!file)
        return;

    // Remove the file from its parent directory's list of files
    if (file->parentDirectory)
    {
        for (int i = 0; i < file->parentDirectory->num_files; ++i)
        {
            if (file->parentDirectory->files[i] == file)
            {
                // Shift the remaining files to fill the gap
                for (int j = i; j < file->parentDirectory->num_files - 1; ++j)
                {
                    file->parentDirectory->files[j] = file->parentDirectory->files[j + 1];
                }
                file->parentDirectory->num_files--;
                break;
            }
        }
    }

    // Free the data
    free((void *)file->data); // Cast away const

    // Free the file structure itself
    free(file);
}

void renameFile(File *file, char *newName)
{
    file->name = newName;
}

void copyFile(File *sourceFile, Directory *destinationDirectory)
{
    // Create a new file with the same name and contents
    File *newFile = createFile(sourceFile->name, destinationDirectory);
    if (!newFile)
    {
        fprintf(stderr, "Failed to copy file: Unable to create new file\n");
        return;
    }
    writeFile(newFile, sourceFile->data);
}

void moveFile(File *file, Directory *newParentDirectory)
{
    // Remove the file from its current parent's files
    if (file->parentDirectory)
    {
        for (int i = 0; i < file->parentDirectory->num_files; ++i)
        {
            if (file->parentDirectory->files[i] == file)
            {
                // Shift the remaining files to fill the gap
                for (int j = i; j < file->parentDirectory->num_files - 1; ++j)
                {
                    file->parentDirectory->files[j] = file->parentDirectory->files[j + 1];
                }
                file->parentDirectory->num_files--;
                break;
            }
        }
    }

    // Update the file's parent to the new parent
    file->parentDirectory = newParentDirectory;
}
