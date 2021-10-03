// Computer Systems Architecture Assignment
// Cache Simulator Program
// Year 3
// 2019 - 2020

// Filename: Alkinoos_Sarioglou_10136315_Cache_Simulator.c
// Author: Alkinoos Sarioglou
// Student ID: 10136315
// Date: 12 March 2020

// Libraries to include
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>    // Used for the log function

// Cache Function prototype, Arguments include: the trace file name and the ID number of the cache configuration mode
void cache_analysis(char str[100],unsigned int mode_id);

// Global variables declaration
// Variables extracted from a line of trace file
int elements=0;         // Elements returned by fscanf function
char c;                 // Variable to save 'R' for Read or 'W' for Write
int addr;               // Address to be accessed
int block_size;         // Cache Block Size (in 16-bit words)
int no_cache_lines;     // Number of cache lines
int block_offset_bits;  // Block offset bits of the address according to the mode
FILE *fp_read;          // File Pointer to Read Trace File
FILE *fp_write;         // File Pointer to Write .csv Results File

// Performance Metrics
unsigned int nra=0;
unsigned int nwa=0;
unsigned int ncrh=0;
unsigned int ncrm=0;
unsigned int ncwh=0;
unsigned int ncwm=0;
unsigned int no_reads=0;
unsigned int no_writes=0;

// Main Function
int main (void) {

    // Clear Results File
    fp_write = fopen ("results.csv", "w");
    if (fp_write == NULL) printf ("Error with the results file");


    // Bubble Sort Program run in All Modes one after the other
    cache_analysis("bubble_sort_trace_027.trc",1);              // Mode 1
    cache_analysis("bubble_sort_trace_027.trc",2);              // Mode 2
    cache_analysis("bubble_sort_trace_027.trc",3);              // Mode 3
    cache_analysis("bubble_sort_trace_027.trc",4);              // Mode 4
    cache_analysis("bubble_sort_trace_027.trc",5);              // Mode 5
    cache_analysis("bubble_sort_trace_027.trc",6);              // Mode 6
    cache_analysis("bubble_sort_trace_027.trc",7);              // Mode 7
    cache_analysis("bubble_sort_trace_027.trc",8);              // Mode 8


    // Cross Correlation Program run in All Modes one after the other
    cache_analysis("cross_correlation_trace_027.trc",1);        // Mode 1
    cache_analysis("cross_correlation_trace_027.trc",2);        // Mode 2
    cache_analysis("cross_correlation_trace_027.trc",3);        // Mode 3
    cache_analysis("cross_correlation_trace_027.trc",4);        // Mode 4
    cache_analysis("cross_correlation_trace_027.trc",5);        // Mode 5
    cache_analysis("cross_correlation_trace_027.trc",6);        // Mode 6
    cache_analysis("cross_correlation_trace_027.trc",7);        // Mode 7
    cache_analysis("cross_correlation_trace_027.trc",8);        // Mode 8

    return 0;
} // End of Main Function

