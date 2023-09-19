#include <vitasdk.h>
#include <string.h>

static inline unsigned int get_random_sample(void) {
    unsigned int sample;
    sceKernelGetRandomNumber(&sample, sizeof(sample));
    return sample;
}


void shuffle(void *buffer, const size_t element_size, const size_t buffer_total_size) {
    const size_t buffer_element_size = buffer_total_size/element_size;
    unsigned char temp[element_size];
    for (int i = 0; i < buffer_element_size; ++i) {
        size_t A = get_random_sample()%buffer_element_size,
               B = get_random_sample()%buffer_element_size;
        memcpy(temp, buffer+(B*element_size), element_size);
        memcpy(buffer+(B*element_size), buffer+(A*element_size), element_size);
        memcpy(buffer+(A*element_size), temp, element_size);
    }
}