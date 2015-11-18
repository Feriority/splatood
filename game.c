/*
 *
 *  SplatooD - A Splatoon Demake
 *
 */

/* NESlib */
#include "neslib.h"

/* Name tables */
#include "levels/splat_title.h"

#include "levels/kelpdome.h"
#include "levels/bluefin.h"
#include "levels/saltspray.h"
#include "levels/twofish.h"

#include "levels/endgame.h"

/* Music data comes from music.s */
extern const unsigned char music_music_data[];

/* 12:4 fixed-point math for player movement */
#define FP_BITS 4

/* Map dimensions */
#define MAP_WDT         16
#define MAP_WDT_BIT     4
#define MAP_HGT         15

/* Convert screen offset to map offset */
#define MAP_ADR(x,y)    ((((y))<<MAP_WDT_BIT)|(x))

/* Map tiles */
#define TILE_SIZE       16
#define TILE_SIZE_BIT   4

/* Movement */
#define DIR_NONE        0
#define DIR_LEFT        PAD_LEFT
#define DIR_RIGHT       PAD_RIGHT
#define DIR_UP          PAD_UP
#define DIR_DOWN        PAD_DOWN
#define WALK_CYCLE      2
#define FRAMES_PER_STEP 10

/* Tuneable gameplay parameters */
#define RESPAWN_TIME    64
#define PROJECTILE_WAIT_TIME    16
#define PROJECTILE_DISTANCE 4
#define TIMER_START     60
#define TIMER_CYCLE     0x20

/* Tile numbers for important things */
#define TILE_PLAYERA    0x10
#define TILE_PLAYERB    0x11

#define PLAYER_MAX  2

/* Sound effects */
#define SFX_START       0
#define SFX_ITEM        1
#define SFX_RESPAWN1    2
#define SFX_RESPAWN2    3

/* Music */
#define MUSIC_LEVEL         0
#define MUSIC_GAME          1
#define MUSIC_CLEAR         2
#define MUSIC_GAME_OVER     3
#define MUSIC_WELL_DONE     4
#define MUSIC_LOSE          4
#define MUSIC_TITLE         5
#define MUSIC_STAGE_SELECT         6

/* Palettes */
const unsigned char palTitle[16]={ 0x0f,0x30,0x14,0x3d,0x0f,0x30,0x29,0x0f,0x0f,0x30,0x27,0x0f,0x0f,0x30,0x0f,0x0f };
const unsigned char palTitleA[16]={ 0x0f,0x30,0x0f,0x0f,0x0f,0x30,0x0f,0x0f,0x0f,0x30,0x0f,0x0f,0x0f,0x30,0x0f,0x0f };
const unsigned char palTitleB[16]={ 0x0f,0x30,0x0f,0x0f,0x0f,0x30,0x0f,0x0f,0x0f,0x30,0x27,0x0f,0x0f,0x30,0x0f,0x0f };
const unsigned char palTitleC[16]={ 0x0f,0x30,0x14,0x0f,0x0f,0x30,0x0f,0x0f,0x0f,0x30,0x27,0x0f,0x0f,0x30,0x0f,0x0f };

const unsigned char palInkable[16]={ 0x0f,0x10,0x30,0x30,0x0f,0x04,0x14,0x24,0x0f,0x19,0x29,0x3a,0x0f,0x01,0x21,0x31 };
const unsigned char palEndgame[16]={ 0x0f,0x30,0x30,0x30,0x0f,0x24,0x24,0x14,0x0f,0x29,0x29,0x3a,0x0f,0x21,0x21,0x11 };

const unsigned char palPaused[] = {0x0f,0x2d,0x2D,0x2D, 0x0f,0x0f,0x00,0x2d, 0x0f,0x0f,0x00,0x2d, 0x0f,0x00,0x30,0x30 };

const unsigned char palCharacters[8]={ 0x0f,0x04,0x30,0x0f,0x0f,0x19,0x30,0x0f };
const unsigned char palCharPaused[8] = {0x0f,0x00,0x2d,0x0f,0x0f,0x00,0x2d,0x0f};
const unsigned char palJudd[]={ 0x0f,0x26,0x27,0x30,0x0f,0x14,0x27,0x30,0x0f,0x2a,0x27,0x30,0x0f,0x26,0x27,0x30 };

const unsigned char victoryMsg[]={ 0xd6,0xe9,0xe3,0xf4,0xef,0xf2,0xf9,0xa1,0x00 };
const unsigned char fourSpaces[]={ 0xa0,0xa0,0xa0,0x00 };
const unsigned char tieMsg[]={ 0xc9,0xf4,0xa7,0xf3,0xa0,0xe1,0xa0,0xf4,0xe9,0xe5,0xa1,0x00 };
const unsigned char zero[]={0};
const unsigned char selector[]={0xaa,0x0};

