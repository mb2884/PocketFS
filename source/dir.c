#include "dir.h"
#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Directory *createDirectory(const char *name, Directory *parentDirectory)
{
    Directory *directory = (Directory *)malloc(sizeof(Directory));
    if (directory)
    {
        directory->name = name;
        directory->num_subdirectories = 0;
        directory->subdirectories = NULL;
        directory->num_files = 0;
        directory->files = NULL;
        directory->parentDirectory = parentDirectory;

        // Update the parent directory's list of subdirectories
        if (parentDirectory)
        {
            parentDirectory->subdirectories = (Directory **)realloc(parentDirectory->subdirectories, (parentDirectory->num_subdirectories + 1) * sizeof(Directory *));
            if (!parentDirectory->subdirectories)
            {
                fprintf(stderr, "Failed to allocate memory for subdirectories\n");
                free(directory); // Free allocated memory for directory
                return NULL;
            }
            parentDirectory->subdirectories[parentDirectory->num_subdirectories] = directory;
            parentDirectory->num_subdirectories++;
        }
    }
    return directory;
}

void deleteDirectory(Directory *directory)
{
    if (!directory)
        return;

    // Remove the directory from its parent's list of subdirectories
    if (directory->parentDirectory)
    {
        for (int i = 0; i < directory->parentDirectory->num_subdirectories; ++i)
        {
            if (directory->parentDirectory->subdirectories[i] == directory)
            {
                // Shift the remaining subdirectories to fill the gap
                for (int j = i; j < directory->parentDirectory->num_subdirectories - 1; ++j)
                {
                    directory->parentDirectory->subdirectories[j] = directory->parentDirectory->subdirectories[j + 1];
                }
                directory->parentDirectory->num_subdirectories--;
                break;
            }
        }
    }

    // Free memory allocated for subdirectories and files
    for (int i = 0; i < directory->num_subdirectories; ++i)
    {
        deleteDirectory(directory->subdirectories[i]);
    }
    free(directory->subdirectories);

    for (int i = 0; i < directory->num_files; ++i)
    {
        deleteFile(directory->files[i]);
    }
    free(directory->files);

    free(directory);
}

void renameDirectory(Directory *directory, const char *newName)
{
    directory->name = newName;
}

File *getFileFromDirectory(Directory *directory, const char *filename)
{
    for (int i = 0; i < directory->num_files; ++i)
    {
        if (strcmp(directory->files[i]->name, filename) == 0)
        {
            return directory->files[i];
        }
    }
    return NULL;
}

File **getAllFilesFromDirectory(Directory *directory)
{
    return directory->files;
}

Directory **getAllSubdirectoriesFromDirectory(Directory *directory)
{
    return directory->subdirectories;
}

void moveDirectory(Directory *directory, Directory *newParentDirectory)
{
    // Remove the directory from its current parent's subdirectories
    if (directory->parentDirectory)
    {
        for (int i = 0; i < directory->parentDirectory->num_subdirectories; ++i)
        {
            if (directory->parentDirectory->subdirectories[i] == directory)
            {
                // Shift the remaining subdirectories to fill the gap
                for (int j = i; j < directory->parentDirectory->num_subdirectories - 1; ++j)
                {
                    directory->parentDirectory->subdirectories[j] = directory->parentDirectory->subdirectories[j + 1];
                }
                directory->parentDirectory->num_subdirectories--;
                break;
            }
        }
    }

    // Update the directory's parent to the new parent
    directory->parentDirectory = newParentDirectory;

    // Add the directory to the new parent's subdirectories
    if (newParentDirectory)
    {
        newParentDirectory->subdirectories = (Directory **)realloc(newParentDirectory->subdirectories, (newParentDirectory->num_subdirectories + 1) * sizeof(Directory *));
        if (!newParentDirectory->subdirectories)
        {
            fprintf(stderr, "Failed to allocate memory for subdirectories\n");
            return;
        }
        newParentDirectory->subdirectories[newParentDirectory->num_subdirectories] = directory;
        newParentDirectory->num_subdirectories++;
    }

    // Update the number of subdirectories in the old parent directory
    if (directory->parentDirectory)
    {
        directory->parentDirectory->num_subdirectories--;
    }
}
