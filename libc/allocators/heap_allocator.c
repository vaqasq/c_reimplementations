#include <stddef.h>
#include <stdalign.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

/* 
 * I do work to keep alignments. Every sbrk is aligned, the structs
 * are padded to be aligned such that the returned void pointer is aligned.
 * Even the free list when combined will still be aligned
 */

// Could use max_align_t aswell. 16 just works
#define ALIGNMENT 16

// Use to round sizes requested and addresses
#define next_highest_aligned_multiple(index) ((index + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT

// This is where the first node lives
static void* heap_start = NULL;

// Node definitions
typedef struct block_descriptor{
    struct block_descriptor* next;
    bool occupied;
    size_t size;
    // Adding padding allows me to simply return (void*)(metadata_node + 1) to the user
    char padding[(ALIGNMENT - (sizeof(struct block_descriptor*) + sizeof(bool) + sizeof(size_t)) % ALIGNMENT) % ALIGNMENT];
} __attribute__((packed)) block_descriptor;
// __attribute__ tells the compiler to not add extra padding to the specific
// values. It would still work regardless, it just saves memory

void* heap_allocate(size_t size_requested){

    // Implementation-defined
    if (size_requested == 0){
        return NULL;
    }

    void* user_ptr = NULL;

    // First allocation is a special case
    if (heap_start == NULL){

        // Need to round up initial address to a multiple of 16
        heap_start = sbrk(0);
        if (heap_start == (void *)-1) return NULL;

        // uintptr_t and intptr_t allow me to arithmetic directly on the memory
        // address, as they are guarenteed to be as big as needed to not lose data
        uintptr_t original_addr = (uintptr_t)heap_start;
        uintptr_t aligned_address = next_highest_aligned_multiple(original_addr);
        intptr_t gap = (intptr_t)(aligned_address - original_addr);
        heap_start = (void *)aligned_address;  

        // Moves up heap end so heap_start is an accessible address
        if (sbrk(gap) == (void *)-1) return NULL;

        // Ensures next node will be aligned. Means more memory is returned to
        // user than request, which is normal

        size_t total = next_highest_aligned_multiple(size_requested + sizeof(block_descriptor));

        sbrk(total);

        // Add first node
        block_descriptor first_bd  = {NULL, true, total};
        memcpy(heap_start, &first_bd, sizeof(block_descriptor));

        // char is 1 byte, otherwise I could just use uintptr_t
        user_ptr = (void *)((char*)(heap_start) + sizeof(block_descriptor));
        return user_ptr;

    } 

    // General case
    
    size_t total = next_highest_aligned_multiple(size_requested + sizeof(block_descriptor));
    
    // search for reusable blocks 
    block_descriptor* starting_block = (block_descriptor*)heap_start;
    while(starting_block != NULL){
        
        if (starting_block->occupied == false && starting_block->size >= total){
            // allocate in this block

            starting_block->occupied = true;
            return (void*)((char *)(starting_block) + sizeof(block_descriptor));

        }

        // otherwise 

        starting_block = starting_block->next;
        
    }


    // Couldn't find reusable block

    void* new_allocation_start = sbrk(total);
    if (new_allocation_start == (void *)-1) return NULL;

    block_descriptor new_bd = {NULL, true, total};
    memcpy(new_allocation_start, &new_bd, sizeof(block_descriptor));

    // connect old bd to this one, then return user_ptr

    block_descriptor* fbd = (block_descriptor *)heap_start;
    while (fbd->next != NULL) fbd = fbd->next;
    fbd->next = (block_descriptor *)new_allocation_start;


    user_ptr = (void *)((char *)(new_allocation_start) + sizeof(block_descriptor));
    return user_ptr;

}

// first connect old bd to new one in allocate and then move here
void heap_free(void *ptr){

    block_descriptor* metadata_node = (block_descriptor *)((char *)(ptr) - sizeof(block_descriptor));

    metadata_node->occupied = false;

    // Check after, then before

    // Merge if the next one is also unoccupied

    // Check metadata_node isn't the last
    if (metadata_node->next != NULL){
        if (metadata_node->next->occupied == false){
            metadata_node->size += metadata_node->next->size;
            metadata_node->next = metadata_node->next->next;
        }
    }

    // Merge if the previous is unoccupied
    block_descriptor* previous_block_descriptor = (block_descriptor *)heap_start;

    // Check metadata_node isn't the first
    if (previous_block_descriptor != metadata_node){
        while (previous_block_descriptor->next != metadata_node) previous_block_descriptor = previous_block_descriptor->next;

        if (previous_block_descriptor->occupied == false){
            previous_block_descriptor-> size += previous_block_descriptor->next->size;
            previous_block_descriptor->next = previous_block_descriptor->next->next;
        }
    }

}
