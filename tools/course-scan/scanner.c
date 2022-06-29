/**
Copyright (c) 2022 BUSeclab

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <limits.h>
#include <math.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "memory.h"

int main(int argc, char *argv[]) {
    int pid = atoi(argv[1]);
    int dump_addresses = 0;

    char *range = argv[2];
    char *map = argv[3];

    FILE *fp;

    if (map && strcmp(map, "[stack]") == 0) {
      dump_addresses = 1;
      char filename[128];
      sprintf(filename, "./stacks/stack.%d", pid);

      fp = fopen(filename, "a+");

      if (fp == NULL) {
        perror("Could not open file for storing stack addresses.");
        exit(1);
      }
    }

    unsigned long long start = strtoull(strtok(range, "-"), NULL, 16);
    unsigned long long end = strtoull(strtok(NULL, "-"), NULL, 16);

    printf("%d: reading from %llx-%llx\n", pid, start, end);

    char line[128];

    int i = 0;
    while (fgets(line, sizeof(line), stdin)) {
        printf("%s", line);
        const char *addr = strtok(line, " ");
        const char *size = strtok(NULL, " ");

        ranges[i].start = strtoull(addr, NULL, 16);
        ranges[i].size = strtoul(size, NULL, 10);
        printf("parsed %llx-%lu\n", ranges[i].start, ranges[i].size);
        i++;
    }


    char mem_file_name[80];
    void *buf = malloc(end - start);
    if (buf == NULL) {
        perror("Could not allocate memory!");
        exit(1);
    }
    sprintf(mem_file_name, "/proc/%d/mem", pid);
    int mem_fd = open(mem_file_name, O_RDONLY);
    if (mem_fd == -1) {
        printf("Failure!\n");
        exit(1);
    }
    ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    waitpid(pid, NULL, 0);
    int ret = lseek(mem_fd, start, SEEK_SET);
    if (ret == -1) {
        perror("Could not seek to offset!");
        exit(1);
    }

    unsigned long n = (end - start);
    unsigned long r = 0;

    while (r < n) {
        int length = fmin(_SC_PAGE_SIZE, n-r);
        ret = read(mem_fd, buf+r, length);
        if (ret == -1) {
            fprintf(stderr, "Problem with %s-%s %s", argv[2], argv[3], argv[4]);
            perror("Could not read from memory!");
            exit(1);
        }
        r += ret;
    }

    ptrace(PTRACE_DETACH, pid, NULL, NULL);
    printf("Read %lu bytes\n", r);

    void *p = buf;
    int hits = 0;
    while (p < ((buf+r)-sizeof(void*))) {
        unsigned long long x = *(unsigned long long*)p;
        void *q = (void*)start + (p-buf);
        /** printf("read %p %llx\n", q, x); */
        for (struct range *r = ranges; r->start; r++) {
          /**  printf("range %llx %lu\n", r->start, r->size); */
          if (in_range(r, x)) {
              hits++;
              if (dump_addresses) {
                  fprintf(fp, "%p\n", q);
              }
              break;
          }
        }
        p++;
    }
    printf("hits %s %d\n", map, hits);
}
