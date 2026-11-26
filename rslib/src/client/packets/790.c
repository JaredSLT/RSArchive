#include "both/packets_impl.h"

/* Sizes from the client packet constructors. Missing opcodes are -3. */
const int8_t rs_sizes_c2s_790[256] = {
      -1,   -2,    4,   -1,   -1,    8,    6,   -1,   -1,   11,    3,   -2,    4,    8,    3,    8,
       0,   -1,   -2,    4,   -1,    0,    8,    8,    9,    2,    4,   -1,    9,   -1,   15,    8,
      -1,    5,    9,   -1,   -1,   -1,   -1,   15,    4,    9,    9,    3,   17,   -2,   -1,    8,
      -1,    3,    3,    3,   11,   -1,   -2,    6,    7,    0,    1,    6,    1,    9,   18,   12,
       7,    4,    3,   -1,    7,    8,    2,    1,    9,    3,   -2,    3,   -1,   -1,   -1,    4,
       3,    3,   -1,    3,    7,   -1,    0,    3,   -1,    4,    3,    8,    1,    4,   16,   16,
       4,   -1,   15,   -2,    3,    2,   -1,   -1,    3,    0,    0,    1,   -2,    3,   -2,    8,
       9,   -2,    2,    7,    7,   -1,    7,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,
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
    {RS_C2S_ENTER_AMOUNT, 2}, /* DataBlock1959.cls34becc0c3 MouseBinding */
    {RS_C2S_WINDOW_STATUS, 6}, /* DataBlock1959.cls34becc0c7 Client */
    {RS_C2S_MAGIC_ON_NPC, 9}, /* DataBlock1959.cls34becc0c10 LowResPlayerInfo */
    {RS_C2S_NPC1, 10}, /* DataBlock1959.cls34becc0c11 LowResPlayerInfo */
    {RS_C2S_PRIVATE_MESSAGE, 11}, /* DataBlock1959.cls34becc0c12 MouseBinding */
    {RS_C2S_CAMERA, 12}, /* DataBlock1959.cls34becc0c13 DataBlock2305 */
    {RS_C2S_BUTTON, 13}, /* DataBlock1959.cls34becc0c14 Client */
    {RS_C2S_BUTTON2, 15}, /* DataBlock1959.cls34becc0c16 Client */
    {RS_C2S_CHAT, 17}, /* DataBlock1959.cls34becc0c18 MouseBinding */
    {RS_C2S_NO_TIMEOUT, 21}, /* DataBlock1959.cls34becc0c22 DataBlock1836 */
    {RS_C2S_OBJECT2, 24}, /* DataBlock1959.cls34becc0c25 LowResPlayerInfo */
    {RS_C2S_OBJECT3, 28}, /* DataBlock1959.cls34becc0c29 LowResPlayerInfo */
    {RS_C2S_BUTTON5, 31}, /* DataBlock1959.cls34becc0c32 Client */
    {RS_C2S_WALK, 33}, /* DataBlock1959.cls34becc0c34 LowResPlayerInfo */
    {RS_C2S_OBJECT1, 34}, /* DataBlock1959.cls34becc0c35 LowResPlayerInfo */
    {RS_C2S_OBJECT4, 41}, /* DataBlock1959.cls34becc0c42 LowResPlayerInfo */
    {RS_C2S_OBJECT5, 42}, /* DataBlock1959.cls34becc0c43 LowResPlayerInfo */
    {RS_C2S_NPC3, 43}, /* DataBlock1959.cls34becc0c44 LowResPlayerInfo */
    {RS_C2S_MAGIC_ON_PLAYER, 52}, /* DataBlock1959.cls34becc0c53 LowResPlayerInfo */
    {RS_C2S_CLICK, 55}, /* DataBlock1959.cls34becc0c56 DataBlock2170 */
    {RS_C2S_WALK_MINIMAP, 62}, /* DataBlock1959.cls34becc0c63 LowResPlayerInfo */
    {RS_C2S_ATTACK_PLAYER, 66}, /* DataBlock1959.cls34becc0c67 LowResPlayerInfo */
    {RS_C2S_NPC2, 73}, /* DataBlock1959.cls34becc0c74 LowResPlayerInfo */
    {RS_C2S_TRADE, 75}, /* DataBlock1959.cls34becc0c76 LowResPlayerInfo */
    {RS_C2S_FOLLOW, 81}, /* DataBlock1959.cls34becc0c82 LowResPlayerInfo */
    {RS_C2S_NPC4, 90}, /* DataBlock1959.cls34becc0c91 LowResPlayerInfo */
    {RS_C2S_BUTTON4, 91}, /* DataBlock1959.cls34becc0c92 Client */
    {RS_C2S_NPC5, 104}, /* DataBlock1959.cls34becc0c105 LowResPlayerInfo */
    {RS_C2S_CLOSE_INTERFACE, 105}, /* DataBlock1959.cls34becc0c106 Client */
    {RS_C2S_FOCUS, 107}, /* DataBlock1959.cls34becc0c108 DataBlock2305 */
    {RS_C2S_BUTTON3, 111}, /* DataBlock1959.cls34becc0c112 Client */
    {RS_C2S_GROUND_ITEM, 115}, /* DataBlock1959.cls34becc0c116 LowResPlayerInfo */
};

void rs_packets_client_790(void) {
    rs_pkt_table_load(790, k_c2s, RS_NELEM(k_c2s), RS_DIR_C2S);
}
