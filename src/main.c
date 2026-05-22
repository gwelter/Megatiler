/**
 * Hello World Example
 * Created With Genesis-Code extension for Visual Studio Code
 * Use "Genesis Code: Compile" command to compile this program.
 **/
#include <genesis.h>
#include <resources.h>

#define SFX_COIN 64
#define SFX_UNLOCK 65

#define SPAWN_TILE 4
#define TILESIZE 8
#define MAP_WIDTH 8
#define MAP_HEIGHT 8

#define ANIM_DOWN 0
#define ANIM_UP 1
#define ANIM_SIDE 2

#define SOLID_TILE 1
#define COIN_TILE 6
#define EXIT_TILE 5

#define LEVEL_NUM 3
#define MAX_COINS 3

u8 coins_collected = 0;
char hud_string[10] = "";
bool exit_unlocked = FALSE;

typedef u8 map[MAP_HEIGHT][MAP_WIDTH];

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

typedef struct {
    Point pos;
    u8 w;
    u8 h;
    Sprite *sprite;
    u8 health;
} Coin;

map level1 = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 4, 0, 0, 0, 0, 6, 0},
    {0, 0, 0, 0, 1, 0, 6, 0},
    {0, 0, 0, 1, 1, 0, 6, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 6, 0, 1, 0, 6, 0},
    {0, 0, 0, 0, 0, 0, 0, 5}
};

map level2 = {
    {0, 0, 0, 6, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 4, 0, 1, 1, 1, 0, 0},
    {0, 0, 0, 6, 0, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 6, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 5, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

map level3 = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 6, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 6, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 6, 1, 1, 1, 1, 0, 0},
    {0, 4, 0, 0, 0, 0, 5, 0},
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
Coin coins[MAX_COINS];
Point exit_location = {.x = 0, .y = 0};

u8 *current_level;
static u8 current_level_index = 0;
map *level_list[LEVEL_NUM] = {&level1, &level2, &level3};

void update_score_display() {
    sprintf(hud_string, "SCORE: %d\n", coins_collected);
    VDP_clearText(MAP_WIDTH, 0, 10);
    VDP_drawText(hud_string, MAP_WIDTH, 0);
}

void unlock_exit() {
    exit_unlocked = TRUE;
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 3), exit_location.x, exit_location.y);
    XGM_startPlayPCM(SFX_UNLOCK, 1, SOUND_PCM_CH2);
}

void clear_level() {
    VDP_clearPlane(BG_B, TRUE);
    VDP_clearSprites();
    coins_collected = 0;
    update_score_display();
    exit_unlocked = FALSE;
}

void load_level() {
    clear_level();
    current_level = (u8 *)level_list[current_level_index];

    u8 i = 0;
    u8 total = MAP_HEIGHT * MAP_WIDTH;
    u8 x = 0;
    u8 y = 0;
    u8 t = 0;
    u8 coin_num = 0;
    Coin *c = coins;

    SPR_init();
    for (i = 0; i < total; i++) {
        t = current_level[i];
        if (t == SPAWN_TILE) {
            player.tile_pos.x = x;
            player.tile_pos.y = y;
            player.pos.x = player.tile_pos.x * TILESIZE;
            player.pos.y = player.tile_pos.y * TILESIZE;
            player.sprite = SPR_addSprite(&spr_player, player.pos.x, player.pos.y, TILE_ATTR(PAL2, 0, FALSE, FALSE));
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 1), x, y);
        } else if (t == COIN_TILE) {
            if (coin_num < MAX_COINS) {
                c = &coins[coin_num];
                c->pos.x = x * TILESIZE;
                c->pos.y = y * TILESIZE;
                c->w = 8;
                c->h = 8;
                c->health = 1;
                c->sprite = SPR_addSprite(&spr_coin, c->pos.x, c->pos.y, TILE_ATTR(PAL2, 0, FALSE, FALSE));
                coin_num++;
            }
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 1), x, y);
        } else if (t == EXIT_TILE) {
            exit_location.x = x;
            exit_location.y = y;
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 1), x, y);
        } else {
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, t + 1), x, y);
        }
        x++;
        if (x >= MAP_WIDTH) {
            y++;
            x = 0;
        }
    }
}

int get_tile_at(u8 x, u8 y) { return *(current_level + (y * MAP_WIDTH + x)); }

