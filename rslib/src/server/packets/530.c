#include "both/packets_impl.h"

const int8_t rs_sizes_s2c_530[256] = {
      -1,    0,    8,    0,    2,    0,    0,    0,    0,   12,    0,    1,    0,    3,    7,    0,
      15,    6,    0,    0,    4,    7,   -2,   -1,    2,    0,    2,    8,    0,    0,    0,    0,
      -2,    5,    0,    0,    8,    3,    6,    0,    0,    0,   -1,    0,   -1,    0,    0,    6,
      -2,    0,   12,    0,    0,    0,   -1,   -2,   10,    0,    0,    0,    3,    0,   -1,    0,
       0,    5,    6,    0,    0,    8,   -1,   -1,    0,    8,    0,    0,    0,    0,    0,    0,
       0,   -1,    0,    0,    6,    2,    0,    0,    0,    0,    1,    0,    0,    0,    0,    0,
       0,    5,    0,    0,    0,    0,    5,    0,    0,   -2,    0,    0,    0,    0,    0,   12,
       2,    0,   -2,   -2,   20,    0,    0,   10,    0,   15,    0,   -1,    0,    8,   -2,    0,
       0,    0,    8,    0,   12,    0,    0,    7,    0,    0,    0,    0,    0,   -1,   -1,    0,
       4,    5,    0,    0,    0,    6,    0,    0,    0,    0,    8,    9,    0,    0,    0,    2,
      -1,    0,   -2,    0,    4,   14,    0,    0,    0,   24,    0,   -2,    5,    0,    0,    0,
      10,    0,    0,    4,    0,    0,    0,    0,    0,    0,    0,    6,    0,    0,    0,    2,
       1,    0,    0,    2,   -1,    1,    0,    0,    0,    0,   14,    0,    0,    0,    0,   10,
       5,    0,    0,    0,    0,    0,   -2,    0,    0,    9,    0,    0,    8,    0,    0,    0,
       0,   -2,    6,    0,    0,    0,   -2,    0,    3,    0,    1,    7,    0,    0,    0,    0,
       3,    0,    0,    0,    0,    0,    0,   -1,    0,    0,    0,    0,    0,    3,    0,    0,
};

static const rs_pkt_bind_row k_s2c[] = {
    {RS_S2C_REGION, 162},
    {RS_S2C_REGION_INSTANCED, 214},
    {RS_S2C_NPC_UPDATE, 32},
    {RS_S2C_PLAYER_UPDATE, 225},
    {RS_S2C_VARP_SMALL, 60},
    {RS_S2C_VARP_LARGE, 226},
    {RS_S2C_MESSAGE, 70},
    {RS_S2C_IF_TEXT, 171},
    {RS_S2C_IF_OPENSUB, 145},
    {RS_S2C_IF_CLOSESUB, 149},
    {RS_S2C_IF_OPENTOP, 155},
    {RS_S2C_INVENTORY_UPDATE, 22},
    {RS_S2C_INVENTORY, 105},
    {RS_S2C_LOGOUT, 86},
    {RS_S2C_ENERGY, 234},
    {RS_S2C_WEIGHT, 159},
    {RS_S2C_SYSTEM_UPDATE, 85},
    {RS_S2C_SKILL, 38},
    {RS_S2C_SONG, 4},
    {RS_S2C_HINT, 217},
    {RS_S2C_CAMERA_RESET, 24},
    {RS_S2C_RESET_ANIMS, 131},
    {RS_S2C_SCRIPT, 115},
    {RS_S2C_PLAYER_OPTION, 44},
    {RS_S2C_PROJECTILE, 104},
    {RS_S2C_MINIMAP, 192},
    {RS_S2C_FRIEND_STATUS, 62},
    {RS_S2C_IGNORE_LIST, 126},
    {RS_S2C_PRIVATE_MESSAGE, 0},
    {RS_S2C_CHAT_SETTINGS, 232},
};

void rs_packets_server_530(void) {
    rs_pkt_table_load(530, k_s2c, RS_NELEM(k_s2c), RS_DIR_S2C);
}
