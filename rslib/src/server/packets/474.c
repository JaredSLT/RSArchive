#include "both/packets_impl.h"

const int8_t rs_sizes_s2c_474[256] = {
       0,    0,    0,    0,   20,    1,    0,    5,    0,    0,    6,    0,    0,    0,    0,    3,
       0,    7,   14,    0,    0,    0,    4,    0,    0,    0,    0,    0,    3,    0,    1,    0,
      11,    6,    0,   -1,   10,    0,   15,    0,    0,    0,    1,    0,    6,    0,    0,    0,
       6,    6,    0,    0,    0,    0,   24,    4,   -2,   -1,    0,    0,    0,   -2,    0,    0,
       0,    0,    2,    0,    0,    0,    0,    5,    0,    0,    0,    0,    8,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    2,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    6,    0,    0,    0,    0,
       0,    0,    6,    0,    0,   -1,    0,   -2,    0,    2,    0,    0,    0,    0,    0,    6,
      -2,    0,    0,    0,    0,   12,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,   -2,    0,    0,    0,   -2,    6,    0,    4,    0,    0,    0,    0,    2,    0,
       0,    0,    0,   -2,    0,    0,    0,    6,    2,    2,    0,    0,    0,   -2,    4,   -2,
       0,    0,    8,    6,    0,    0,    0,    0,    5,    0,    0,   -2,    4,    0,    0,    7,
       0,    0,    0,    0,    6,    0,    0,    0,   11,    0,    0,    0,    0,    5,    0,    2,
       0,   -1,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    7,    0,    0,    0,
       0,   -1,    0,    0,    0,    0,    3,   -2,    0,    0,    0,    0,    4,   -2,   -2,    0,
       0,    0,    0,    5,    0,    0,    0,    0,    0,   10,    0,    2,    0,    3,    0,    6,
};

static const rs_pkt_bind_row k_s2c[] = {
    {RS_S2C_VARP_LARGE, 10},
    {RS_S2C_PRIVATE_MESSAGE, 57},
    {RS_S2C_REGION, 61},
    /* JavaMouseWheel.call72b59b7f opcode 119. */
    {RS_S2C_INVENTORY, 119},
    /* opcode 187, size -2. */
    {RS_S2C_INVENTORY_UPDATE, 187},
    {RS_S2C_NPC_UPDATE, 146},
    {RS_S2C_PLAYER_UPDATE, 150},
    {RS_S2C_REGION_INSTANCED, 175},
    /* opcode 196, size 6. */
    {RS_S2C_SKILL, 196},
    {RS_S2C_MESSAGE, 209},
    {RS_S2C_IF_CLOSE, 241},
    {RS_S2C_VARP_SMALL, 253},
};

void rs_packets_server_474(void) {
    rs_pkt_table_load(474, k_s2c, RS_NELEM(k_s2c), RS_DIR_S2C);
}
