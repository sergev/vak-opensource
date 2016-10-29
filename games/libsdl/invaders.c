#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

typedef struct {
    const unsigned char width;
    const unsigned char height;
    const unsigned char *data;
    const short transparentIndex;
} image_t;

#include "invaders_images.h"

#define SCREEN_BTN_BOTTOM_RIGHT     0x8
#define SCREEN_BTN_BOTTOM_LEFT      0x1
#define SCREEN_BTN_BOTTOM           (SCREEN_BTN_BOTTOM_LEFT | SCREEN_BTN_BOTTOM_RIGHT)
#define SCREEN_BTN_TOP_LEFT         0x2
#define SCREEN_BTN_TOP_RIGHT        0x4
#define SCREEN_BTN_TOP              (SCREEN_BTN_TOP_LEFT | SCREEN_BTN_TOP_RIGHT)

static int rightButton;
static int leftButton;
unsigned buttonState;

#define PLAYER_SHIP_Y               56
#define PLAYER_SHIP_SIZE_X          7
#define PLAYER_SHIP_SIZE_Y          7
#define PLAYER_SHOOT_COOLDOWN       15

#define PROJECTILE_U                0
#define PROJECTILE_V                7
#define PROJECTILE_W                3
#define PROJECTILE_H                3
#define PROJECTILE_TYPE_PLAYER      1
#define PROJECTILE_TYPE_ENEMY       2

#define MAX_LEFT                    8
#define MAX_RIGHT                   88
#define MAX_PROJECTILE_COUNT        16

#define MONSTER_ROWS                3
#define MONSTER_COLS                6
#define MONSTER_WIDTH               8
#define MONSTER_HEIGHT              6
#define MONSTER_SPACING             4
#define MONSTER0_U                  0
#define MONSTER0_V                  42
#define MONSTER_ROW_WIDTH           (MONSTER_COLS * (MONSTER_WIDTH + MONSTER_SPACING))

#define GAME_MODE_START             1
#define GAME_MODE_PLAY              2
#define GAME_MODE_PAUSE             3
#define GAME_MODE_GAMEOVER          4
#define GAME_MODE_CREDITS           5
#define GAME_MODE_WON               6

#define EXPLOSION_SPRITE_U          25
#define EXPLOSION_SPRITE_V          0
#define EXPLOSION_SPRITE_W          6
#define EXPLOSION_SPRITE_H          6
#define EXPLOSION_SPRITE_N          4

#define FIREWORKS_SPRITE_U          36
#define FIREWORKS_SPRITE_V          6
#define FIREWORKS_SPRITE_W          3
#define FIREWORKS_SPRITE_H          3
#define FIREWORKS_SPRITE_N          4

#define SPRITE_TYPE_EXPLOSION       1
#define SPRITE_TYPE_FIREWORKSHOT    2

#define MAX_SPRITE_COUNT            16

typedef struct Player_s {
    char x;
    char cooldown;
} Player;

typedef struct Projectile_s {
    char x;
    char y;
    char type;
} Projectile;

typedef struct SpriteAnim_s {
    char x;
    char y;
    char frame;
    char type;
    union {
        struct {
            char vx:4;
            char vy:4;
        };
    };
} SpriteAnim;

typedef struct MonsterRow_s {
    char x;
    char y;
    unsigned int isAliveBits;
} MonsterRow;

typedef struct Game_s {
    unsigned int frame;
    unsigned int parallax;
    Player player;
    unsigned char monsterAliveCount;
    MonsterRow monsterRows[MONSTER_ROWS];
    unsigned char monsterDanceDir;
    unsigned char monsterMoveTimeSlit;
    unsigned char monsterSpeed;
    unsigned char projectileCount;
    unsigned char killCount;
    unsigned char gameMode;
    unsigned char spriteCount;
    SpriteAnim sprites[MAX_SPRITE_COUNT];
    Projectile projectiles[MAX_PROJECTILE_COUNT];
} Game;

static Game game;

