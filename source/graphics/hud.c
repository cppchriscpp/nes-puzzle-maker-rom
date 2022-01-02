#include "source/neslib_asm/neslib.h"
#include "source/graphics/hud.h"
#include "source/map/map.h"
#include "source/menus/text_helpers.h"
#include "source/configuration/game_info.h"
#include "source/globals.h"
#include "source/menus/error.h"

CODE_BANK(PRG_BANK_HUD);

ZEROPAGE_DEF(unsigned char, editorSelectedTileId);

#define tempTileId tempChar1
#define tempTileIndex tempChar2

// FIXME: Merge with original; no different anymore
void put_hud_str(unsigned int adr, const char* str) {
	vram_adr(adr);
	while(1) {
		if(!*str) break;
		vram_put((*str++)+0x60);//-0x20 because ASCII code 0x20 is placed in tile 80 of the CHR
	}
}


void draw_hud() {
    vram_adr(NAMETABLE_A + HUD_POSITION_START);
    for (i = 0; i != 160; ++i) {
        vram_put(HUD_TILE_BLANK);
    }
    vram_put(HUD_TILE_BORDER_BL);
    for (i = 0; i != 30; ++i) {
        vram_put(HUD_TILE_BORDER_HORIZONTAL);
    }
    vram_put(HUD_TILE_BORDER_BR);

    vram_adr(NAMETABLE_A + HUD_ATTRS_START);
    for (i = 0; i != 16; ++i) {
        vram_put(0xff);
    }

    set_vram_update(NULL);
    
    /*
    memcpy(screenBuffer, (&(currentGameData[0]) + GAME_DATA_OFFSET_TITLE), GAME_DATA_OFFSET_TITLE_LENGTH);
    screenBuffer[GAME_DATA_OFFSET_TITLE_LENGTH] = NULL;
    put_hud_str(NTADR_A(1, 26), screenBuffer);
    */

    
}

void draw_editor_hud() {
    vram_adr(NAMETABLE_A + HUD_POSITION_START);
    for (i = 0; i != 160; ++i) {
        vram_put(HUD_TILE_BLANK);
    }
    vram_put(HUD_TILE_BORDER_BL);
    for (i = 0; i != 30; ++i) {
        vram_put(HUD_TILE_BORDER_HORIZONTAL);
    }
    vram_put(HUD_TILE_BORDER_BR);

    vram_adr(NAMETABLE_A + HUD_ATTRS_START);
    for (i = 0; i != 16; ++i) {
        vram_put(0xff);
    }

    // TODO: Constants for the arrows
    if (currentLevelId > 0) {
        vram_adr(NAMETABLE_A + HUD_POSITION_START + 64 + 1);
        vram_put(0xe9);
        vram_adr(NAMETABLE_A + HUD_POSITION_START + 96 + 1);
        vram_put(0xf9);
    }

    if (currentLevelId < (totalGameLevels-1)) {
        vram_adr(NAMETABLE_A + HUD_POSITION_START + 64 + 30);
        vram_put(0xea);
        vram_adr(NAMETABLE_A + HUD_POSITION_START + 96 + 30);
        vram_put(0xfa);
    }

    put_hud_str(NAMETABLE_A + HUD_EDITOR_TITLE_START, "          Objects       Player");

    vram_adr(NAMETABLE_A + HUD_EDITOR_TILES_START);
    for (i = 0; i != 8; ++i) {
        tempTileId = currentMapTileData[i << 2];
        vram_put(tempTileId++);
        vram_put(tempTileId);
    }

    // Fixing palette ids
    vram_adr(NAMETABLE_A + 0x3f1);
    for (i = 0; i != 4; ++i) {
        // Get second tile's id first, then shift it two and add the first
        tempTileId = currentMapTileData[(i << 3) + TILE_DATA_LOOKUP_OFFSET_PALETTE + 4];
        tempTileId <<= 2;
        tempTileId |= currentMapTileData[(i << 3) + TILE_DATA_LOOKUP_OFFSET_PALETTE];
        // Leave other two palettes alone
        tempTileId <<= 4;
        tempTileId |= 0x0f;
        vram_put(tempTileId);
    }


    oam_spr(208, 15, playerSpriteTileId, 0x03, HUD_PLAYER_SPRITE_OAM);
    oam_spr(216, 15, playerSpriteTileId+1, 0x03, HUD_PLAYER_SPRITE_OAM+4);
    oam_spr(208, 23, playerSpriteTileId+16, 0x03, HUD_PLAYER_SPRITE_OAM+8);
    oam_spr(216, 23, playerSpriteTileId+17, 0x03, HUD_PLAYER_SPRITE_OAM+12);


    vram_adr(NAMETABLE_A + HUD_EDITOR_TILES_START + 32);
    for (i = 0; i != 8; ++i) {
        tempTileId = currentMapTileData[i << 2] + 16;
        vram_put(tempTileId++);
        vram_put(tempTileId);
    }


}