const unsigned char credits_0[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xd3,0xf0,0xec,0xe1,0xf4,0xef,0xef,0xc4,0x00 };
const unsigned char credits_1[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xe1,0xa0,0xd3,0xf0,0xec,0xe1,0xf4,0xef,0xef,0xee,0xa0,0xe4,0xe5,0xed,0xe1,0xeb,0xe5,0x00 };
const unsigned char credits_2[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xd0,0xf2,0xef,0xea,0xe5,0xe3,0xf4,0xa0,0xcc,0xe5,0xe1,0xe4,0x00 };
const unsigned char credits_3[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xca,0xef,0xe8,0xee,0xa0,0xc3,0xe1,0xf2,0xed,0xe1,0xe3,0xeb,0xe5,0xf2,0xe1,0xec,0x00 };
const unsigned char credits_4[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xcd,0xf5,0xf3,0xe9,0xe3,0x00 };
const unsigned char credits_5[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xce,0xef,0xe2,0xf5,0xef,0xa0,0xd3,0xf1,0xf5,0xe9,0xe4,0xe9,0xed,0xe1,0xf4,0xf3,0xf5,0x00 };
const unsigned char credits_6[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xd4,0xe9,0xf4,0xec,0xe5,0xa0,0xa6,0xa0,0xd0,0xef,0xf3,0xf4,0xe7,0xe1,0xed,0xe5,0x00 };
const unsigned char credits_7[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xcb,0xef,0xea,0xe9,0xa0,0xc9,0xeb,0xe1,0xf2,0xe1,0xf3,0xe8,0xe9,0x00 };
const unsigned char credits_8[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xc7,0xe1,0xed,0xe5,0xa0,0xc4,0xe5,0xf3,0xe9,0xe7,0xee,0x00 };
const unsigned char credits_9[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xd3,0xf1,0xf5,0xe9,0xe4,0xe5,0xef,0xa0,0xcb,0xef,0xea,0xe9,0xed,0xe1,0x00 };
const unsigned char credits_10[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xc7,0xf2,0xe1,0xf0,0xe8,0xe9,0xe3,0xf3,0x00 };
const unsigned char credits_11[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xd3,0xf1,0xf5,0xe9,0xe4,0xa0,0xcd,0xe5,0xe9,0xe5,0xf2,0x00 };
const unsigned char credits_12[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xc7,0xe1,0xed,0xe5,0xa0,0xcd,0xe5,0xe3,0xe8,0xe1,0xee,0xe9,0xe3,0xf3,0x00 };
const unsigned char credits_13[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xc1,0xed,0xe5,0xf2,0xe9,0xe3,0xe1,0xee,0xa0,0xcd,0xe3,0xc7,0xe9,0xec,0xec,0x00 };
const unsigned char credits_14[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xd3,0xf0,0xec,0xe1,0xf4,0xef,0xef,0xee,0xa0,0xa8,0xc3,0xa9,0xa0,0xb2,0xb0,0xb1,0xb5,0x00 };
const unsigned char credits_15[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xce,0xe9,0xee,0xf4,0xe5,0xee,0xe4,0xef,0x00 };
const unsigned char credits_16[] = { 0xa0,0xa0,0xa0,0xa0,0xa0,0xd4,0xe8,0xe1,0xee,0xeb,0xf3,0xa0,0xe6,0xef,0xf2,0xa0,0xf0,0xec,0xe1,0xf9,0xe9,0xee,0xe7,0xa1,0x00 };
const unsigned char * credits[] = { credits_0, credits_1, credits_2, credits_3, credits_4, credits_5, credits_6, credits_7, credits_8, credits_9, credits_10, credits_11, credits_12, credits_13, credits_14, credits_15, credits_16, zero, zero, zero };

const unsigned char select_map[] = { 0xd3,0xe5,0xec,0xe5,0xe3,0xf4,0xa0,0xcd,0xe1,0xf0,0x00 };
const unsigned char level_kelpdome[] = { 0xcb,0xe5,0xec,0xf0,0xa0,0xc4,0xef,0xed,0xe5,0x00 };
const unsigned char level_bluefin_depot[] ={ 0xc2,0xec,0xf5,0xe5,0xe6,0xe9,0xee,0xa0,0xc4,0xe5,0xf0,0xef,0xf4,0x00 };
const unsigned char level_saltspray[] = { 0xd3,0xe1,0xec,0xf4,0xf3,0xf0,0xf2,0xe1,0xf9,0xa0,0xd2,0xe9,0xe7,0x00 };
const unsigned char level_twofish[] = { 0xd4,0xf7,0xef,0xa0,0xc6,0xe9,0xf3,0xe8,0x00 };

/* Meta sprites for players */
/* DOWN, LEFT, RIGHT, UP */
const unsigned char sprPlayerA1[][17]={
    {
        0,-1,0x40,0,
        8,-1,0x41,0,
        0, 7,0x50,0,
        8, 7,0x51,0,
        128
    },
    {
        0,-1,0x45,0|OAM_FLIP_H,
        8,-1,0x44,0|OAM_FLIP_H,
        0, 7,0x55,0|OAM_FLIP_H,
        8, 7,0x54,0|OAM_FLIP_H,
        128
    },
    {
        0,-1,0x44,0,
        8,-1,0x45,0,
        0, 7,0x54,0,
        8, 7,0x55,0,
        128
    },
    {
        0,-1,0x48,0,
        8,-1,0x49,0,
        0, 7,0x58,0,
        8, 7,0x59,0,
        128
    }
};

const unsigned char sprPlayerA2[][17]={
    {
        0,-1,0x42,0,
        8,-1,0x43,0,
        0, 7,0x52,0,
        8, 7,0x53,0,
        128
    },
    {
        0,-1,0x47,0|OAM_FLIP_H,
        8,-1,0x46,0|OAM_FLIP_H,
        0, 7,0x57,0|OAM_FLIP_H,
        8, 7,0x56,0|OAM_FLIP_H,
        128
    },
    {
        0,-1,0x46,0,
        8,-1,0x47,0,
        0, 7,0x56,0,
        8, 7,0x57,0,
        128
    },
    {
        0,-1,0x4a,0,
        8,-1,0x4b,0,
        0, 7,0x5a,0,
        8, 7,0x5b,0,
        128
    }
};

const unsigned char sprPlayerB1[][17]={
    {
        0,-1,0x40,1,
        8,-1,0x41,1,
        0, 7,0x50,1,
        8, 7,0x51,1,
        128
    },
    {
        0,-1,0x45,1|OAM_FLIP_H,
        8,-1,0x44,1|OAM_FLIP_H,
        0, 7,0x55,1|OAM_FLIP_H,
        8, 7,0x54,1|OAM_FLIP_H,
        128
    },
    {
        0,-1,0x44,1,
        8,-1,0x45,1,
        0, 7,0x54,1,
        8, 7,0x55,1,
        128
    },
    {
        0,-1,0x48,1,
        8,-1,0x49,1,
        0, 7,0x58,1,
        8, 7,0x59,1,
        128
    }
};

