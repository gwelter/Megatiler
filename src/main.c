/**
 * Hello World Example
 * Created With Genesis-Code extension for Visual Studio Code
 * Use "Genesis Code: Compile" command to compile this program.
 **/
#include <genesis.h>
#include <resources.h>

u8 level1[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

void loadLevel() {
    u8 x = 0;
    u8 y = 0;
    u8 t = 0;

    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            t = level1[y][x];
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, t + 1), x, y);
        }
    }
}

int main() {
    VDP_loadTileSet(floortiles.tileset, 1, DMA);
    PAL_setPalette(PAL1, floortiles.palette->data, DMA);

    loadLevel();

    VDP_drawText("Hello Sega!!", 10, 13);
    while (1) {
        // For versions prior to SGDK 1.60 use VDP_waitVSync instead.
        SYS_doVBlankProcess();
    }
    return (0);
}
