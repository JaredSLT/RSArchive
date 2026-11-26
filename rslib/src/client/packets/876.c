#include "both/packets_impl.h"

/* Sizes from the client packet constructors. Missing opcodes are -3. */
const int8_t rs_sizes_c2s_876[256] = {
       7,   15,    1,   -1,    8,   -1,    0,    3,    4,   -1,   -1,    5,   16,   -1,    8,    4,
      -1,    3,    8,    1,    6,    8,    0,    8,    7,    1,    7,    7,    7,    9,   -2,    8,
      11,   -1,    2,    2,    9,    4,    3,    9,    9,    9,   -1,    3,   18,    8,   -2,   12,
      -2,    3,    8,   -2,    3,   -1,    3,   -1,    0,    0,   -2,    3,    1,    3,    8,    4,
      15,    3,   -1,    3,    3,    9,    7,   -1,    8,    6,   -2,   -1,    4,    1,    3,    4,
      -1,    4,    4,   -2,    3,   11,   -2,   -1,    3,    5,    9,    2,   -1,    4,    7,   -1,
      -1,   -1,   -1,    9,    3,    3,   16,   -1,   -2,   -1,    0,   17,    4,   -1,   18,   -1,
      -2,    6,   -1,   -1,   -1,    1,   -2,    0,   -1,   -2,    4,   -3,   -3,   -3,   -3,   -3,
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,
};

static const rs_pkt_bind_row k_c2s[] = {
    {RS_C2S_CHAT_MODE, 2}, /* DataBlock1675.clscdcb044e3 Statics */
    {RS_C2S_WALK, 11}, /* DataBlock1675.clscdcb044e12 Friend */
    {RS_C2S_BUTTON3, 14}, /* DataBlock1675.clscdcb044e15 Client */
    {RS_C2S_BUTTON4, 18}, /* DataBlock1675.clscdcb044e19 Client */
    {RS_C2S_WINDOW_STATUS, 20}, /* DataBlock1675.clscdcb044e21 Statics */
    {RS_C2S_GROUND_ITEM, 24}, /* DataBlock1675.clscdcb044e25 Friend */
    {RS_C2S_FOCUS, 25}, /* DataBlock1675.clscdcb044e26 DataBlock1200 */
    {RS_C2S_BUTTON5, 31}, /* DataBlock1675.clscdcb044e32 Client */
    {RS_C2S_MAGIC_ON_NPC, 32}, /* DataBlock1675.clscdcb044e33 Friend */
    {RS_C2S_OBJECT1, 36}, /* DataBlock1675.clscdcb044e37 Friend */
    {RS_C2S_OBJECT2, 39}, /* DataBlock1675.clscdcb044e40 Friend */
    {RS_C2S_OBJECT3, 40}, /* DataBlock1675.clscdcb044e41 Friend */
    {RS_C2S_OBJECT4, 41}, /* DataBlock1675.clscdcb044e42 Friend */
    {RS_C2S_TRADE, 43}, /* DataBlock1675.clscdcb044e44 Friend */
    {RS_C2S_WALK_MINIMAP, 44}, /* DataBlock1675.clscdcb044e45 Friend */
    {RS_C2S_BUTTON, 50}, /* DataBlock1675.clscdcb044e51 Client */
    {RS_C2S_NPC3, 52}, /* DataBlock1675.clscdcb044e53 Friend */
    {RS_C2S_CLOSE_INTERFACE, 56}, /* DataBlock1675.clscdcb044e57 Statics */
    {RS_C2S_PRIVATE_MESSAGE, 58}, /* DataBlock1675.clscdcb044e59 PingRequest */
    {RS_C2S_NPC1, 59}, /* DataBlock1675.clscdcb044e60 Friend */
    {RS_C2S_NPC4, 61}, /* DataBlock1675.clscdcb044e62 Friend */
    {RS_C2S_NPC2, 65}, /* DataBlock1675.clscdcb044e66 Friend */
    {RS_C2S_FOLLOW, 68}, /* DataBlock1675.clscdcb044e69 Friend */
    {RS_C2S_BUTTON2, 72}, /* DataBlock1675.clscdcb044e73 Client */
    {RS_C2S_CLICK, 73}, /* DataBlock1675.clscdcb044e74 NativeMouse */
    {RS_C2S_CAMERA, 79}, /* DataBlock1675.clscdcb044e80 DataBlock1200 */
    {RS_C2S_ENTER_AMOUNT, 81}, /* DataBlock1675.clscdcb044e82 PingRequest */
    {RS_C2S_MAGIC_ON_PLAYER, 85}, /* DataBlock1675.clscdcb044e86 Friend */
    {RS_C2S_CHAT, 87}, /* DataBlock1675.clscdcb044e88 Statics */
    {RS_C2S_NPC5, 88}, /* DataBlock1675.clscdcb044e89 Friend */
    {RS_C2S_OBJECT5, 90}, /* DataBlock1675.clscdcb044e91 Friend */
    {RS_C2S_ATTACK_PLAYER, 100}, /* DataBlock1675.clscdcb044e101 Friend */
    {RS_C2S_NO_TIMEOUT, 119}, /* DataBlock1675.clscdcb044e120 DataBlock1279 */
};

void rs_packets_client_876(void) {
    rs_pkt_table_load(876, k_c2s, RS_NELEM(k_c2s), RS_DIR_C2S);
}
