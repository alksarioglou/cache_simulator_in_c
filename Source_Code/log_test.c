// Cache simulator programme

// Alkinoos Sarioglou
// 10136315

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

int main () {

    int block_size = 2;
    int block_offset = log(block_size)/log(2);

    int no_cache_lines =256/block_size;

    uint16_t cache_mode_2[no_cache_lines];
    memset(cache_mode_2, 0, 2*no_cache_lines);

    for (int i=0;i<no_cache_lines;i++){

        printf("%d\n", cache_mode_2[i]);

    }


    return 0;

}
