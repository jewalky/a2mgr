#pragma once
#include <string>

#define SCREENTYPE_PNG 0
#define SCREENTYPE_JPG 1
#define SCREENTYPE_BMP 2
#define SCREENTYPE_GIF 3

extern unsigned long v_screentype;

extern unsigned long g_scrollintv;

extern bool r_fps;
extern bool r_time;

extern bool r_grid;

extern bool g_altchat;

extern unsigned long r_width;
extern unsigned long r_height;
extern unsigned long z_resolution;

extern bool z_logsdir;
extern bool z_screendir;

extern bool z_russian;
extern bool z_lang_override;

extern unsigned long net_key;
extern unsigned long net_maxplayers;

extern unsigned long r_grid_color;

extern unsigned long z_handle;

extern unsigned long crc32_allods2_exe;
extern unsigned long crc32_a2mgr_dll;
extern unsigned long crc32_patch_res;
extern unsigned long crc32_world_res;
extern unsigned long crc32_graphics_res;

extern unsigned char sha1_allods2_exe[20];
extern unsigned char sha1_a2mgr_dll[20];
extern unsigned char sha1_patch_res[20];
extern unsigned char sha1_world_res[20];
extern unsigned char sha1_graphics_res[20];
extern unsigned char sha1_uuid[20];
extern std::string current_directory;

extern bool z_softcore;

#define PROTO_VER 20

void ParseConfig();