time_t millis()
{
    struct timeval tv;

    gettimeofday(&tv, 0);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

int calcParallaxX(int camX, int x, char layer)
{
    long dx = x - camX;
    //if (dx > 256) dx = 256;
    //else if (dx < -256) dx = -256;
    return (camX) + ((dx-48)/layer)+48;
}

int calcParallaxY(int camY, int y, char layer)
{
    long dy = y - camY;
    //if (dy > 256) dy = 256;
    //else if (dy < -256) dy = -256;
    return (camY) + ((dy-32)/layer+32);
}

static void draw_image(int x, int y, int w, int h, const image_t *image, int u0, int v0)
{
#if 0
    //TODO
    if (_renderScreen.commandCount >= RENDERSCREEN_MAX_COMMANDS)
        return;
    if (x >= RENDERSCREEN_WIDTH || y >= RENDERSCREEN_HEIGHT || w <= 0 || h <= 0)
        return;

    int x2 = x+w;
    int y2 = y+h;
    if (x2 <= 0 || y2 <= 0)
        return;
    if (x2 > RENDERSCREEN_WIDTH)
        x2 = RENDERSCREEN_WIDTH;
    if (y2 > RENDERSCREEN_HEIGHT)
        y2 = RENDERSCREEN_HEIGHT;

    int u = 0, v = 0;
    if (x < 0)
        u = -x, x = 0;
    if (y < 0)
        v = -y, y = 0;

    RenderCommand *command = &_renderScreen.commands[_renderScreen.commandCount++];
    command->type = RENDERCOMMAND_TRECT;
    command->texture = RENDERCOMMAND_TEXTURED;
    command->fill = imageId;
    command->rect.x1 = x;
    command->rect.y1 = y;
    command->rect.w = x2-x;
    command->rect.y2 = y2;
    command->rect.u = u + u0;
    command->rect.v = v + v0;
#endif
}

static void draw_text(int x, int y, char *string)
{
    //TODO
}

static void tinyInvadersSetup()
{
    memset(&game, 0, sizeof(Game));
    game.player.x = MAX_LEFT + (MAX_RIGHT - MAX_LEFT) / 2;

    //_renderScreen.flags |= RENDERSCREEN_FLAG_NOCLEAR | RENDERSCREEN_FLAG_CLEARBITMAP;
    //_renderScreen.clearFill = &_image_sky_background_opaque;

    unsigned int aliveBits = 0;
    int i;
    for (i=0; i < MONSTER_COLS; i++)
        aliveBits |= 1<<i;
    for (i=0; i < MONSTER_ROWS; i++) {
            MonsterRow *row = &game.monsterRows[i];
            row->x = MAX_LEFT + MONSTER_WIDTH;
            row->y = i * (MONSTER_HEIGHT + MONSTER_SPACING) + 8;
            row->isAliveBits = aliveBits;
    }
    game.monsterMoveTimeSlit = 16;
    game.monsterSpeed = 1;
    game.gameMode = GAME_MODE_START;
}

static SpriteAnim* spawnSprite(char x, char y, char type, char frame)
{
    if (game.spriteCount >= MAX_SPRITE_COUNT) return NULL;
    SpriteAnim *anim = &game.sprites[game.spriteCount++];
    memset(anim,0,sizeof(SpriteAnim));
    anim->x = x;
    anim->y = y;
    anim->type = type;
    anim->frame = frame;

    return anim;
}

static void spawnFirework(char x, char y)
{
    SpriteAnim *anim = spawnSprite(x, y, SPRITE_TYPE_FIREWORKSHOT, rand()%8);
    if (anim) {
        anim->vx = rand()%4 - 1;
            anim->vy = rand()%4 + 1;
    }
}

static SpriteAnim* spawnExplosion(char x, char y, char vx, char vy)
{
    SpriteAnim *e = spawnSprite(x,y,SPRITE_TYPE_EXPLOSION,0);
    if (e) {
        e->vx = vx;
        e->vy = vy;
    }
    return e;
}

static void stepSprites()
{
    int i;
    for (i=game.spriteCount-1; i>=0; i--) {
        SpriteAnim *anim = &game.sprites[i];
        char del = 0;
        char drawFrame = 0;
        char u = 0, v = 0, w = 0, h = 0;
        switch (anim->type & 0xf) {
        case SPRITE_TYPE_EXPLOSION:
            drawFrame = (anim->frame >> 1);
            del = (drawFrame >= EXPLOSION_SPRITE_N);
            u = EXPLOSION_SPRITE_U;
            v = EXPLOSION_SPRITE_V;
            w = EXPLOSION_SPRITE_W;
            h = EXPLOSION_SPRITE_H;
            anim->y -= anim->vy;
            anim->x += anim->vx;
            if (anim->frame % 2 == 0 && anim->vy > 1) {
                anim->vy--;
            }

            anim->frame++;
            break;
        case SPRITE_TYPE_FIREWORKSHOT:
            anim->y -= anim->vy;
            if (anim->frame % 2 == 0 && anim->vy > 1) {
                anim->vy--;
            }
            anim->x += anim->vx;
            anim->frame++;
            u = FIREWORKS_SPRITE_U;
            v = FIREWORKS_SPRITE_V;
            w = FIREWORKS_SPRITE_W;
            h = FIREWORKS_SPRITE_H;
            drawFrame = FIREWORKS_SPRITE_N - (anim->frame >> 2);
            del = (drawFrame < 0);
            if (del) {
                spawnExplosion(anim->x, anim->y,anim->vx, anim->vy);
            }
            break;
        }
        if (!del) {
            u += drawFrame * w;
            draw_image(anim->x - w/2, anim->y - h/2, w, h,
                &_image_tiny_invaders, u, v);
        } else {
            game.sprites[i] = game.sprites[--game.spriteCount];
        }
    }
}

static char shoot(char x, char y, char type)
{
    if (game.projectileCount >= MAX_PROJECTILE_COUNT) return 0;
    Projectile *p = &game.projectiles[game.projectileCount++];
    p->x = x;
    p->y = y;
    p->type = type;
    return 1;
}

static void increaseDifficulty()
{
    if (game.monsterMoveTimeSlit > 8) game.monsterMoveTimeSlit /= 2;
    else if (game.monsterMoveTimeSlit > 4) game.monsterMoveTimeSlit -= 2;
    else if (game.monsterMoveTimeSlit >= 2) game.monsterMoveTimeSlit -= 1;
    else if (game.monsterSpeed < 16 && game.monsterAliveCount < 10) {
            game.monsterSpeed += 1;
    }
}

static void stepMonsters()
{
    game.monsterAliveCount = 0;
    if (game.monsterDanceDir == 0) game.monsterDanceDir = 1;
    char minX = MAX_RIGHT;
    char maxX = MAX_LEFT;
    char maxY = 0;
    int i;
    for (i=0; i < MONSTER_ROWS; i++) {
        MonsterRow *row = &game.monsterRows[i];
        char x = row->x - MONSTER_WIDTH/2;
        char x1 = x + MONSTER_WIDTH;
        char y = row->y;
        char y1 = row->y+MONSTER_HEIGHT;
        char j;
        for (j = 0; j < MONSTER_COLS; j++) {
            if (row->isAliveBits & (1<<j)) {
                if (x < minX) minX = x;
                if (x1 > maxX) maxX = x1;
                if (y1 > maxY) maxY = y1;
                game.monsterAliveCount++;
                draw_image(x, y,
                    MONSTER_WIDTH, MONSTER_HEIGHT, &_image_tiny_invaders,
                    MONSTER0_U + (MONSTER_WIDTH * ((game.frame >> 2) % 2)),
                    MONSTER0_V);
                int k;
                for (k = 0; k < game.projectileCount; k++) {
                    Projectile *p = &game.projectiles[k];
                    if (p->x >= x && p->y >= y && p->x < x1 && p->y < y1) {
                        // kill monster & projectile
                        row->isAliveBits &= ~(1<<j);
                        game.projectiles[k] = game.projectiles[--game.projectileCount];
                        game.killCount += 1;
                        spawnSprite(x+MONSTER_WIDTH/2,y+MONSTER_HEIGHT/2,SPRITE_TYPE_EXPLOSION,0);
                        if (game.killCount % 2 == 0)
                            increaseDifficulty();
                        break;
                    }
                }
            }
            x += MONSTER_WIDTH + MONSTER_SPACING;
            x1 += MONSTER_WIDTH + MONSTER_SPACING;
        }
    }
    if (maxY > PLAYER_SHIP_Y) {
        if (game.gameMode != GAME_MODE_GAMEOVER) {
            game.gameMode = GAME_MODE_GAMEOVER;
            game.frame = 0;
        }
        return;
    }
    if (game.frame % game.monsterMoveTimeSlit == 0) {
        char flip = 0;
        if (minX < MAX_LEFT) {
            flip = 1;
            game.monsterDanceDir = 1;
        }
        if (maxX > MAX_RIGHT) {
            flip = 1;
            game.monsterDanceDir = -1;
        }
        int i;
        for (i = 0; i < MONSTER_ROWS; i++) {
            MonsterRow *row = &game.monsterRows[i];
            row->x += game.monsterDanceDir * ((game.frame % (game.monsterSpeed >> 1 | 1) != 0) + ((game.monsterSpeed >> 2) + 1));
            row->y += flip;
        }
        if (flip) increaseDifficulty();
    }
    if (game.monsterAliveCount == 0) {
        game.gameMode = GAME_MODE_WON;
        game.frame = 0;
    }
}

static void stepProjectiles()
{
    int i;

    for (i = game.projectileCount - 1; i>=0;i--) {
        Projectile *p = &game.projectiles[i];
        char isOut = 0;
        switch (p->type) {
        case PROJECTILE_TYPE_PLAYER:
            p->y -= 3;
            isOut = p->y <= 0;
            break;
        case PROJECTILE_TYPE_ENEMY:
            p->y += 1;
            isOut = p->y >= 64;
            break;
        }

        draw_image(p->x - (PROJECTILE_W/2),
            p->y - (PROJECTILE_H/2),
            PROJECTILE_W, PROJECTILE_H, &_image_tiny_invaders,
            PROJECTILE_U + ((game.frame+i)&1) * PROJECTILE_W, PROJECTILE_V);
        if (isOut) {
            game.projectiles[i] = game.projectiles[--game.projectileCount];
        }
    }
}

static void stepPlayer()
{
    char cmd = 0;
    if (buttonState & SCREEN_BTN_BOTTOM_LEFT)
        cmd -= 1;
    if (buttonState & SCREEN_BTN_BOTTOM_RIGHT)
        cmd += 1;
    if (cmd > 0 && game.player.x < MAX_RIGHT)
        game.player.x += 1;
    if (cmd < 0 && game.player.x > MAX_LEFT)
        game.player.x -= 1;
    if ((leftButton || rightButton || buttonState & SCREEN_BTN_TOP) && game.player.cooldown == 0
                    && shoot(game.player.x,PLAYER_SHIP_Y,PROJECTILE_TYPE_PLAYER)) {
        game.player.cooldown = PLAYER_SHOOT_COOLDOWN;
    }

    game.player.cooldown -= (game.player.cooldown > 0);

    draw_image(game.player.x - (PLAYER_SHIP_SIZE_X/2),
        PLAYER_SHIP_Y, PLAYER_SHIP_SIZE_X, PLAYER_SHIP_SIZE_Y,
        &_image_tiny_invaders, 0, 0);
}

static void stepBackground(char moving)
{
    game.frame += 1;
    game.parallax += moving ? 1 : 0;
#if 0
    _renderScreen.clearFillOffsetX = game.parallax>>4;
    if (game.frame % 4 == 0) {
        if (!moving) {
            if (_renderScreen.clearFillOffsetY > 0)
                _renderScreen.clearFillOffsetY -= 1;
        } else {
            if (_renderScreen.clearFillOffsetY < 64)
                _renderScreen.clearFillOffsetY += 1;
        }
    }
#endif
    draw_image(0, 63-16-6, 96, 16, &_image_clouds,
        (game.parallax>>3) % _image_clouds.width, 0);

    draw_image(0, 63-16, 96, 16, &_image_citylayer,
        (game.parallax>>2) % _image_citylayer.width, 0);

    draw_image(0, 63, 96, 1, &_image_roadlayer_opaque,
        (game.parallax) % 32, 0);
}

static void gamePlayLoop()
{
    stepBackground(0);
    //draw_image(0, 0, 96, 64, &_image_sky_background_opaque, 0, 0);
    stepPlayer();
    stepMonsters();
    stepProjectiles();
    stepSprites();
}

static void gameStartLoop()
{
    stepBackground(1);
    tinyInvadersSetup();
    game.gameMode = GAME_MODE_PLAY;

    stepSprites();

    draw_image(6, 8, 84, 33, &_image_tiny_invaders, 44, 15);

    draw_text(13, 48, "press button");
}

static void gameOverLoop()
{
    if (rand() > 26000) {
        spawnExplosion(4 + rand()%90, 48 + rand()%16, 0, 1);
    }
    stepBackground(1);
    stepMonsters();
    stepSprites();
    draw_text(20, 10, "game over");

    if ((leftButton == 1 || rightButton == 1 || buttonState) && game.frame > 30) {
        game.gameMode = GAME_MODE_START;
    }
}

static void gameWonLoop()
{
    stepBackground(1);
    stepPlayer();
    stepSprites();
    if (rand() > 27500)
        spawnFirework(16 + rand()%64, 52 - rand()%8);

    draw_text(23, 40, "you won!");

    if ((leftButton == 1 || rightButton == 1 || buttonState) && game.frame > 30) {
            game.gameMode = GAME_MODE_START;
    }
}

static void tinyInvadersLoop()
{
    static char init = 0;

    if (! init) {
        init = 1;
        //_renderScreen.clearFillOffsetY = 64;
        tinyInvadersSetup();
    }

    switch (game.gameMode) {
    case GAME_MODE_START:    gameStartLoop(); break;
    case GAME_MODE_PLAY:     gamePlayLoop(); break;
    case GAME_MODE_GAMEOVER: gameOverLoop(); break;
    case GAME_MODE_WON:      gameWonLoop(); break;
    }
}

static int getButtons()
{
    //TODO
    return 0;
}

int main()
{
    for (;;) {
        unsigned start = millis();

        buttonState = getButtons();
        tinyInvadersLoop();

        // cap to 20fps
        while (millis() - start < 50)
            usleep(10000);
    }
}