const unsigned char sprPlayerB2[][17]={
    {
        0,-1,0x42,1,
        8,-1,0x43,1,
        0, 7,0x52,1,
        8, 7,0x53,1,
        128
    },
    {
        0,-1,0x47,1|OAM_FLIP_H,
        8,-1,0x46,1|OAM_FLIP_H,
        0, 7,0x57,1|OAM_FLIP_H,
        8, 7,0x56,1|OAM_FLIP_H,
        128
    },
    {
        0,-1,0x46,1,
        8,-1,0x47,1,
        0, 7,0x56,1,
        8, 7,0x57,1,
        128
    },
    {
        0,-1,0x4a,1,
        8,-1,0x4b,1,
        0, 7,0x5a,1,
        8, 7,0x5b,1,
        128
    }
};


/* Judd metasprites for post-game screen. */
const unsigned char judd_left[] = {
     0,0,0x60,1,
     8,0,0x61,1,
    16,0,0x62,0,
    24,0,0x63,0,
    32,0,0x64,0,

     0,8,0x70,1,
     8,8,0x71,1,
    16,8,0x72,0,
    24,8,0x73,0,
    32,8,0x74,0,

     0,16,0x80,1,
     8,16,0x81,1,
    16,16,0x82,0,
    24,16,0x83,0,
    32,16,0x84,0,

     0,24,0x90,1,
     8,24,0x91,1,
    16,24,0x92,0,
    24,24,0x93,0,
    32,24,0x94,0,

    128,
};

const unsigned char judd_tie[] = {
    0,0,0x65,0,
    8,0,0x62,0,
    16,0,0x63,0,
    24,0,0x64,0,

    0,8,0x75,0,
    8,8,0x72,0,
    16,8,0x73,0,
    24,8,0x74,0,

    0,16,0x85,0,
    8,16,0x82,0,
    16,16,0x83,0,
    24,16,0x84,0,

    0,24,0x95,0,
    8,24,0x92,0,
    16,24,0x93,0,
    24,24,0x94,0,

    128,
};

const unsigned char judd_right[] = {
    32,0,0x60,2|OAM_FLIP_H,
    24,0,0x61,2|OAM_FLIP_H,
    16,0,0x62,0|OAM_FLIP_H,
     8,0,0x63,0|OAM_FLIP_H,
     0,0,0x64,0|OAM_FLIP_H,

    32,8,0x70,2|OAM_FLIP_H,
    24,8,0x71,2|OAM_FLIP_H,
    16,8,0x72,0|OAM_FLIP_H,
     8,8,0x73,0|OAM_FLIP_H,
     0,8,0x74,0|OAM_FLIP_H,

    32,16,0x80,2|OAM_FLIP_H,
    24,16,0x81,2|OAM_FLIP_H,
    16,16,0x82,0|OAM_FLIP_H,
     8,16,0x83,0|OAM_FLIP_H,
     0,16,0x84,0|OAM_FLIP_H,

    32,24,0x90,2|OAM_FLIP_H,
    24,24,0x91,2|OAM_FLIP_H,
    16,24,0x92,0|OAM_FLIP_H,
     8,24,0x93,0|OAM_FLIP_H,
     0,24,0x94,0|OAM_FLIP_H,

    128,
};

/* Metasprite sets. */
const unsigned char* const sprListPlayerDown[]={
    sprPlayerA1[0],sprPlayerA2[0],sprPlayerB1[0],sprPlayerB2[0]
};
const unsigned char* const sprListPlayerLeft[]={
    sprPlayerA1[1],sprPlayerA2[1],sprPlayerB1[1],sprPlayerB2[1]
};
const unsigned char* const sprListPlayerRight[]={
    sprPlayerA1[2],sprPlayerA2[2],sprPlayerB1[2],sprPlayerB2[2]
};
const unsigned char* const sprListPlayerUp[]={
    sprPlayerA1[3],sprPlayerA2[3],sprPlayerB1[3],sprPlayerB2[3]
};


/* Level count */
#define LEVELS_ALL 4

/* Number of pointers in a level entry in the following list. */
#define LEVEL_ENTRY 4

/*
 * Level list.
 *
 * Nametable, Background palette, Sprite palette, Name
 */
const unsigned char* const levelList[LEVELS_ALL*4]={
    kelpdome,palInkable,palCharacters,level_kelpdome,
    bluefin_depot,palInkable,palCharacters,level_bluefin_depot,
    saltspray,palInkable,palCharacters,level_saltspray,
    twofish,palInkable,palCharacters,level_twofish,
};

/* This is used to ensure the updatelist starts empty. */
const unsigned char updateListData[]={
    NT_UPD_EOF
};

/* (play-time) map data */
static unsigned char map[MAP_WDT*MAP_HGT];

static unsigned char palette_state[64];

/* All the globals after this end up in the zeropage. */

#pragma bssseg (push,"ZEROPAGE")
#pragma dataseg(push,"ZEROPAGE")

static unsigned char i,j;
static unsigned char ptr,spr;
static unsigned char px,py;
static unsigned char wait;
static unsigned int i16;
static unsigned int  sum,div;

static unsigned char nameRow[32];

static unsigned char player_all;

static unsigned int  player_x_spawn [PLAYER_MAX];
static unsigned int  player_y_spawn [PLAYER_MAX];

