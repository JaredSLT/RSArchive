#include "both/packets_impl.h"

/* Sizes from the client packet constructors. Missing opcodes are -3. */
const int8_t rs_sizes_c2s_887[256] = {
       0,    3,   -1,   11,    8,   -1,    1,    4,    4,    7,   -2,   -1,   -1,    3,   -2,    3,
       1,   -1,   -1,    1,    4,   -1,    8,    2,    8,    9,    3,   -1,    0,    2,    1,    3,
       7,    6,   -1,   -2,    3,    7,    8,   -1,    9,   18,    9,    3,    4,   16,    9,    3,
      -1,    9,    3,    9,   -1,    0,   -1,    8,    3,    3,    3,    4,    0,   -1,   -1,   -2,
      -2,    2,    4,   -1,   -1,   15,   -2,    3,   15,   17,    3,   -1,   -2,   -1,    8,    3,
      -1,    0,   11,   -1,    4,   -1,   16,    0,    8,    5,    4,    6,   12,    7,   -2,    8,
       1,    4,    8,   -1,    4,    9,   -1,   -1,    9,    5,    8,   -1,    4,    7,    6,   -1,
       3,   -2,    3,   -1,   -2,    7,    7,   18,   -2,   -2,    1,   -3,   -3,   -3,   -3,   -3,
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
    {RS_C2S_NPC2, 1}, /* DataBlock552.cls1e74bdac2 WorldMapRenderer */
    {RS_C2S_MAGIC_ON_NPC, 3}, /* DataBlock552.cls1e74bdac4 WorldMapRenderer */
    {RS_C2S_BUTTON2, 4}, /* DataBlock552.cls1e74bdac5 Client */
    {RS_C2S_FOCUS, 6}, /* DataBlock552.cls1e74bdac7 DataBlock741 */
    {RS_C2S_CAMERA, 7}, /* DataBlock552.cls1e74bdac8 DataBlock741 */
    {RS_C2S_NPC5, 15}, /* DataBlock552.cls1e74bdac16 WorldMapRenderer */
    {RS_C2S_CLOSE_INTERFACE, 28}, /* DataBlock552.cls1e74bdac29 Client */
    {RS_C2S_CHAT_MODE, 30}, /* DataBlock552.cls1e74bdac31 ColourUtils */
    {RS_C2S_NPC1, 31}, /* DataBlock552.cls1e74bdac32 WorldMapRenderer */
    {RS_C2S_CLICK, 33}, /* DataBlock552.cls1e74bdac34 ClientWorldMap */
    {RS_C2S_BUTTON4, 38}, /* DataBlock552.cls1e74bdac39 Client */
    {RS_C2S_OBJECT3, 40}, /* DataBlock552.cls1e74bdac41 WorldMapRenderer */
    {RS_C2S_OBJECT5, 42}, /* DataBlock552.cls1e74bdac43 WorldMapRenderer */
    {RS_C2S_TRADE, 47}, /* DataBlock552.cls1e74bdac48 WorldMapRenderer */
    {RS_C2S_OBJECT4, 49}, /* DataBlock552.cls1e74bdac50 WorldMapRenderer */
    {RS_C2S_BUTTON, 55}, /* DataBlock552.cls1e74bdac56 Client */
    {RS_C2S_NPC4, 56}, /* DataBlock552.cls1e74bdac57 WorldMapRenderer */
    {RS_C2S_ENTER_AMOUNT, 59}, /* DataBlock552.cls1e74bdac60 ColourUtils */
    {RS_C2S_NO_TIMEOUT, 60}, /* DataBlock552.cls1e74bdac61 DataBlock996 */
    {RS_C2S_ATTACK_PLAYER, 71}, /* DataBlock552.cls1e74bdac72 WorldMapRenderer */
    {RS_C2S_FOLLOW, 74}, /* DataBlock552.cls1e74bdac75 WorldMapRenderer */
    {RS_C2S_CHAT, 80}, /* DataBlock552.cls1e74bdac81 ColourUtils */
    {RS_C2S_MAGIC_ON_PLAYER, 82}, /* DataBlock552.cls1e74bdac83 WorldMapRenderer */
    {RS_C2S_WINDOW_STATUS, 91}, /* DataBlock552.cls1e74bdac92 Client */
    {RS_C2S_BUTTON3, 98}, /* DataBlock552.cls1e74bdac99 Client */
    {RS_C2S_OBJECT2, 101}, /* DataBlock552.cls1e74bdac102 WorldMapRenderer */
    {RS_C2S_OBJECT1, 104}, /* DataBlock552.cls1e74bdac105 WorldMapRenderer */
    {RS_C2S_WALK, 105}, /* DataBlock552.cls1e74bdac106 WorldMapRenderer */
    {RS_C2S_BUTTON5, 106}, /* DataBlock552.cls1e74bdac107 Client */
    {RS_C2S_PRIVATE_MESSAGE, 113}, /* DataBlock552.cls1e74bdac114 ColourUtils */
    {RS_C2S_NPC3, 114}, /* DataBlock552.cls1e74bdac115 WorldMapRenderer */
    {RS_C2S_GROUND_ITEM, 118}, /* DataBlock552.cls1e74bdac119 WorldMapRenderer */
    {RS_C2S_WALK_MINIMAP, 119}, /* DataBlock552.cls1e74bdac120 WorldMapRenderer */
};

void rs_packets_client_887(void) {
    rs_pkt_table_load(887, k_c2s, RS_NELEM(k_c2s), RS_DIR_C2S);
}
