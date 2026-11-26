#include "both/packets_impl.h"

const int8_t rs_sizes_c2s_468[256] = {
      0,    0,    0,    0,    0,    0,    2,    0,    0,    8,    2,    0,    0,    0,    0,    0,
      0,    0,    0,    0,    0,    0,    0,    8,   -1,    0,    0,    1,    8,    8,    0,    2,
      2,    0,    0,    2,    8,    2,    8,    0,    0,    0,    0,    2,    8,    2,    0,    0,
      0,    0,    2,    0,    0,    0,    0,    0,    9,    0,    0,    0,    0,    0,    0,    0,
      0,    0,    0,   10,    0,    3,    0,    0,    0,    2,    0,   14,    0,    0,    0,    0,
      6,    2,    0,    0,    0,    0,   -1,    0,    0,   -1,    0,    0,    0,    0,    0,    0,
     12,   14,    6,    0,    0,    0,    0,    6,    0,    0,    0,    0,    6,    0,   -1,    0,
      6,    0,    0,    8,    0,    8,    0,    1,    6,    0,   12,    0,    0,    0,    0,    0,
      0,    0,    8,    0,    0,    0,    0,    8,    0,    6,    2,    0,    0,    0,    0,    6,
      6,    0,    0,    0,    0,    2,    6,    0,    0,    4,    0,    0,    0,    0,    2,    0,
      0,    0,    6,    0,    1,    0,    0,    6,    0,    0,    6,    0,    6,    0,    6,    6,
      0,    1,   12,    0,    0,    6,    6,    0,   -1,    2,    0,    0,    0,    0,    0,    0,
     10,    2,    8,    8,    0,    0,    0,    0,   -1,    0,    0,    0,   14,    9,    0,    0,
     12,    0,    0,    4,    0,    0,    8,    8,    4,    0,    4,    0,    0,    6,    0,    0,
      0,    0,    0,    0,    0,    6,    0,   16,    0,    0,    0,    0,    0,    0,   10,    8,
      0,    8,    0,    0,    0,    0,    0,    0,    0,    0,    6,    0,    8,    8,    0,    0,
};

static const rs_pkt_bind_row k_c2s[] = {
    {RS_C2S_WALK, 184},
    {RS_C2S_WALK_MINIMAP, 24},
    {RS_C2S_WALK_ANTICHEAT, 110},
    {RS_C2S_BUTTON, 221},
    {RS_C2S_BUTTON2, 108},
    {RS_C2S_BUTTON3, 181},
    {RS_C2S_BUTTON4, 143},
    {RS_C2S_BUTTON5, 167},
    {RS_C2S_CLOSE_INTERFACE, 113},
    {RS_C2S_NO_TIMEOUT, 232},
    {RS_C2S_CHAT, 89},
    {RS_C2S_COMMAND, 86},
    {RS_C2S_PRIVATE_MESSAGE, 164},
    {RS_C2S_CHAT_MODE, 69},
    {RS_C2S_ENTER_AMOUNT, 216},
    {RS_C2S_ENTER_NAME, 36},
    {RS_C2S_ADD_FRIEND, 28},
    {RS_C2S_DEL_FRIEND, 135},
    {RS_C2S_CLICK, 119},
    {RS_C2S_NPC1, 185},
    {RS_C2S_OBJECT1, 174},
    {RS_C2S_GROUND_ITEM, 80},
    {RS_C2S_ATTACK_PLAYER, 31},
    {RS_C2S_FOLLOW, 10},
    {RS_C2S_DROP, 252},
    {RS_C2S_EQUIP, 9},
    {RS_C2S_MOVE_ITEM, 215},
    {RS_C2S_IDLE_LOGOUT, 23},
};

void rs_packets_client_468(void) {
    rs_pkt_table_load(468, k_c2s, RS_NELEM(k_c2s), RS_DIR_C2S);
}
