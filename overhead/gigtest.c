#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "../tst.h"
#include "../brk.h"

int main(int agrc, char **argv) {
    int i;
    for (i = 0; i < 1000000; i++) {
        void *p = malloc(1024 * 1024 * 1024);
        free(p);
    }
    return (0);
}