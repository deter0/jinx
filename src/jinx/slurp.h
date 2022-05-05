#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <limits.h>

char currPath[PATH_MAX] = {'\0'};

// https://stackoverflow.com/a/298518
void getPath() {
    if (*currPath == '\0') {
        if (getcwd(currPath, sizeof(currPath)) == NULL) {
            perror("getcwd() error");
            exit(1);
        }
    }
}

// avoid naming collisions
char *jinxSlurpFile(char *path) {
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file: %s\n", path);
        getPath();
        fprintf(stderr, "\tFYI, Current directory is: %s\n", currPath);
        return NULL;
    }
    size_t fileSize;
    fseek(fp, 0L, SEEK_END);
    fileSize = ftell(fp);
    rewind(fp);
    char *buffer = malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Error allocating a buffer of size %zu for file %s\n", fileSize, path);
        fclose(fp);
        return NULL;
    }
    if (fread(buffer, fileSize, 1, fp) != 1) {
        fprintf(stderr, "Error reading file: %s\n", path);
        free(buffer);
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    return buffer;
}