static unsigned int  player_x        [PLAYER_MAX];
static unsigned int  player_y        [PLAYER_MAX];
static unsigned char player_dir      [PLAYER_MAX];
static int           player_cnt      [PLAYER_MAX];
static unsigned int  player_speed    [PLAYER_MAX];
static unsigned char player_wait     [PLAYER_MAX];
static unsigned char player_cooldown     [PLAYER_MAX];
static unsigned char player_anim_cnt [PLAYER_MAX];
static unsigned char player_diag_flip[PLAYER_MAX];

static unsigned int  projectile_x        [PLAYER_MAX];
static unsigned int  projectile_y        [PLAYER_MAX];
static unsigned char projectile_dir      [PLAYER_MAX];
static int           projectile_cnt      [PLAYER_MAX];
static unsigned int  projectile_speed    [PLAYER_MAX];
static unsigned char projectile_dist [PLAYER_MAX];

static unsigned char game_level;

static unsigned char game_done;
static unsigned char game_paused;

static unsigned char frame_cnt;
static unsigned char anim_index;
static unsigned char bright;

static unsigned char timer;


/* PPU Update list */
static unsigned char update_list[13*3+1];
static unsigned char update_list_index = 0;

void insert_into_update_list(unsigned char addr1, unsigned char addr2, unsigned char value) {
    update_list[update_list_index] = addr1;
    update_list[update_list_index+1] = addr2;
    update_list[update_list_index+2] = value;
    update_list[update_list_index+3] = NT_UPD_EOF;
    update_list_index += 3;

}

void clear_update_list() {
    update_list_index = 0;
    update_list[0] = NT_UPD_EOF;
}


/**
 * Set the pallete for a tile in the attribute table.
 */
void set_tile_palette(unsigned char x_idx, unsigned char y_idx, unsigned char value) {
    unsigned char palette_mask;
    unsigned char palette_address;
    unsigned char palette_shift_val;
    palette_address =  (y_idx / 2) * 8 + x_idx / 2;
    palette_shift_val = (y_idx % 2)*4 + (x_idx % 2)*2;
    palette_mask = ~(0x03 << palette_shift_val);
    palette_state[palette_address] = (palette_state[palette_address] & palette_mask) + (value << palette_shift_val);
    insert_into_update_list((NAMETABLE_A >> 8) + 3, 0xc0 + palette_address, palette_state[palette_address]);
}

void reset_palette_state() {
    unsigned char i;
    for (i = 0; i < 64; i++) {
        palette_state[i] = 0;
    }
}

/* Player scores */
static unsigned int player_score[2] = { 0 };

/**
 * Calculate a player's score from map data.
 */
unsigned int score_player(unsigned char player_id) {
    unsigned char mask;
    unsigned char intermediate;
    unsigned int score = 0;
    unsigned char i;
    mask = player_id + 1;
    mask = ((mask << 2) | mask );
    mask = ((mask << 4) | mask );
    for (i = 0; i < 64; i++) {
        intermediate = palette_state[i] ^ (~ mask);
        intermediate = 0x55 & intermediate & (intermediate >> 1);
        intermediate = (intermediate >> 4) + (intermediate & 0x0f);
        score += (intermediate >> 2) + (intermediate & 0x03);
    }
    return score;
}

/**
 * Determine if a given map tile can be inked.
 */
unsigned char can_ink(unsigned char map_type) {
    if (map_type == 0x2c ||
        map_type == 0x2d ||
        map_type == 0x3c ||
        map_type == 0x3d ||
        map_type == 0x2a ||
        map_type == 0x2b ||
        map_type == 0x3a ||
        map_type == 0x3b) {
            return FALSE;
    }
    return TRUE;
}

/* Make sure this stays a multiple of 2 */
#define TITLE_ROLL_HEIGHT 110

/**
 * Used on title and credits screens to
 * delay while waiting for "START" to be pressed.
 *
 * Allows for skippable animations.
 */
#define WAIT_WITH_SKIP(w) \
    wait = w; \
    while (1) { \
        if (wait == 0) break; \
        wait--; \
        ppu_wait_frame(); \
        if ((pad_trigger(0) | pad_trigger(1)) & PAD_START) { \
            goto _skip_title; \
        } \
    }

/**
 * Show title screen.
 */
void show_title(void) {
    /* Ensure we have the write CHR bank set for the title, and reset scroll to 0. */
    ppu_off();
    bank_bg(0);
    scroll(0,TITLE_ROLL_HEIGHT);

    /* Load title screen into NAME A */
    vram_adr(NAMETABLE_A);
    vram_write(splat_title,sizeof(splat_title));

    /* Clear nametable C to avoid artifacts when bouncing. */
    vram_adr(NAMETABLE_C);
    vram_fill(0,1024);

    pal_bright(4);
    pal_bg(palTitleA);
    ppu_on_bg();
    WAIT_WITH_SKIP(20);

    frame_cnt=0;
    wait=10;

    /* Scroll */
    while (1) {
        frame_cnt += 2;
        scroll(0,TITLE_ROLL_HEIGHT-frame_cnt);

        if (frame_cnt == TITLE_ROLL_HEIGHT) break;

        ppu_wait_frame();

        if ((pad_trigger(0) | pad_trigger(1)) & PAD_START) {
            goto _skip_title;
        }
    }

    scroll(0,0);

    /* Splat splat splat */
    WAIT_WITH_SKIP(20);
    sfx_play(SFX_ITEM,0);
    pal_bg(palTitleB);
    WAIT_WITH_SKIP(20);
    sfx_play(SFX_ITEM,0);
    pal_bg(palTitleC);
    WAIT_WITH_SKIP(20);
    sfx_play(SFX_ITEM,0);
    pal_bg(palTitle);
    WAIT_WITH_SKIP(20);
    music_play(MUSIC_TITLE);

    pal_col(15,0x30);

    wait = 10;
    frame_cnt = 0;

    while (1) {

        /* Flash start between white and gray. */
        wait--;
        if (!wait) {
            pal_col(15,(frame_cnt&1)?0x30:0x20);
            wait = 10;
            frame_cnt++;
        }

        ppu_wait_frame();

        /* Wait for input to start game. */
        if ((pad_trigger(0) | pad_trigger(1)) & PAD_START) {
            break;
        }
    }

_skip_title:
    scroll(0,0);
    pal_bg(palTitle);

    music_stop();

    sfx_play(SFX_START,0);

    /* Blink rapidly. */
    for (i=0;i<16;++i) {
        pal_col(15, (i & 1) ? 0x0f : 0x20);
        delay(4);
    }

}