void move_player(MoveDirection direction) {
    if (!player.moving) {
        switch (direction) {
            case MOVE_DIRECTION_UP:
                if (player.tile_pos.y > 0 && get_tile_at(player.tile_pos.x, player.tile_pos.y - 1) != SOLID_TILE) {
                    player.tile_pos.y -= 1;
                    player.moving = TRUE;
                    player.dir = direction;
                }
                break;
            case MOVE_DIRECTION_DOWN:
                if (player.tile_pos.y < MAP_HEIGHT - 1 &&
                    get_tile_at(player.tile_pos.x, player.tile_pos.y + 1) != SOLID_TILE) {
                    player.tile_pos.y += 1;
                    player.moving = TRUE;
                    player.dir = direction;
                }
                break;
            case MOVE_DIRECTION_LEFT:
                if (player.tile_pos.x > 0 && get_tile_at(player.tile_pos.x - 1, player.tile_pos.y) != SOLID_TILE) {
                    player.tile_pos.x -= 1;
                    player.moving = TRUE;
                    player.dir = direction;
                }
                break;
            case MOVE_DIRECTION_RIGHT:
                if (player.tile_pos.x < MAP_WIDTH - 1 &&
                    get_tile_at(player.tile_pos.x + 1, player.tile_pos.y) != SOLID_TILE) {
                    player.tile_pos.x += 1;
                    player.moving = TRUE;
                    player.dir = direction;
                }
                break;

            default:
                break;
        }
    }
}

void myJoyHandler(u16 joy, u16 change, u16 state) {
    if (joy == JOY_1) {
        if (state & BUTTON_UP) {
            move_player(MOVE_DIRECTION_UP);
        } else if (state & BUTTON_DOWN) {
            move_player(MOVE_DIRECTION_DOWN);
        } else if (state & BUTTON_LEFT) {
            move_player(MOVE_DIRECTION_LEFT);
        } else if (state & BUTTON_RIGHT) {
            move_player(MOVE_DIRECTION_RIGHT);
        }
    }
}

int main() {
    XGM_setPCM(SFX_COIN, sfx_coin, sizeof(sfx_coin));
    XGM_setPCM(SFX_UNLOCK, sfx_unlock, sizeof(sfx_coin));

    VDP_loadTileSet(floortiles.tileset, 1, DMA);
    PAL_setPalette(PAL1, floortiles.palette->data, DMA);
    PAL_setPalette(PAL2, spr_player.palette->data, DMA);

    JOY_init();
    JOY_setEventHandler(&myJoyHandler);

    load_level();
    update_score_display();

    Coin *coin_to_check;
    while (1) {
        /**
         * For continuous movement.
         * Don't use myJoyHandler in case.
         */
        // u16 joy = JOY_readJoypad(JOY_1);
        // if (joy & BUTTON_UP) {
        //     move_player(MOVE_DIRECTION_UP);
        // } else if (joy & BUTTON_DOWN) {
        //     move_player(MOVE_DIRECTION_DOWN);
        // } else if (joy & BUTTON_LEFT) {
        //     move_player(MOVE_DIRECTION_LEFT);
        // } else if (joy & BUTTON_RIGHT) {
        //     move_player(MOVE_DIRECTION_RIGHT);
        // }

        if (player.moving) {
            switch (player.dir) {
                case MOVE_DIRECTION_UP:
                    player.pos.y -= 1;
                    SPR_setAnim(player.sprite, ANIM_UP);
                    break;
                case MOVE_DIRECTION_DOWN:
                    player.pos.y += 1;
                    SPR_setAnim(player.sprite, ANIM_DOWN);
                    break;
                case MOVE_DIRECTION_LEFT:
                    player.pos.x -= 1;
                    SPR_setAnim(player.sprite, ANIM_SIDE);
                    SPR_setHFlip(player.sprite, TRUE);
                    break;
                case MOVE_DIRECTION_RIGHT:
                    player.pos.x += 1;
                    SPR_setAnim(player.sprite, ANIM_SIDE);
                    SPR_setHFlip(player.sprite, FALSE);
                    break;

                default:
                    break;
            }
        }
        if (player.pos.x % TILESIZE == 0 && player.pos.y % TILESIZE == 0) {
            player.moving = FALSE;
            if (exit_unlocked && player.tile_pos.x == exit_location.x && player.tile_pos.y == exit_location.y) {
                current_level_index++;
                if (current_level_index >= LEVEL_NUM) {
                    current_level_index = 0;
                }
                load_level();
            }
        }
        u8 i = 0;
        for (i = 0; i < MAX_COINS; i++) {
            coin_to_check = &coins[i];
            if (player.pos.x < coin_to_check->pos.x + coin_to_check->w &&
                player.pos.x + player.w > coin_to_check->pos.x &&
                player.pos.y < coin_to_check->pos.y + coin_to_check->h &&
                player.pos.y + player.h > coin_to_check->pos.y) {
                if (coin_to_check->health > 0) {
                    coin_to_check->health = 0;
                    SPR_setVisibility(coin_to_check->sprite, HIDDEN);
                    XGM_startPlayPCM(SFX_COIN, 1, SOUND_PCM_CH2);
                    coins_collected++;
                    update_score_display();

                    if (coins_collected == MAX_COINS) {
                        unlock_exit();
                    }
                }
            }
        }
        SPR_update();
        SPR_setPosition(player.sprite, player.pos.x, player.pos.y);
        SYS_doVBlankProcess();
    }
    return (0);
}
