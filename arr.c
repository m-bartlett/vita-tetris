#include <stdio.h>

unsigned char A[10][10]={0};

const unsigned char (*playfield_view())[10] {
    return (const unsigned char (*)[10])A;
}
    

int main() {
    A[0][0]=69;
    A[1][0]=69;
    A[0][1]=69;
    A[1][1]=69;

    const unsigned char (*view)[10] = playfield_view();
    for (size_t j = 0; j < 2; j++) {
        for (size_t i = 0; i < 10; i++) {
            printf("%d ", view[j][i]);
        }
    }

    printf("\n");
    printf("%d\n", view[0][0]);
    printf("%d\n", view[1][0]);
    printf("%d\n", view[0][1]);
    printf("%d\n", view[1][1]);
    printf("\n");
}