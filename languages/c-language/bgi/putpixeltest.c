#include <assert.h>
#include <graphics.h>

void mybar(int x, int y, int w, int h, int c)
{
    int i, j;
    for (i = y; i <= y + h; i++) {
        for (j = x; j <= x + h; j++) {
            putpixel(j, i, c);
        }
    }
}

int main()
{
    int gd = DETECT, gm = 0;
    initgraph(&gd, &gm, "");
    outtext("8-bit mode test");
    putpixel(0, 0, GREEN);
    putpixel(getmaxx(), 0, BLUE);
    putpixel(0, getmaxy(), WHITE);
    putpixel(getmaxx(), getmaxy(), YELLOW);
    putpixel(getmaxx() / 2, getmaxy() / 2, BROWN);
    assert(getpixel(0, 0) == GREEN);
    assert(getpixel(getmaxx(), 0) == BLUE);
    assert(getpixel(0, getmaxy()) == WHITE);
    assert(getpixel(getmaxx(), getmaxy()) == YELLOW);
    assert(getpixel(getmaxx() / 2, getmaxy() / 2) == BROWN);
    readkey();
    closegraph();
    initgraph(&gd, &gm, "RGB");
    outtext("RGB mode test");
    mybar(0, 10, 10, 10, RED);
    mybar(10, 10, 10, 10, GREEN);
    mybar(20, 10, 10, 10, BLUE);

    putpixel(0, 0, GREEN);
    putpixel(getmaxx(), 0, BLUE);
    putpixel(0, getmaxy(), WHITE);
    putpixel(getmaxx(), getmaxy(), YELLOW);
    putpixel(getmaxx() / 2, getmaxy() / 2, BROWN);
    assert(getpixel(0, 0) == GREEN);
    assert(getpixel(getmaxx(), 0) == BLUE);
    assert(getpixel(0, getmaxy()) == WHITE);
    assert(getpixel(getmaxx(), getmaxy()) == YELLOW);
    assert(getpixel(getmaxx() / 2, getmaxy() / 2) == BROWN);
    readkey();
    closegraph();
}