/* Where numbers start in the EBCDIC tileset */
#define NUM_START 0xB0

/**
 * Put a number into the vram nametable when not rendering.
 * Cheaper than an update table, but only use this when
 * initializing VRAM and not during play.
 */
void put_num(unsigned int adr,unsigned int num,unsigned char len) {
    vram_adr(adr);

    if (len>2) vram_put(NUM_START+(num/100)%10);
    if (len>1) vram_put(NUM_START+(num/10)%10);
    vram_put(NUM_START+(num%10));
}

/**
 * Print a string (EBCDIC) into VRAM.
 * Use only between frames!
 */
void print_str(unsigned int adr, const unsigned char * str) {
    vram_adr(adr);
    while (*str) {
        vram_put(*str);
        str++;
    }
}

/**
 * Map select screen.
 */
void show_select() {
    /* Use bank 1 to get full characterset tiles */
    bank_bg(1);
    bank_spr(1);
    music_play(MUSIC_STAGE_SELECT);

    while (1) {
        /* We disable the PPU while we draw text. */
        ppu_off();
        pal_bg(palEndgame);

        /* We don't use an existing nametable, but instead clear out NAMETABLE_A */
        vram_adr(NAMETABLE_A);
        vram_fill(0xa0,1024-64);
        vram_fill(0,64);

        /* "Select Map" */
        print_str(NAMETABLE_A+0x0EB,select_map);

        /* Map names */
        for (i = 0; i < LEVELS_ALL; ++i) {
            print_str(NAMETABLE_A+0x148+0x20*i,levelList[i*LEVEL_ENTRY+3]);
        }

        /*
         * An asterisk represents the current selection.
         * TODO: use something nicer in 1.1
         */
        print_str(NAMETABLE_A+0x146+0x20*game_level,selector);
        ppu_on_bg();

        /* Loop waiting for input. */
        while (1) {
            ppu_wait_frame();
            j = pad_trigger(0) | pad_trigger(1);
            /* Start - Select map */
            if (j & PAD_START) return;
            /* Up - move cursor up */
            if (j & PAD_UP) {
                if (game_level == 0) {
                    game_level = LEVELS_ALL-1;
                } else {
                    game_level -= 1;
                }

                break;
            }
            /* Select or Down - move cursor down */
            if (j & PAD_SELECT || j & PAD_DOWN) {
                game_level += 1;
                if (game_level == LEVELS_ALL) game_level = 0;
                break;
            }
        }

    }
}

/**
 * Credits sequence.
 */
void show_credits() {
    bank_bg(1);
    bank_spr(1);

    /* Clear the screen. */
    ppu_off();
    pal_bg(palEndgame);

    /* Write credits. */
    i = 0;

    while (i < 17) { /* 4 6 A C */
        ppu_off();
        vram_adr(NAMETABLE_A);
        vram_fill(0xa0,1024-64);
        vram_fill(0,64);
        ppu_on_bg();

        /*
         * This works much the same as the map select screen.
         *
         * We turn off the PPU and render text. Then we'll delay
         * a bit before writing the next line.
         *
         * We show four lines per screen, which usually means
         * two people, with their titles.
         */

        ppu_off();
        print_str(NAMETABLE_A+0x122, credits[i]);
        ppu_on_bg();
        WAIT_WITH_SKIP(50);

        ppu_off();
        print_str(NAMETABLE_A+0x162, credits[i+1]);
        ppu_on_bg();
        WAIT_WITH_SKIP(50);

        ppu_off();
        print_str(NAMETABLE_A+0x1C2, credits[i+2]);
        ppu_on_bg();
        WAIT_WITH_SKIP(50);

        ppu_off();
        print_str(NAMETABLE_A+0x202, credits[i+3]);
        ppu_on_bg();
        WAIT_WITH_SKIP(50);
        i+=4;
    }
    WAIT_WITH_SKIP(200);

_skip_title:
    delay(20);
}

/**
 * Post Game Screen
 *
 * Shows Judd pointing to the winner,
 * inked % of map, total inked cells,
 * and a gauge.
 */
