/**
 * Hello World Example
 * Created With Genesis-Code extension for Visual Studio Code
 * Use "Genesis Code: Compile" command to compile this program.
 **/
#include <genesis.h>
#include <resources.h>

#define SPAWN_TILE 4
#define TILESIZE 8
#define MAP_WIDTH 8
#define MAP_HEIGHT 8

typedef enum {
    MOVE_DIRECTION_UP,
    MOVE_DIRECTION_DOWN,
    MOVE_DIRECTION_LEFT,
    MOVE_DIRECTION_RIGHT,
    MOVE_DIRECTION_NONE,
} MoveDirection;

typedef struct {
    u8 x;
    u8 y;
} Point;

typedef struct {
    Point pos;
    Point tile_pos;
    int w;
    int h;
    int health;
    bool moving;
    MoveDirection dir;
    Sprite *sprite;
    char name[6];
} Entity;

u8 level1[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 4, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

Entity player = {
    {0, 0},
    {0, 0},
    8,
    8,
    0,
    FALSE,
    MOVE_DIRECTION_NONE,
    NULL,
    "PLAYER",
};

void loadLevel() {
    u8 x = 0;
    u8 y = 0;
    u8 t = 0;

    SPR_init();
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            t = level1[y][x];
            if (t == SPAWN_TILE) {
                player.tile_pos.x = x;
                player.tile_pos.y = y;
                player.pos.x = player.tile_pos.x * TILESIZE;
                player.pos.y = player.tile_pos.y * TILESIZE;
                player.sprite =
                    SPR_addSprite(&spr_player, player.pos.x, player.pos.y, TILE_ATTR(PAL2, 0, FALSE, FALSE));
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 1), x, y);
            } else {
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, t + 1), x, y);
            }
        }
    }
}

int main() {
    VDP_loadTileSet(floortiles.tileset, 1, DMA);
    PAL_setPalette(PAL1, floortiles.palette->data, DMA);
    PAL_setPalette(PAL2, spr_player.palette->data, DMA);

    loadLevel();

    VDP_drawText("Hello Sega!!", 10, 13);
    while (1) {
        // For versions prior to SGDK 1.60 use VDP_waitVSync instead.
        SPR_update();
        SYS_doVBlankProcess();
    }
    return (0);
}
