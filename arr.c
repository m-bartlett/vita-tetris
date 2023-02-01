#include <stdio.h>

#define PLAYFIELD_WIDTH 10
#define PLAYFIELD_HEIGHT 20

typedef const unsigned char (*playfield_view_t)[PLAYFIELD_WIDTH];

unsigned char A[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH]={0};

playfield_view_t playfield_view() {
    return (playfield_view_t)A;
}
    

int main() {
    A[0][0]=69;
    A[1][0]=69;
    A[0][1]=69;
    A[1][1]=60;

    playfield_view_t view = playfield_view();
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