void show_endgame(void) {
    bank_bg(1);
    bank_spr(1);

    /* Set up the background (meter, "Game Over") */
    ppu_off();
    vram_adr(NAMETABLE_A);
    vram_write(endgame,1024);
    pal_bg(palEndgame);
    pal_spr(palJudd);
    oam_clear();

    /* We're going to use the update lists to set background palettes */
    clear_update_list();

    /* Give players with no points a freebie to help with the math. */
    if (player_score[0] == 0) player_score[0] = 1;
    if (player_score[1] == 0) player_score[1] = 1;

    /*
     * We calculate percentages and bar gauge based on total inked
     * not total available for inking. This is a bit different from
     * the real deal.
     */
    sum = (player_score[0] + player_score[1]);

    /* Calculate percentage for player 1 */
    div = player_score[0] * 100;
    div /= sum;
    if (div == 100) div = 99;
    if (div == 0) div = 1;
    put_num(NAMETABLE_A+0x1EA,div,2);

    /* Player 2 must be 100% - (player 1) */
    div = 100 - div;
    put_num(NAMETABLE_A+0x1F4,div,2);

    /* Convert Player 2's percentage to a gauge width */
    div *= 9;
    div /= 100;

    /* Print the victory message. */
    if (player_score[0] > player_score[1]) {
        print_str(NAMETABLE_A+0x1C9,victoryMsg);
        oam_meta_spr(0x68,0x48,0,judd_left);
    } else if (player_score[0] == player_score[1]) {
        /* If both players have the same score, it was a tie. */
        print_str(NAMETABLE_A+0x1CB,tieMsg);
        oam_meta_spr(0x70,0x48,0,judd_tie);
    } else {
        print_str(NAMETABLE_A+0x1CF,victoryMsg);
        oam_meta_spr(0x70,0x48,0,judd_right);
    }

    /* Print raw player scores below the gauge */
    put_num(NAMETABLE_A+0x24A,player_score[0],3);
    put_num(NAMETABLE_A+0x253,player_score[1],3);

    /* Enable the background */
    ppu_on_all();

    /* Play the post-game music. */
    music_play(MUSIC_WELL_DONE);

    /* Set the gauge palettes. */
    clear_update_list();
    for (i = 4; i < 12 - div; ++i) {
        /* Player 1 on the left. */
        set_tile_palette(i,8,1);
    }
    for (i = 12 - div; i <= 11; ++i) {
        /* Player 2 on the right. */
        set_tile_palette(i,8,2);

    }
    set_vram_update(update_list);

    while (1) {
        /* Wait for someone to hit start. */
        ppu_wait_frame();
        clear_update_list();
        if ((pad_trigger(0) | pad_trigger(1)) & PAD_START) break;
    }
    reset_palette_state();
    oam_clear();
}

/* 
 * Move a projectile if the is no wall
 */
void projectile_move(unsigned char id) {
    unsigned char map_type = 0;
    unsigned char dir = projectile_dir[id];

    /*
     * Reset speed; if the last direction we check is valid, we'll set speed, but we
     * shouldn't let players input a valid direction that sets speed plus a higher priority
     * invalid direction to let them move in the invalid direction
     */
    projectile_cnt[id]=0;
    projectile_dist[id]-=1;

    if (projectile_dist[id] == 0) {
            projectile_dir[id]=DIR_NONE;
            return;
    }

    px=projectile_x[id]>>(TILE_SIZE_BIT+FP_BITS);
    py=projectile_y[id]>>(TILE_SIZE_BIT+FP_BITS);

    switch (dir) {
        case DIR_NONE: return; break;
        case DIR_LEFT:   --px; break;
        case DIR_RIGHT:  ++px; break;
        case DIR_UP:     --py; break;
        case DIR_DOWN:   ++py; break;
    }


    if (px > MAP_WDT-1) {
            projectile_dir[id]=DIR_NONE;
            return;
    }

    if (py > MAP_HGT-1) {
            projectile_dir[id]=DIR_NONE;
            return;
    }

    map_type = map[MAP_ADR(px, py)];

    if (can_ink(map_type)) {
        set_tile_palette(px, py, id+1);
    }

    if ((map_type >= 0x22 && map_type <= 0x29) || (map_type >= 0x32 && map_type <= 0x39)) {
            projectile_dir[id]=DIR_NONE;
            return;
    }

    projectile_cnt[id]=TILE_SIZE<<FP_BITS;
}


/**
 * Player movement.
 *
 * Player movement is tile-by-tile with an animation between
 * tiles that we need to initialize.
 */
void player_move(unsigned char id,unsigned char dir) {
    unsigned char map_type = 0;
    if (player_cooldown[id]) {
        return;
    }

    /* We always set direction so sprites update facing */
    player_dir[id]=dir;

    /* A pad direction is being held, so animate */
    player_anim_cnt[id] += 1;

    /*
     * Reset speed; if the last direction we check is valid, we'll set speed, but we
     * shouldn't let players input a valid direction that sets speed plus a higher priority
     * invalid direction to let them move in the invalid direction
     */
    player_cnt[id]=0;

    px=player_x[id]>>(TILE_SIZE_BIT+FP_BITS);
    py=player_y[id]>>(TILE_SIZE_BIT+FP_BITS);

    switch (dir) {
        case DIR_LEFT:  --px; break;
        case DIR_RIGHT: ++px; break;
        case DIR_UP:    --py; break;
        case DIR_DOWN:  ++py; break;
    }


    if (px > MAP_WDT-1) {
            return;
    }

    if (py > MAP_HGT-1) {
            return;
    }

    /*
     * Wall tiles:
     * 0x22~0x29
     * 0x32~0x39
     */
    map_type = map[MAP_ADR(px, py)];

    if (can_ink(map_type)) {
        set_tile_palette(px, py, id+1);
    }

    if ((map_type >= 0x22 && map_type <= 0x29) || (map_type >= 0x32 && map_type <= 0x39)) {
            return;
    }

    player_cnt[id]=TILE_SIZE<<FP_BITS;
    player_diag_flip[id]=1;
    sfx_play(SFX_ITEM,0);
}

/**
 * Generate a projectile from a player.
 */
void player_make_projectile(unsigned char id) {
    if (!player_cooldown[id]) {
        projectile_dist[id] = PROJECTILE_DISTANCE;
        projectile_dir[id] = player_dir[id];
        projectile_x[id] = player_x[id];
        projectile_y[id] = player_y[id];
        player_cooldown[id] = PROJECTILE_WAIT_TIME;
    }
}

/**
 * Splat!
 */
void player_die(unsigned char id) {
    /* Reset to spawn. */
    player_x[id] = player_x_spawn[id];
    player_y[id] = player_y_spawn[id];
    player_cnt[id]=0;
    player_anim_cnt[id]=0;
    player_diag_flip[id]=0;
    player_dir[id] = DIR_NONE;

    player_wait[id] = RESPAWN_TIME;
    sfx_play(SFX_ITEM,0);
}


