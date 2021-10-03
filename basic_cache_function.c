// Cache simulator program

// Alkinoos Sarioglou
// 10136315

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>    // Used for the log function

void basic_cache_function(void);

// Global variables declaration
// Variables extracted from a line of trace file
int elements=0;
char c;
int addr;
int block_size;
int no_cache_lines;
int block_offset_bits;
FILE *fp;

// Performance Metrics
unsigned int nra=0;
unsigned int nwa=0;
unsigned int ncrh=0;
unsigned int ncrm=0;
unsigned int ncwh=0;
unsigned int ncwm=0;
unsigned int no_reads=0;
unsigned int no_writes=0;


int main (void) {

    basic_cache_function();


    return 0;
}


void basic_cache_function(void) {

    // Declare the cache
    uint16_t cache_mode_1[256] = {0};
    bool valid_bit[256] = {false};
    bool dirty_bit[256] = {false};
    int count=0;
    int round_robin_index=0;

    // Open Bubble Sort Trace file
    fp = fopen("test_file.trc", "r");
    if (fp == NULL) printf ("Bubble Sort Trace File does not exist");
    rewind(fp);

    // Loop to deal with each memory access
    while (1) {

        // Scan the memory access for WRITE/READ and ADDRESS
        elements = fscanf(fp, "%c %x ", &c, &addr);
        if (elements<0) {
            break;          // If the end of the program is reached exit the loop
        }

        // Read Operation
        else if (c=='R') {
            //printf("Read ");
            //printf("Address: %d\n", addr);
            no_reads++;
            for (int i=0;i<256;i++) {

                    // Add if statement to test whether valid_bit is false
                    // If valid bit false then continue
                    // and count++ (!!)


                    if (cache_mode_1[i] == addr && valid_bit[i]==true) {
                        ncrh++;                        // Cache Read Hit
                        break;
                    }

                    else count++;

            }

            if (count==256){      // Cache Read Miss
                ncrm++;           // Increment Read Miss variable
                if (dirty_bit[round_robin_index]==1) {    // Replacement
                    nwa++;
                    dirty_bit[round_robin_index]=0;
                    cache_mode_1[round_robin_index] = addr;
                    valid_bit[round_robin_index++] = true;
                    if (round_robin_index == 256) {
                        round_robin_index = 0;
                    }
                    nra++;            // Increment Number of Read Accesses to Main Memory
                }
                else {
                    cache_mode_1[round_robin_index] = addr; // Access Main Memory and save contents in cache
                    valid_bit[round_robin_index++] = true;
                    if (round_robin_index == 256) {
                        round_robin_index = 0;
                    }
                    nra++;            // Increment Number of Read Accesses to Main Memory
                }

            }

            count=0;

        }

        // Write Operation
        else if (c=='W'){
            //printf("Write ");
            //printf("Address: %d\n", addr);
            no_writes++;
            for (int k=0;k<256;k++) {


                    if (cache_mode_1[k] == addr && valid_bit[k]==true) {
                        ncwh++;                        // Cache Read Hit
                        dirty_bit[k] = 1;
                        break;
                    }

                    else count++;

            }

            if (count==256){      // Cache Read Miss
                ncwm++;           // Increment Read Miss variable
                if (dirty_bit[round_robin_index]==1) {
                    nwa++;
                    cache_mode_1[round_robin_index] = addr;
                    dirty_bit[round_robin_index]=1;  // Dirty Bit because of Write operation
                    valid_bit[round_robin_index++] = true;
                    if (round_robin_index == 256) {
                        round_robin_index = 0;
                    }
                    nra++;            // Increment Number of Read Accesses to Main Memory
                }
                else {
                    cache_mode_1[round_robin_index] = addr; // Access Main Memory and save contents in cache
                    dirty_bit[round_robin_index]=1;  // Dirty Bit because of Write operation
                    valid_bit[round_robin_index++] = true;
                    if (round_robin_index == 256) {
                        round_robin_index = 0;
                    }
                    nra++;            // Increment Number of Read Accesses to Main Memory
                }

            }

            count=0;

        }


    }

    printf("\nTest file, Mode 1, NRA = %d, NWA = %d, NCRH = %d, NCRM = %d, NCWH = %d, NCWM = %d, Reads = %d, Writes = %d\n", nra, nwa, ncrh, ncrm, ncwh, ncwm, no_reads, no_writes);

    fclose(fp);


}
