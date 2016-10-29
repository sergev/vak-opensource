#include "cube.h"
#include "math.h"

static void line_3d(int x1, int y1, int z1, int x2, int y2, int z2)
{
    int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc,
        err_1, err_2, dx2, dy2, dz2;
    int pixel[3];

    pixel[0] = x1;
    pixel[1] = y1;
    pixel[2] = z1;
    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;
    if (dx >= 0) {
        x_inc = 1;
        l = dx;
    } else {
        x_inc = -1;
        l = -dx;
    }
    if (dy >= 0) {
        y_inc = 1;
        m = dy;
    } else {
        y_inc = -1;
        m = -dy;
    }
    if (dz >= 0) {
        z_inc = 1;
        n = dz;
    } else {
        z_inc = -1;
        n = -dz;
    }
    dx2 = l << 1;
    dy2 = m << 1;
    dz2 = n << 1;
    if ((l >= m) && (l >= n)) {
        err_1 = dy2 - l;
        err_2 = dz2 - l;
        for (i = 0; i < l; i++) {
            setvoxel(pixel[0], pixel[1], pixel[2]);

            if (err_1 > 0) {
                pixel[1] += y_inc;
                err_1 -= dx2;
            }
            if (err_2 > 0) {
                pixel[2] += z_inc;
                err_2 -= dx2;
            }
            err_1 += dy2;
            err_2 += dz2;
            pixel[0] += x_inc;
        }
    } else if ((m >= l) && (m >= n)) {
        err_1 = dx2 - m;
        err_2 = dz2 - m;
        for (i = 0; i < m; i++) {
            setvoxel(pixel[0], pixel[1], pixel[2]);

            if (err_1 > 0) {
                pixel[0] += x_inc;
                err_1 -= dy2;
            }
            if (err_2 > 0) {
                pixel[2] += z_inc;
                err_2 -= dy2;
            }
            err_1 += dx2;
            err_2 += dz2;
            pixel[1] += y_inc;
        }
    } else {
        err_1 = dy2 - n;
        err_2 = dx2 - n;
        for (i = 0; i < n; i++) {
            setvoxel(pixel[0], pixel[1], pixel[2]);
            if (err_1 > 0) {
                pixel[1] += y_inc;
                err_1 -= dz2;
            }
            if (err_2 > 0) {
                pixel[0] += x_inc;
                err_2 -= dz2;
            }
            err_1 += dy2;
            err_2 += dx2;
            pixel[2] += z_inc;
        }
    }
    setvoxel(pixel[0], pixel[1], pixel[2]);
}

void effect_sinelines(int seconds, int delay)
{
    int iterations = seconds * 1000 / delay;
    int i, x;

    float left, right, sine_base, x_dividor, ripple_height;

    for (i=0; i<iterations && !sensor_active; i++) {
        for (x=0; x<8; x++) {
            x_dividor = 2 + sin((float)i / 100) + 1;
            ripple_height = 3 + (sin((float)i / 200) + 1) * 6;

            sine_base = (float) i/40 + (float) x/x_dividor;

            left = 4 + sin(sine_base) * ripple_height;
            right = 4 + cos(sine_base) * ripple_height;
            right = 7-left;

            line_3d(0-3, x, (int) left, 7+3, x, (int) right);
        }

        delay_ms(delay);
        fill(0);
    }
}

void effect_linespin(int seconds, int delay)
{
    float top_x, top_y, top_z, bot_x, bot_y, bot_z, sin_base;
    float center_x, center_y;
    int i, z;
    int iterations = seconds * 1000 / delay;

    center_x = 4;
    center_y = 4;

    for (i=0; i<iterations && !sensor_active; i++) {

        for (z = 0; z < 8; z++) {

            sin_base = (float)i/50 + (float)z/(10 + (7*sin((float)i / 200)));

            top_x = center_x + sin(sin_base) * 5;
            top_y = center_y + cos(sin_base) * 5;

            bot_x = center_x + sin(sin_base+3.14) * 10;
            bot_y = center_y + cos(sin_base+3.14) * 10;

            bot_z = z;
            top_z = z;

            line_3d((int) top_z, (int) top_x, (int) top_y, (int) bot_z, (int) bot_x, (int) bot_y);
        }

        delay_ms(delay);
        fill(0);
    }
}
