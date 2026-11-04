#include <stdalign.h>
#include <stddef.h>


// Ensures compiler/cpu are happy that the address is a multiple of max_align_t
alignas(max_align_t) static unsigned char arena[1024 * 1024];
static int currIndex = 0;

#define largestIndex 1024*1024-1
// Ensures compiler/cpu are happy that the address is a multiple of the type.
// Given 0th index of arena is aligned, just align type with respect to index
#define next_highest_multiple(index, alignment) ((index + alignment - 1) / alignment) * alignment;

void* arena_allocate(size_t numBytes, size_t alignment){

    int newIndex = next_highest_multiple(currIndex + numBytes, alignment);

    // Check if there is no room left
    if (newIndex > largestIndex) return NULL;

    int userIndex = currIndex;
    currIndex = newIndex;
    
    // Otherwise give them the memory
    return (void *) &arena[userIndex];

}

void free_arena(void *ptr){
    currIndex = 0;
}
