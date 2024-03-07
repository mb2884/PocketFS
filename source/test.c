#include <stdio.h>
#include "test.h"
#include "display.h"

void testFileOperations() {
    printf("Testing File Operations:\n");

    // Create a directory
    Directory* rootDirectory = createDirectory("Root", NULL);

    // Create a file
    File* file1 = createFile("file1.txt", rootDirectory);
    writeFile(file1, "This is file1.txt content.");
    
    // Edit file
    editFile(file1, "This is updated content for file1.txt.");

    // Rename file
    renameFile(file1, "file1_updated.txt");

    // Create another file
    File* file2 = createFile("file2.txt", rootDirectory);
    writeFile(file2, "This is file2.txt content.");

    // Copy file
    copyFile(file2, rootDirectory);
    
    // Print all files
    printAllFiles(rootDirectory);

    // Move file
    Directory* subdir = createDirectory("Subdirectory", rootDirectory);
    moveFile(file2, subdir);

    // Delete file
    deleteFile(file1);
    deleteFile(file2);

    // Delete root directory
    deleteDirectory(rootDirectory);

    printf("File operations testing complete.\n\n");
}

void testDirectoryOperations() {
    printf("Testing Directory Operations:\n");

    // Create a directory
    Directory* rootDirectory = createDirectory("Root", NULL);

    // Create subdirectories
    Directory* subDir1 = createDirectory("Subdirectory1", rootDirectory);
    Directory* subDir2 = createDirectory("Subdirectory2", rootDirectory);

    // Rename directory
    renameDirectory(subDir2, "UpdatedSubdirectory2");

    // Create files in directories
    File* file1 = createFile("file1.txt", rootDirectory);
    File* file2 = createFile("file2.txt", subDir1);

    // Get files from directory
    File* fileFromRoot = getFileFromDirectory(rootDirectory, "file1.txt");
    File* fileFromSubdir = getFileFromDirectory(subDir1, "file2.txt");

    if (fileFromRoot && fileFromSubdir) {
        printf("Found files:\n");
        printf("File from root directory: %s\n", fileFromRoot->name);
        printf("File from subdirectory: %s\n", fileFromSubdir->name);
    } else {
        printf("Failed to find files.\n");
    }

    // Move directory
    moveDirectory(subDir1, subDir2);

    // Get all files from directory
    printAllFiles(rootDirectory);

    // Get all subdirectories from directory
    printAllSubdirectories(rootDirectory);

    // Delete files and directories
    deleteFile(file1);
    deleteFile(file2);
    deleteDirectory(rootDirectory);

    printf("Directory operations testing complete.\n\n");
}