// Cache Function definition
void cache_analysis(char str[100],unsigned int mode_id) {

    // Reset Performance Metrics
    nra=0;
    nwa=0;
    ncrh=0;
    ncrm=0;
    ncwh=0;
    ncwm=0;
    no_reads=0;   // Overall Number of Reads
    no_writes=0;  // Overall Number of Writes

    // Switch statement to determine the Block Size according to the Mode ID
    switch (mode_id)
    {
        case 1: block_size = 1; break;
        case 2: block_size = 2; break;
        case 3: block_size = 4; break;
        case 4: block_size = 8; break;
        case 5: block_size = 16; break;
        case 6: block_size = 32; break;
        case 7: block_size = 64; break;
        case 8: block_size = 128; break;
        default: printf("Invalid Mode ID! Please enter a value between 1 and 8.");

    }

    // Calculate Number of Cache Lines
    no_cache_lines = 256/block_size;

    // Calculate Number of Block Offset Bits for the specific Block Size
    block_offset_bits = (int) (log(block_size)/log(2));  // Formula is log2(block_size), but perform change-of-base conversion to base e because no function for log with base 2

    // Declare the cache array
    uint16_t cache[no_cache_lines];
    memset(cache, 0, 2*no_cache_lines);    // Initialize the variable-length array with zeros - we specify the number of bytes to be initialized to zero and this is = 2*no_cache_lines, because each cache line contains 16-bit values (2 bytes), that is why we have 2*no_cache_lines

    // Declare the valid bit array
    bool valid_bit[no_cache_lines];
    memset(valid_bit, 0, 2*no_cache_lines);    // Initialize the variable-length array with zeros - we specify the number of bytes to be initialized to zero and this is = 2*no_cache_lines, because each cache line contains 16-bit values (2 bytes), that is why we have 2*no_cache_lines

    // Declare the dirty bit array
    bool dirty_bit[no_cache_lines];
    memset(dirty_bit, 0, 2*no_cache_lines);    // Initialize the variable-length array with zeros - we specify the number of bytes to be initialized to zero and this is = 2*no_cache_lines, because each cache line contains 16-bit values (2 bytes), that is why we have 2*no_cache_lines

    int count=0;                               // Used for comparisons in the cache
    int round_robin_index=0;                   // Used for implementation of FIFO replacement policy

    // Open Results file to save the results into it (append mode to save all data without overwriting)
    fp_write = fopen ("results.csv", "a");
    if (fp_write == NULL) printf ("Error with the results file");

    // Open Bubble Sort Trace file
    fp_read = fopen(str, "r");
    if (fp_read == NULL) printf ("Trace File does not exist");
    rewind(fp_read);           // Sets the position of the pointer at the beginning of the file in order to read memory accesses from the beginning

    // Loop to deal with each memory access
    while (1) {

        // Scan the memory access for WRITE/READ and ADDRESS
        elements = fscanf(fp_read, "%c %x ", &c, &addr);
        addr = addr>>block_offset_bits;     // Keep the TAG bits by shifting appropriate places to the right according to mode ID (removing the bits of Block Offset)

        if (elements<0) {
            break;          // If the end of the program is reached exit the loop
        }

        // Read Operation
        else if (c=='R') {
            no_reads++;    // Increment number of reads
            for (int i=0;i<no_cache_lines;i++) {

                    if (valid_bit[i] == false) {
                        count++;                   // If data in cache line not valid, do not compare
                        continue;
                    }


                    else if (cache[i] == addr && valid_bit[i] == true) {  // Cache Read Hit
                            ncrh++;    // Increment Read Hit variable
                            break;
                    }

                    else count++;

            }

            if (count==no_cache_lines){      // Cache Read Miss
                ncrm++;           // Increment Read Miss variable

                // FIFO replacement policy
                if (dirty_bit[round_robin_index]==1) {      // Write block to replace to memory if something was written to it
                    nwa+=block_size;                        // Increment Write Accesses by the block size
                    dirty_bit[round_robin_index]=0;         // Clear Dirty bit
                    cache[round_robin_index] = addr;        // Replace block
                    valid_bit[round_robin_index++] = true;  // Raise Valid bit
                    if (round_robin_index == no_cache_lines) { // Loop around if end of cache is reached
                        round_robin_index = 0;
                    }
                    nra+=block_size;            // Increment Number of Read Accesses to Main Memory by the block size
                }
                // If dirty bit is LOW
                else {
                    cache[round_robin_index] = addr; // Save Block into cache
                    valid_bit[round_robin_index++] = true;  // Raise Valid bit
                    if (round_robin_index == no_cache_lines) { // Loop around if end of cache is reached
                        round_robin_index = 0;
                    }
                    nra+=block_size;            // Increment Number of Read Accesses to Main Memory by the block size
                }

            }

            count=0;                            // Clear "count" variable for next address comparison

        }

        // Write Operation
        else if (c=='W'){
            no_writes++;        // Increment number of writes
            for (int k=0;k<no_cache_lines;k++) {

                    if (valid_bit[k] == false) {
                        count++;    // If data in cache line not valid, do not compare
                        continue;
                    }

                    else if (cache[k] == addr && valid_bit[k]==true) {
                        ncwh++;                         // Cache Read Hit
                        dirty_bit[k] = 1;               // Raise 'Dirty Bit' because writing to the block
                        break;
                    }

                    else count++;

            }

            if (count==no_cache_lines){      // Cache Read Miss
                ncwm++;           // Increment Read Miss variable
                if (dirty_bit[round_robin_index]==1) {      // Write block to replace to memory if something was written to it
                    nwa+=block_size;                        // Increment Write Accesses
                    cache[round_robin_index] = addr;        // Replace block
                    dirty_bit[round_robin_index]=1;         // Dirty Bit stays HIGH because of Write operation
                    valid_bit[round_robin_index++] = true;  // Raise Valid bit
                    if (round_robin_index == no_cache_lines) { // Loop around if end of cache is reached
                        round_robin_index = 0;
                    }
                    nra+=block_size;            // Increment Number of Read Accesses to Main Memory by the Block Size
                }
                else {
                    cache[round_robin_index] = addr; // Save Block into cache
                    dirty_bit[round_robin_index]=1;  // Dirty Bit stays HIGH because of Write operation
                    valid_bit[round_robin_index++] = true; // Raise Valid bit
                    if (round_robin_index == no_cache_lines) { // Loop around if end of cache is reached
                        round_robin_index = 0;
                    }
                    nra+=block_size;            // Increment Number of Read Accesses to Main Memory by the block size
                }

            }

            count=0;                           // Clear "count" variable for next address comparison

        }


    }

    printf("\n %s, %d, %d, %d, %d, %d, %d, %d", str, mode_id, nra, nwa, ncrh, ncrm, ncwh, ncwm);    // Print results to the terminal window

    fprintf(fp_write, "%s, %d, %d, %d, %d, %d, %d, %d\n", str, mode_id, nra, nwa, ncrh, ncrm, ncwh, ncwm);  // Write results to the "results.csv" file

    fclose(fp_read);    // Close Trace File

    fclose(fp_write);   // Close results File

} // End of Cache Function
