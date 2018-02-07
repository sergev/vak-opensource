#include <unistd.h>
#include <fcntl.h>
#include <sys/gpanel.h>
#include <SDL.h>

#define WIDTH   320
#define HEIGHT  240

SDL_Window *_gpanel_window;
SDL_Renderer *_gpanel_renderer;
SDL_Surface *_gpanel_surface;
SDL_Texture *_gpanel_texture;

static int _col, _row;

/*
 * Open a graphics panel device.
 */
int gpanel_open(const char *devname)
{
    /* Initialize SDL subsystems - in this case, only video. */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        return -1;
    }
    /* Register SDL_Quit to be called at exit;
     * makes sure things are cleaned up when we quit. */
    atexit(SDL_Quit);

    /* Attempt to create a window with 16 bit pixels. */
    _gpanel_window = SDL_CreateWindow("Flappy Bird",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WIDTH, HEIGHT, SDL_SWSURFACE);
    if (! _gpanel_window) {
        fprintf(stderr, "Unable to create window %ux%u: %s\n",
            WIDTH, HEIGHT, SDL_GetError());
        return -1;
    }

    _gpanel_renderer = SDL_CreateRenderer(_gpanel_window, -1, 0);
    if (! _gpanel_renderer) {
        fprintf(stderr, "Unable to create renderer: %s\n",
            SDL_GetError());
        return -1;
    }

    _gpanel_surface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT,
        16, 0xF800, 0x07E0, 0x001F, 0);
    if (! _gpanel_surface) {
        fprintf(stderr, "Unable to create 16-bit surface: %s\n",
            SDL_GetError());
        return -1;
    }

    _gpanel_texture = SDL_CreateTexture(_gpanel_renderer,
        SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    if (! _gpanel_texture) {
        fprintf(stderr, "Unable to create RGB565 texture %ux%u: %s\n",
            WIDTH, HEIGHT, SDL_GetError());
        return -1;
    }

    /* If we fail, return error. */
    return 0;
}

/*
 * Close gpanel device.
 */
void gpanel_close(void)
{
    SDL_Quit();
}

/*
 * Poll for events, and handle the ones we care about.
 */
int gpanel_input()
{
    SDL_Event event;
    int ret = 0;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            //print_key(&event);
            ret = 1;
            break;

        case SDL_KEYUP:
            //print_key(&event);
            /* If escape is pressed, terminate the program. */
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                SDL_Quit();
                exit(0);
            }
            break;

        case SDL_QUIT:
            SDL_Quit();
            exit(0);
        }
    }
    return ret;
}

void gpanel_clear(int color, int *xsize, int *ysize)
{
    gpanel_fill(color, 0, 0, WIDTH-1, HEIGHT-1);
    if (xsize)
        *xsize = WIDTH;
    if (ysize)
        *ysize = HEIGHT;
}

int gpanel_get_pixel(int x, int y)
{
    short *ptr;
    int color;

    /* Draw to screen */
    ptr = (short*) (_gpanel_surface->pixels + y * _gpanel_surface->pitch);
    color = ptr[x];

    return color;
}

void gpanel_pixel(int color, int x, int y)
{
    short *ptr;

    /* Draw to screen */
    ptr = (short*) (_gpanel_surface->pixels + y * _gpanel_surface->pitch);
    ptr[x] = color;

    /* Tell SDL to update the whole screen */
    SDL_UpdateTexture(_gpanel_texture, NULL, _gpanel_surface->pixels, _gpanel_surface->pitch);
    SDL_RenderClear(_gpanel_renderer);
    SDL_RenderCopy(_gpanel_renderer, _gpanel_texture, NULL, NULL);
    SDL_RenderPresent(_gpanel_renderer);
}

void gpanel_fill(int color, int x0, int y0, int x1, int y1)
{
    int x, y;
    short *ptr;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) x0 = 0;
    if (x1 < 0) x1 = 0;
    if (y1 < 0) x1 = 0;
    if (x0 >= WIDTH) x0 = WIDTH-1;
    if (x1 >= WIDTH) x1 = WIDTH-1;
    if (y0 >= HEIGHT) y0 = HEIGHT-1;
    if (y1 >= HEIGHT) y1 = HEIGHT-1;

    if (x1 < x0) {
        int t = x0;
        x0 = x1;
        x1 = t;
    }
    if (y1 < y0) {
        int t = y0;
        y0 = y1;
        y1 = t;
    }

    /* Draw to screen */
    for (y = y0; y <= y1; y++) {
        ptr = (short*) (_gpanel_surface->pixels + y * _gpanel_surface->pitch);
        for (x = x0; x <= x1; x++) {
            ptr[x] = color;
        }
    }

    /* Tell SDL to update the whole screen */
    SDL_UpdateTexture(_gpanel_texture, NULL, _gpanel_surface->pixels, _gpanel_surface->pitch);
    SDL_RenderClear(_gpanel_renderer);
    SDL_RenderCopy(_gpanel_renderer, _gpanel_texture, NULL, NULL);
    SDL_RenderPresent(_gpanel_renderer);
}