/**
 * Main gameplay loop.
 */
void game_loop(void) {
    unsigned char map_type;

    ppu_off();

    oam_clear();

    bank_bg(1);
    bank_spr(1);
    scroll(0,0);

    /* Load map */
    vram_adr(NAMETABLE_A);
    vram_write(levelList[game_level*LEVEL_ENTRY],1024);

    vram_adr(NAMETABLE_A+0x3c0);
    vram_read(palette_state, sizeof(palette_state));

    /* Clear nametable C to avoid artifacts when bouncing. */
    vram_adr(NAMETABLE_C);
    vram_fill(0,1024);

    /* Set palette */
    pal_bright(4);
    pal_bg(levelList[game_level*LEVEL_ENTRY+1]);
    pal_spr(levelList[game_level*LEVEL_ENTRY+2]);

    player_all=0;

    timer = TIMER_START;

    i16=NAMETABLE_A;
    ptr=0;
    wait=0;

    vram_adr(NAMETABLE_A);

    /*
     * Read through the map in VRAM to find where
     * players should spawn and replace their spawn
     * locations with the appropriate map tile
     * for the upper left corner of a spawn point.
     */
    for (i = 0; i < MAP_HGT; ++i) {
        vram_adr(i16);
        vram_read(nameRow,32);
        vram_adr(i16);

        for (j = 0; j < (MAP_WDT << 1); j += 2) {
            spr=nameRow[j];

            switch (spr) {
            case TILE_PLAYERA:
            case TILE_PLAYERB:
                player_dir       [player_all] = DIR_NONE;
                player_x_spawn   [player_all] = (j<<3)<<FP_BITS;
                player_y_spawn   [player_all] = (i<<4)<<FP_BITS;
                player_cnt       [player_all] = 0;
                player_anim_cnt  [player_all] = 0;
                player_diag_flip [player_all] = 0;
                player_wait      [player_all] = 16+((spr-TILE_PLAYERA)<<4);
                player_cooldown  [player_all] = 0;
                player_speed     [player_all] = 2<<FP_BITS;

                projectile_cnt   [player_all] = 0;
                projectile_dir   [player_all] = DIR_NONE;
                projectile_speed [player_all] = 8<<FP_BITS;

                ++player_all;
                wait+=16;
                spr=0x2a;
                break;
            }

            map[ptr++]=spr;

            vram_put(spr);
            vram_put(nameRow[j+1]);
        }

        i16+=64;
    }

    /* Store spawn locations for respawn. */
    for (i = 0; i < player_all; ++i) {
        player_x[i] = player_x_spawn[i];
        player_y[i] = player_y_spawn[i];
    }

    /* Clear the update list */
    memcpy(update_list,updateListData,sizeof(updateListData));
    set_vram_update(update_list);

    /* Start rendering. */
    ppu_on_all();

    game_done = FALSE;
    game_paused = FALSE;

    bright = 0;
    frame_cnt = 0;

    while (!game_done) {
        /* Place sprites in the OAM for players. */

        spr=(player_all-1)<<4;

        for (i=0;i<player_all;++i) {
            py=player_y[i]>>FP_BITS;

            if (player_wait[i]) {
                if (player_wait[i]>=16||player_wait[i]&2) py=240;
            }

            px = player_x[i]>>FP_BITS;

            anim_index = i * WALK_CYCLE + (player_anim_cnt[i] / FRAMES_PER_STEP) % WALK_CYCLE;

            switch (player_dir[i]) {
                case DIR_NONE:
                case DIR_DOWN: oam_meta_spr(px,py,spr,sprListPlayerDown[anim_index]); break;
                case DIR_LEFT: oam_meta_spr(px,py,spr,sprListPlayerLeft[anim_index]); break;
                case DIR_RIGHT: oam_meta_spr(px,py,spr,sprListPlayerRight[anim_index]); break;
                case DIR_UP: oam_meta_spr(px,py,spr,sprListPlayerUp[anim_index]); break;
            }
            spr-=16;
        }

        /* Wait for the next frame. */
        ppu_wait_frame();
        clear_update_list();

        /* Tick the timer */
        ++frame_cnt;
        if (!game_paused && (frame_cnt % TIMER_CYCLE == 0)) {
            timer--;
        }
        /* And update it on screen. */
        insert_into_update_list(0x20,0x4f,timer/10 + 0xb0);
        insert_into_update_list(0x20,0x50,timer%10 + 0xb0);


        /* Read both inputs for "START" in trigger mode. */
        i = pad_trigger(0);
        i |= pad_trigger(1);

        /* Pause the game if someone hit start. */
        if (i & PAD_START) {
            game_paused ^= TRUE;
            if (game_paused) {
                /* When the game is paused, we switch to a grayscale palette */
                pal_bg(palPaused);
                pal_spr(palCharPaused);
            } else {
                /* Switch back to level palette. */
                pal_bg(levelList[game_level*LEVEL_ENTRY+1]);
                pal_spr(levelList[game_level*LEVEL_ENTRY+2]);
            }
            music_pause(game_paused);
            sfx_play(SFX_ITEM,0);
        }

        /* Skip processing inputs when the game is paused. */
        if (game_paused) continue;

        /*
         * When players spawn, they flash a bit.
         * We don't start the music until they are done flashing.
         */
        if (wait) {
            --wait;

            if (!wait) {
                music_play(MUSIC_GAME);
            }
        }

        /* When the timer reaches 0, the game is over. */
        if (timer == 0) {
            music_play(MUSIC_CLEAR);
            game_done = TRUE;
        }

        /*
         * Process player actions.
         */
        for (i = 0; i < player_all; ++i) {
            /* Flash respawning players, and prevent them from providing input. */
            if (player_wait[i]) {
                if (player_wait[i] == 16) {
                    sfx_play(i?SFX_RESPAWN2:SFX_RESPAWN1,i);
                }
                --player_wait[i];
                continue;
            }

            if (wait) continue; /* Avoid processing input during initial spawn. */

            /* This player is moving, so animate their movement. */
            if (player_cnt[i])  {
                switch (player_dir[i]) {
                    case DIR_RIGHT: player_x[i]+=player_speed[i]; break;
                    case DIR_LEFT:  player_x[i]-=player_speed[i]; break;
                    case DIR_DOWN:  player_y[i]+=player_speed[i]; break;
                    case DIR_UP:    player_y[i]-=player_speed[i]; break;
                }

                player_cnt[i]-=player_speed[i];

                /* Finish movement. */
                if (player_cnt[i] <= 0) {
                    if (player_cnt[i] < 0) {
                        player_cnt[i] = 0;

                        /* Align player to grid. */
                        player_x[i]=(player_x[i]&0xff00)+(player_dir[i]==DIR_LEFT?0x100:0);
                        player_y[i]=(player_y[i]&0xff00)+(player_dir[i]==DIR_UP  ?0x100:0);
                    }

                } else {
                    /* Continue animating movement. */
                    player_anim_cnt[i] += 1;
                }
            }

            /* No current movement, accept input. */
            if (!player_cnt[i]) {

                /* Read pad state. */
                j = pad_state(i);

                if (player_cooldown[i]) {
                    player_cooldown[i] -= 1;
                }

                if (j & player_dir[i] && player_diag_flip[i]) {
                    /* Give priority new new direction (useful for cornering) */
                    j &= ~player_dir[i];
                    player_diag_flip[i] = 0;
                    player_move(i, player_dir[i]);
                }

                if (j&PAD_LEFT)  player_move(i,DIR_LEFT);
                if (j&PAD_RIGHT) player_move(i,DIR_RIGHT);
                if (j&PAD_UP)    player_move(i,DIR_UP);
                if (j&PAD_DOWN)  player_move(i,DIR_DOWN);
                if (j&PAD_A)     player_make_projectile(i);
            }

            /* If this player has a projectile, move it as well. */
            if (projectile_cnt[i]) {
                if (projectile_cnt[i] <= 0) {
                    if (projectile_cnt[i] < 0) {
                        projectile_cnt[i] = 0;

                        /* Projectiles also lock to tiles, like players. */

                        projectile_x[i]=(projectile_x[i]&0xff00)+(projectile_dir[i]==DIR_LEFT?0x100:0);
                        projectile_y[i]=(projectile_y[i]&0xff00)+(projectile_dir[i]==DIR_UP  ?0x100:0);
                    }
                }
            }

            if (!projectile_cnt[i]) {
                projectile_move(i);
            }

            if (projectile_cnt[i]) {
                switch (projectile_dir[i]) {
                    case DIR_RIGHT: projectile_x[i]+=projectile_speed[i]; break;
                    case DIR_LEFT:  projectile_x[i]-=projectile_speed[i]; break;
                    case DIR_DOWN:  projectile_y[i]+=projectile_speed[i]; break;
                    case DIR_UP:    projectile_y[i]-=projectile_speed[i]; break;
                }

                projectile_cnt[i]-=projectile_speed[i];
            }

        }

        /* Kill players that collide with eachother. */
        for (i = 0; i < player_all; ++i) {
            for (j = i + 1; j < player_all; ++j) {
                if (player_x[i] == player_x[j] && player_y[i] == player_y[j]) {
                    /*
                     * Since all players have rollers and move at the same speed, if you
                     * share a space you must have run into each other, so you both die.
                     */
                    player_die(i);
                    player_die(j);
                }
                if (projectile_dir[j] != DIR_NONE &&
                        (player_x[i] >> (TILE_SIZE_BIT+FP_BITS)) == (projectile_x[j]  >> (TILE_SIZE_BIT+FP_BITS)) &&
                        (player_y[i] >> (TILE_SIZE_BIT+FP_BITS)) == (projectile_y[j]  >> (TILE_SIZE_BIT+FP_BITS))) {
                    /* Player j kills player i using a projectile */
                    player_die(i);
                }
                if (projectile_dir[i] != DIR_NONE &&
                        (player_x[j]  >> (TILE_SIZE_BIT+FP_BITS)) == (projectile_x[i] >> (TILE_SIZE_BIT+FP_BITS)) &&
                        (player_y[j]  >> (TILE_SIZE_BIT+FP_BITS)) == (projectile_y[i] >> (TILE_SIZE_BIT+FP_BITS))) {
                    /* Player i kills player j using a projectile */
                    player_die(j);
                }
            }
        }

        /* Kill players that fall in water. */
        for ( i = 0; i < player_all; ++i) {
            px = player_x[i] >> (TILE_SIZE_BIT + FP_BITS);
            py = player_y[i] >> (TILE_SIZE_BIT + FP_BITS);
            map_type = map[MAP_ADR(px, py)];
            /* Water tiles are 0x2c, 0x2d, 0x3c, 0x3d */
            if (map_type == 0x2c || map_type == 0x2d || map_type == 0x3c || map_type == 0x3d) {
                player_die(i);
            }
        }
    }

    /* Game is over, calculate score. */
    for (i = 0; i < player_all; ++i) {
        player_score[i] = score_player(i);
    }

    reset_palette_state();
    delay(100);
}



void main(void) {
    while (1) {
        /* Show the main title. */
        show_title();

        /* Select a map. */
        show_select();

        /* Start the game. */
        game_loop();

        /* Show the final scores. */
        show_endgame();

        /* Play the credits. */
        show_credits();

        /* Loop back to the title... */
    }
}