#include <stdalign.h>
#include <stddef.h>


// Ensures compiler/cpu are happy that the address is a multiple of max_align_t
alignas(max_align_t) static unsigned char arena[1024 * 1024];
static size_t currIndex = 0;

// Ensures compiler/cpu are happy that the address is a multiple of the type.
// Given 0th index of arena is aligned, just align type with respect to index
#define next_highest_multiple(index, alignment) ((((index) + (alignment) - 1) / (alignment)) * (alignment))

void* arena_allocate(size_t numBytes, size_t alignment){

    size_t alignedIndex = next_highest_multiple(currIndex, alignment);

    if (alignedIndex + numBytes > sizeof(arena)) return NULL;

    void *ptr = (void *)&arena[alignedIndex];
    currIndex = alignedIndex + numBytes;
    return ptr;

}

void free_arena(){
    currIndex = 0;
}
