#include <stdio.h>

struct range {
   unsigned long long start;
   unsigned long size;
};

#define MAXRANGE 100 

static struct range ranges[MAXRANGE];

int in_range(struct range *r, unsigned long long addr);
