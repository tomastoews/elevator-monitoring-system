#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

char* intToString(int n) {
    char *num_char = malloc(3); // TODO: Calculate needed char count.
    memset(num_char, ' ', strlen(num_char));
    sprintf(num_char, "%d", n);
    num_char[2] = '\0';
    return num_char;
}
