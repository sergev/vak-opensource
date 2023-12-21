#include <graphics.h>
#include <math.h>

#define PARTICLE_NUMBER 35
#define PARTICLE_MASS 1
#define PARTICLE_RADIUS 2
#define TIME_DELTA .001
#define LEN_DJ 1
#define EPS 1e4
#define NORM (getmaxx() / 20)

int orgX = 0, orgY = 0;

typedef struct tagVECTOR {
    double x, y;
} VECTOR;

typedef struct tagPARTICLE {
    VECTOR position;
    VECTOR speed;
} PARTICLE;

PARTICLE particles[PARTICLE_NUMBER];  // main array;
PARTICLE particles2[PARTICLE_NUMBER]; // second, temporary array

double sqr(double val)
{
    return val * val;
}

double power(double x, int y)
{
    double res;
    x /= NORM;
    res = x;

    for (; y > 1; --y) {
        res *= x;
    }

    return (1 / res);
}

// double Power (double x, float y)
//{
//	return EXR(LOG(x)* EXP(LOG(y)));
// }

void calcInteraction(PARTICLE *active, PARTICLE *passive, VECTOR *accel)
{
    // accel->x = -(active->position.x - passive->position.x) / 10;
    // accel->y = -(active->position.y - passive->position.y) / 10;
    double dx = active->position.x - passive->position.x;
    double dy = active->position.y - passive->position.y;
    double dist = sqrt(sqr(dx) + sqr(dy));
    double c = dx / dist;
    double s = dy / dist;
    double a = EPS * LEN_DJ * (power(dist, 13) - power(dist, 7)) / PARTICLE_MASS;

    accel->y = 0;
    accel->x = a * c;
    accel->y = a * s;
}

void vectorInit(VECTOR *vec, double x, double y)
{
    vec->x = x;
    vec->y = y;
}

void vectorAdd(VECTOR *dest, VECTOR *toAdd)
{
    dest->x += toAdd->x;
    dest->y += toAdd->y;
}

void updateParticle(PARTICLE *source, PARTICLE *dest, VECTOR *accel)
{
    source->speed.x += accel->x * TIME_DELTA;
    source->speed.y += accel->y * TIME_DELTA;
    *dest = *source;
    dest->position.x += source->speed.x * TIME_DELTA;
    dest->position.y += source->speed.y * TIME_DELTA;
}

void updateParticles(PARTICLE *ps, PARTICLE *tmp)
{
    int i, j;
    VECTOR accel;
    VECTOR summAccel;
    memcpy(tmp, ps, sizeof(PARTICLE) * PARTICLE_NUMBER);
    for (i = 0; i != PARTICLE_NUMBER; i++) {
        vectorInit(&summAccel, 0, 0);
        for (j = 0; j != PARTICLE_NUMBER; j++) {
            if (i == j)
                continue;
            calcInteraction(ps + i, ps + j, &accel);
            vectorAdd(&summAccel, &accel);
        }
        updateParticle(ps + i, tmp + i, &summAccel);
    }
    memcpy(ps, tmp, sizeof(PARTICLE) * PARTICLE_NUMBER);
}

void drawParticles(PARTICLE *ps)
{
    int i;
    int w = getmaxx() / 2;
    int h = getmaxy() / 2;
    for (i = 0; i != PARTICLE_NUMBER; i++) {
        setcolor((i % getmaxcolor()) + 1);
        circle(w + ps[i].position.x + orgX, h - ps[i].position.y - orgY, PARTICLE_RADIUS);
    }
}

int intersectsAny(PARTICLE *p, int n)
{
    int i;
    for (i = 0; i != n; i++) {
        if ((sqr(p->position.x - particles[i].position.x)) +
                sqr(p->position.y - particles[i].position.y) <
            sqr(4 * PARTICLE_RADIUS)) {
            return 1;
        }
    }
    return 0;
}

void initParticle(PARTICLE *p, int n)
{
    const int maxSpeed = 500;
    do {
        p->position.x =
            PARTICLE_RADIUS + (rand() % (getmaxx() - 2 * PARTICLE_RADIUS)) - getmaxx() / 2;
        p->position.y =
            PARTICLE_RADIUS + (rand() % (getmaxy() - 2 * PARTICLE_RADIUS)) - getmaxy() / 2;
    } while (intersectsAny(p, n));
    p->speed.x = (rand() % maxSpeed) - maxSpeed / 2;
    p->speed.y = (rand() % maxSpeed) - maxSpeed / 2;
}

void initParticles(PARTICLE *ps)
{
    int i;

    srand(clock());

    for (i = 0; i != PARTICLE_NUMBER; i++) {
        initParticle(ps + i, i);
        /*if (ps->position.x == (ps + i)->position.x ||
                ps->position.y == (ps + i)->position.y ||
                ps->speed.x == (ps + i)->speed.x ||
                ps->speed.y == (ps + i)->speed.y)*/
    }
}

int main(void)
{
    // int gd = CUSTOM, gm = CUSTOM_MODE(1024,768);
    int gd = 0, gm = 0;
    int page = 0;
    int old_x, old_y;
    int scrolling = 0;

    initgraph(&gd, &gm, "");

    initParticles(particles);

    while (!xkbhit()) {
        if (!ismouseclick(WM_LBUTTONDOWN)) {
            scrolling = 0;
        } else {
            if (scrolling == 0) {
                scrolling = 1;
            } else {
                orgX -= -mousex() + old_x;
                orgY += -mousey() + old_y;
            }
            old_x = mousex();
            old_y = mousey();
        }
        updateParticles(particles, particles2);
        setactivepage(1 - page);
        clearviewport();
        drawParticles(particles);
        setvisualpage(1 - page);
        page = 1 - page;
        delay(4);
    }
    closegraph();
}