void update_hud() {
    switch (currentGameStyle) {
        case GAME_STYLE_MAZE:
            break;
        case GAME_STYLE_COIN:
            for (i = 0; i != 16; ++i) {
                if (tileCollisionTypes[i] == TILE_COLLISION_COLLECTABLE) {
                    tempTileIndex = i;
                    tempTileId = (i<<1);
                    break;
                }
            }
            i = 0;
            screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START) | NT_UPD_HORZ;
            screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START);
            screenBuffer[i++] = 2;
            screenBuffer[i++] = tempTileId;
            screenBuffer[i++] = tempTileId+1;
            screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START + 32) | NT_UPD_HORZ;
            screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START + 32);
            screenBuffer[i++] = 2;
            screenBuffer[i++] = tempTileId+16;
            screenBuffer[i++] = tempTileId+17;
            screenBuffer[i++] = MSB(NAMETABLE_A + 0x03f5);
            screenBuffer[i++] = LSB(NAMETABLE_A + 0x03f5);
            screenBuffer[i++] = (tilePalettes[tempTileIndex] << 6) | 0x3f;
            screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START + 62) | NT_UPD_HORZ;
            screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START + 62);
            screenBuffer[i++] = 5;
            screenBuffer[i++] = (playerKeyCount / 10) + '0' + 0x60;
            screenBuffer[i++] = (playerKeyCount % 10) + '0' + 0x60;
            screenBuffer[i++] = '/' + 0x60;
            screenBuffer[i++] = (totalKeyCount / 10) + '0' + 0x60;
            screenBuffer[i++] = (totalKeyCount % 10) + '0' + 0x60;

            screenBuffer[i++] = NT_UPD_EOF;
            set_vram_update(screenBuffer);

            break;
        case GAME_STYLE_CRATES:
            for (i = 0; i != 16; ++i) {
                if (tileCollisionTypes[i] == TILE_COLLISION_CRATE) {
                    tempTileIndex = i;
                    tempTileId = (i<<1);
                    break;
                }
            }
            i = 0;
            screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START) | NT_UPD_HORZ;
            screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START);
            screenBuffer[i++] = 2;
            screenBuffer[i++] = tempTileId;
            screenBuffer[i++] = tempTileId+1;
            screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START + 32) | NT_UPD_HORZ;
            screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START + 32);
            screenBuffer[i++] = 2;
            screenBuffer[i++] = tempTileId+16;
            screenBuffer[i++] = tempTileId+17;
            screenBuffer[i++] = MSB(NAMETABLE_A + 0x03f5);
            screenBuffer[i++] = LSB(NAMETABLE_A + 0x03f5);
            screenBuffer[i++] = (tilePalettes[tempTileIndex] << 6) | 0x3f;
            screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START + 62) | NT_UPD_HORZ;
            screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START + 62);
            screenBuffer[i++] = 5;
            screenBuffer[i++] = (playerCrateCount / 10) + '0' + 0x60;
            screenBuffer[i++] = (playerCrateCount % 10) + '0' + 0x60;
            screenBuffer[i++] = '/' + 0x60;
            screenBuffer[i++] = (totalCrateCount / 10) + '0' + 0x60;
            screenBuffer[i++] = (totalCrateCount % 10) + '0' + 0x60;

            screenBuffer[i++] = NT_UPD_EOF;
            set_vram_update(screenBuffer);

            break;
    }
}

void update_editor_hud() {

    tempTileId = HUD_EDITOR_TILES_SPRITE_START + (editorSelectedTileId << 4);

    if (editorSelectedTileId == TILE_EDITOR_POSITION_LEFT) {
        tempTileId = 4;
    } else if (editorSelectedTileId == TILE_EDITOR_POSITION_RIGHT) {
        tempTileId = 236;
    }

    oam_spr(tempTileId, 15, HUD_SELECTOR_SPRITE_ID, 0x00, HUD_SELECTOR_SPRITE_OAM);
    oam_spr(tempTileId+8, 15, HUD_SELECTOR_SPRITE_ID+1, 0x00, HUD_SELECTOR_SPRITE_OAM+4);
    oam_spr(tempTileId, 23, HUD_SELECTOR_SPRITE_ID+16, 0x00, HUD_SELECTOR_SPRITE_OAM+8);
    oam_spr(tempTileId+8, 23, HUD_SELECTOR_SPRITE_ID+17, 0x00, HUD_SELECTOR_SPRITE_OAM+12);

}