void gpanel_rect(int color, int x0, int y0, int x1, int y1)
{
    gpanel_fill(color, x0, y0, x1, y0);
    gpanel_fill(color, x0, y1, x1, y1);
    gpanel_fill(color, x0, y0, x0, y1);
    gpanel_fill(color, x1, y0, x1, y1);
}

/*
 * Draw a glyph of one symbol.
 */
static void draw_glyph(const struct gpanel_font_t *font,
    int color, int background, int x, int y, int width,
    const unsigned short *bits)
{
    int h, w;
    unsigned bitmask = 0;
    short *ptr;

    if (x + width > WIDTH ||  y + font->height > HEIGHT)
        return;

    if (background >= 0) {
        /*
         * Clear background.
         */
        /* Loop on each glyph row. */
        for (h=0; h<font->height; h++) {
            ptr = (short*) (_gpanel_surface->pixels + (y + h) * _gpanel_surface->pitch);

            /* Loop on every pixel in the row (left to right). */
            for (w=0; w<width; w++) {
                if ((w & 15) == 0)
                    bitmask = *bits++;
                else
                    bitmask <<= 1;

                ptr[x + w] = (bitmask & 0x8000) ? color : background;
            }
        }
    } else {
        /*
         * Transparent background.
         */
        /* Loop on each glyph row. */
        for (h=0; h<font->height; h++) {
            ptr = (short*) (_gpanel_surface->pixels + (y + h) * _gpanel_surface->pitch);

            /* Loop on every pixel in the row (left to right). */
            for (w=0; w<width; w++) {
                if ((w & 15) == 0)
                    bitmask = *bits++;
                else
                    bitmask <<= 1;

                if (bitmask & 0x8000) {
                    ptr[x + w] = color;
                }
            }
        }
    }

    /* Tell SDL to update the whole screen */
    SDL_UpdateTexture(_gpanel_texture, NULL, _gpanel_surface->pixels, _gpanel_surface->pitch);
    SDL_RenderClear(_gpanel_renderer);
    SDL_RenderCopy(_gpanel_renderer, _gpanel_texture, NULL, NULL);
    SDL_RenderPresent(_gpanel_renderer);
}

/*
 * Draw a character from a specified font.
 */
static void draw_char(const struct gpanel_font_t *font, int color, int background, int sym)
{
    unsigned cindex, width;
    const unsigned short *bits;

    switch (sym) {
    case '\n':      /* goto next line */
        _row += font->height;
        _col = 0;
        if (_row > HEIGHT - font->height)
            _row = 0;
        return;
    case '\r':      /* carriage return - go to begin of line */
        _col = 0;
        return;
    case '\t':      /* tab replaced by space */
        sym = ' ';
        break;
    }

    if (sym < font->firstchar || sym >= font->firstchar + font->size)
        sym = font->defaultchar;
    cindex = sym - font->firstchar;

    /* Get font bitmap depending on fixed pitch or not. */
    if (font->width) {
        /* Proportional font. */
        width = font->width[cindex];
    } else {
        /* Fixed width font. */
        width = font->maxwidth;
    }
    if (font->offset) {
        bits = font->bits + font->offset[cindex];
    } else {
        bits = font->bits + cindex * font->height;
    }

    /* Draw a character. */
    draw_glyph(font, color, background, _col, _row, width, bits);
    _col += width;
}

void gpanel_char(const struct gpanel_font_t *font, int color, int background,
    int x, int y, int sym)
{
    _col = x;
    _row = y;
    draw_char(font, color, background, sym);
}

void gpanel_text(const struct gpanel_font_t *font, int color, int background,
    int x, int y, const char *text)
{
    int sym;

    _col = x;
    _row = y;
    for (;;) {
        sym = *text++;
        if (! sym)
            break;

        draw_char(font, color, background, sym);
    }
}

/*
 * Swap values of two integer variables.
 */
#define swapi(x,y) { int _t = x; x = y; y = _t; }

void gpanel_fill_triangle(int color, int x0, int y0,
    int x1, int y1, int x2, int y2)
{
    int a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        swapi(y0, y1);
        swapi(x0, x1);
    }
    if (y1 > y2) {
        swapi(y2, y1);
        swapi(x2, x1);
    }
    if (y0 > y1) {
        swapi(y0, y1);
        swapi(x0, x1);
    }

    if (y0 == y2) {
        // Handle awkward all-on-same-line case as its own thing
        a = b = x0;

        if (x1 < a)
            a = x1;
        else if (x1 > b)
            b = x1;

        if (x2 < a)
            a = x2;
        else if (x2 > b)
            b = x2;

        gpanel_fill(color, a, y0, b, y0);
        return;
    }

    int dx01 = x1 - x0;
    int dy01 = y1 - y0;
    int dx02 = x2 - x0;
    int dy02 = y2 - y0;
    int dx12 = x2 - x1;
    int dy12 = y2 - y1;
    int sa = 0;
    int sb = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if (y1 == y2)
        last = y1;      // Include y1 scanline
    else
        last = y1-1;    // Skip it

    for (y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;

        /* longhand:
         * a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
         * b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
         */
        if (a > b)
            swapi(a, b);

        gpanel_fill(color, a, y, b, y);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for (; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;

        /* longhand:
         * a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
         * b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
         */
        if (a > b)
            swapi(a, b);

        gpanel_fill(color, a, y, b, y);
    }
}
