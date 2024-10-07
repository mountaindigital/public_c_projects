#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INPUT_BUFFER_SIZE   BUFSIZ 
#define MIN_ARR_SIZE        BUFSIZ 

char const*const delim = " ";

void quicksort() {
    
}

// removes newline chars and nonprintable chars
char* processstr(size_t len, char str[len]) {
    printf("debug str[%zu]: %s\n", len, str);
    // ignore strings that start with non-alphanumeric chars
    if (!isalnum(str[0])) 
        return (char*){0};
    
    // remove trailing non-alphanumeric chars
    if (len > 2 && !isalnum(str[len - 2])) {
        puts("got here");
        printf("%c\n", str[len - 2]);
        str[len - 2] = '\0';
    }

    return str;
}

typedef struct _strarr {
   char** arr;
   size_t len;
   size_t cap;
} strarr;

void strarr_append(strarr* sa, char* elem) {
    size_t len = strlen(elem) + 1;
    char* elemcpy = malloc(len);
    strncpy(elemcpy, elem, len);
    if (sa->len + 1 >= sa->cap) {
        sa->cap *= 1.5;
        sa->arr = realloc(sa->arr, sa->cap * sizeof(char*));
    }

    sa->arr[sa->len] = elemcpy;
    sa->len++; 
}

int main(int argc, char* argv[argc + 1]) {
    size_t const len = 4;
    char* myarr[] = {"print1", " notprint1", "\n", "print2\n", };
    for (size_t i = 0; i < len; i++) {
        char* res = processstr(strlen(myarr[i])+1, myarr[i]);
        if (res) printf("result: %s\n", res);
    }
    // obv delete when ready
    return 0;

    if (argc < 2) {
        fprintf(stderr, "please provide filename of ints to ingest\n");
        return EXIT_FAILURE;
    }

    FILE* inputf = fopen(argv[1], "r");
    if (!inputf) {
        perror("could not open file");
        return EXIT_FAILURE;
    }
    if (errno == EINVAL) {
        fprintf(stderr, "could not open file: mode is invalid\n");
        return EXIT_FAILURE;
    }

    static char input[INPUT_BUFFER_SIZE] = {0};
    strarr sa = {
        .arr = malloc(MIN_ARR_SIZE * sizeof(char*)),
        .len = 0,
        .cap= MIN_ARR_SIZE,
    };

    char* str = {0};
    char* tok = {0};
    char* procstr = {0};
    while ((str = fgets(input, INPUT_BUFFER_SIZE, inputf)) != NULL) {
        while ((tok = strtok(str, delim)) != NULL) {
            str = NULL; // necessary to continue pulling tokens from str
            procstr = processstr(strlen(tok) + 1, tok);
            if (procstr) 
                strarr_append(&sa, tok);
            sleep(1);
            printf("sa[%zu] => %s\n", sa.len, sa.arr[sa.len - 1]);
        }
    }

    return EXIT_SUCCESS;
}
