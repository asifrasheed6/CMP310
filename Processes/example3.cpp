// Simple implementation of a bounded buffer
#define BUFFER_BOUND 10

struct item{
    // Attributes goes here
};

item buffer[BUFFER_BOUND]; // we have 0..BUFFER_BOUND-1 number of items
int in = 0;
int out = 0;

// Why do we need in and out?
// When the consumer takes an element from the buffer, we don't want to move the element to the front of the buffer, this is not an efficient way. So we have two indexes for the buffer that are in and out. in points to the first free slot in the buffer where the producer could put the item and out points to the first available itme that the consumer could consume.
// Keep in mind that in a bounded buffe, in and out are cyclic.
// If in==out, it is an empty buffer.
// One slot of the buffer should always be free.
