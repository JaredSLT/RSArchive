#include "both/packets_impl.h"

const int8_t rs_sizes_c2s_254[256] = {
       0,    0,    0,    0,    1,    0,   -1,    0,    1,    8,    0,    0,    0,    2,    0,    0,
       0,    2,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    4,    0,    0,    0,
       0,    0,    0,    0,    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    6,
       0,    0,    0,   -1,    0,    0,    0,    0,    4,    0,    0,    0,    0,    0,    6,    0,
       0,    0,    0,    0,    4,    0,    0,    0,    0,    0,    0,    0,    0,    2,    0,    0,
       6,    0,    0,   -1,    8,    0,   -1,    0,    0,    0,    0,    4,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    8,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       0,    8,    0,    0,    0,    0,    0,    8,    0,    1,    0,    0,    0,    0,    0,   -1,
       0,    3,    0,    2,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   -1,    2,
       0,    0,    2,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       0,    4,    3,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
       7,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    4,    0,    8,    0,    0,
       2,    8,    0,    0,    0,    0,    0,    0,   12,    1,    8,   10,    0,    0,    1,    0,
       0,    0,    0,    0,    0,    0,   -1,    0,    0,    0,    0,    0,   -1,    0,    0,    0,
       0,   -1,    1,    0,    0,    0,    0,    4,   -1,    1,    4,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    2,   12,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
};


static const rs_pkt_bind_row k_c2s[] = {
    {RS_C2S_IDLE, 239},
    {RS_C2S_FOCUS, 8},
    {RS_C2S_CHAT, 83},
    {RS_C2S_COMMAND, 86},
    {RS_C2S_CHAT_MODE, 129},
    {RS_C2S_WALK, 6},
    {RS_C2S_WALK_MINIMAP, 220},
    {RS_C2S_WALK_ANTICHEAT, 127},
    {RS_C2S_APPEARANCE, 13},
    {RS_C2S_CLOSE_INTERFACE, 58},
    {RS_C2S_PRIVATE_MESSAGE, 214},
    {RS_C2S_ADD_FRIEND, 84},
    {RS_C2S_DEL_FRIEND, 193},
    {RS_C2S_ADD_IGNORE, 189},
    {RS_C2S_DEL_IGNORE, 9},
    {RS_C2S_IDLE_LOGOUT, 144},
    {RS_C2S_CAMERA, 91},
    {RS_C2S_CLICK, 234},
    {RS_C2S_ENTER_AMOUNT, 161},
    {RS_C2S_DIALOGUE, 146},
    {RS_C2S_MOVE_ITEM, 176},
    {RS_C2S_BUTTON, 244},
};

void rs_packets_client_254(void) {
    rs_pkt_table_load(254, k_c2s, RS_NELEM(k_c2s), RS_DIR_C2S);
}
