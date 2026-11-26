#include "both/packets_impl.h"

const int8_t rs_sizes_c2s_530[256] = {
       0,    0,    8,    4,    0,    0,    0,    0,    0,    4,    0,    0,    6,    0,    0,    0,
       0,    3,    0,    0,    0,    2,    0,    0,    4,    0,    0,    0,    0,    0,    8,    0,
       0,    4,    0,    0,    0,    2,    0,    4,   10,    0,    8,    0,    0,    4,    0,    0,
       0,    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,    6,    0,    8,    0,    4,
       0,    0,    0,    6,    0,    0,    4,    0,    0,    0,    0,    0,    0,    0,   -1,    0,
       0,    4,    0,    0,    0,    0,    0,    0,    2,    0,    2,    0,    0,    0,    2,    0,
       0,    0,    0,    0,    0,    0,    2,    0,    0,    0,    0,   -1,    0,    0,    0,   -1,
       0,    0,    4,    0,    0,    1,    0,    1,    0,    0,    0,    2,    0,    0,    0,   -1,
       0,    0,    0,    8,    8,    2,    6,    0,    6,    8,    3,    0,    0,    2,    0,    0,
       0,    0,    0,    0,    0,    0,    4,    0,    4,    0,   12,    0,    0,    0,    4,   -1,
       2,   -1,    0,    0,    0,    0,    0,    5,    0,    2,    0,    0,    0,    2,    0,    0,
       0,    0,   -1,    4,    0,    0,    0,    2,    0,    2,    4,    0,    0,   -1,    2,    4,
       0,    0,    0,    2,    0,    0,    0,    2,    8,    2,    0,    2,    0,    0,    0,    0,
       /* Opcode 215 is the walk. Client writes one length byte, then 2*n+3 payload bytes. */
       0,    0,    0,    6,    3,    0,    2,   -1,    0,    0,    0,    0,    6,    0,   -1,    0,
      10,    0,    2,    0,    2,    0,    0,   -1,    2,    2,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,    1,    0,   -1,    0,    0,    0,    0,    0,
};

static const rs_pkt_bind_row k_c2s[] = {
    {RS_C2S_COMMAND, 44},
    {RS_C2S_ADD_FRIEND, 120},
    {RS_C2S_DEL_FRIEND, 57},
    {RS_C2S_ADD_IGNORE, 34},
    {RS_C2S_DEL_IGNORE, 213},
    {RS_C2S_CLICK, 75},
    {RS_C2S_CAMERA, 21},
    {RS_C2S_FOCUS, 22},
    {RS_C2S_ENTER_AMOUNT, 23},
    {RS_C2S_ENTER_NAME, 244},
    {RS_C2S_CHAT_MODE, 157},
    {RS_C2S_WALK, 215},
    {RS_C2S_WALK_MINIMAP, 39},
    {RS_C2S_WALK_ANTICHEAT, 77},
    {RS_C2S_CLOSE_INTERFACE, 184},
    {RS_C2S_NO_TIMEOUT, 93},
    {RS_C2S_BUTTON, 155},
    {RS_C2S_BUTTON2, 196},
    {RS_C2S_BUTTON3, 124},
    {RS_C2S_BUTTON4, 199},
    {RS_C2S_BUTTON5, 234},
};

void rs_packets_client_530(void) {
    rs_pkt_table_load(530, k_c2s, RS_NELEM(k_c2s), RS_DIR_C2S);
}
