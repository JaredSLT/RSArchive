#include "both/packets_impl.h"

/* Sizes from the client packet constructors. Missing opcodes are -3. */
const int8_t rs_sizes_c2s_902[256] = {
       4,    2,   -1,   -1,    8,    3,    6,    3,    8,    2,   -1,   -1,   -1,    4,   -2,    7,
       9,    8,    8,    9,   -1,   -1,    3,    7,    7,   -2,   11,    7,   -2,    3,    4,   -1,
       9,   -2,    3,   -2,   -2,   -1,   -2,   -1,    3,    5,    4,   -1,   -1,    3,    8,   -1,
      -2,    6,   -2,    9,   12,    3,    8,    0,   15,   17,    3,    0,   11,    4,    0,   -2,
       9,    0,    8,    4,   -2,   -1,   -1,    6,    9,    4,   -1,   -1,   15,    7,   18,   -1,
       3,    4,   -1,    3,    7,   -1,    8,    3,    1,   -1,    1,    7,    1,    1,    3,   -1,
       0,   -1,   -2,   18,    1,   -1,    3,    9,   -1,    2,    4,    4,   -1,    0,    3,   -1,
       3,    3,   16,    8,    8,    1,    4,    5,    9,   -1,   16,   -3,   -3,   -3,   -3,   -3,
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
    {RS_C2S_ENTER_AMOUNT, 0}, /* DataBlock420.clse7da79ad VarClanSettingsDomain */
    {RS_C2S_BUTTON, 4}, /* DataBlock420.clse7da79ad5 Client */
    {RS_C2S_FOLLOW, 5}, /* DataBlock420.clse7da79ad6 AccountAppealLogin */
    {RS_C2S_BUTTON3, 8}, /* DataBlock420.clse7da79ad9 Client */
    {RS_C2S_OBJECT3, 16}, /* DataBlock420.clse7da79ad17 AccountAppealLogin */
    {RS_C2S_BUTTON4, 17}, /* DataBlock420.clse7da79ad18 Client */
    {RS_C2S_BUTTON5, 18}, /* DataBlock420.clse7da79ad19 Client */
    {RS_C2S_OBJECT2, 19}, /* DataBlock420.clse7da79ad20 AccountAppealLogin */
    {RS_C2S_GROUND_ITEM, 24}, /* DataBlock420.clse7da79ad25 AccountAppealLogin */
    {RS_C2S_MAGIC_ON_PLAYER, 26}, /* DataBlock420.clse7da79ad27 AccountAppealLogin */
    {RS_C2S_CHAT, 31}, /* DataBlock420.clse7da79ad32 VarClanSettingsDomain */
    {RS_C2S_OBJECT1, 32}, /* DataBlock420.clse7da79ad33 AccountAppealLogin */
    {RS_C2S_NPC4, 34}, /* DataBlock420.clse7da79ad35 AccountAppealLogin */
    {RS_C2S_PRIVATE_MESSAGE, 36}, /* DataBlock420.clse7da79ad37 VarClanSettingsDomain */
    {RS_C2S_WALK, 41}, /* DataBlock420.clse7da79ad42 AccountAppealLogin */
    {RS_C2S_ATTACK_PLAYER, 45}, /* DataBlock420.clse7da79ad46 AccountAppealLogin */
    {RS_C2S_CLICK, 49}, /* DataBlock420.clse7da79ad50 SubInterface */
    {RS_C2S_NPC1, 53}, /* DataBlock420.clse7da79ad54 AccountAppealLogin */
    {RS_C2S_NO_TIMEOUT, 59}, /* DataBlock420.clse7da79ad60 RebuildType */
    {RS_C2S_MAGIC_ON_NPC, 60}, /* DataBlock420.clse7da79ad61 AccountAppealLogin */
    {RS_C2S_CLOSE_INTERFACE, 62}, /* DataBlock420.clse7da79ad63 Client */
    {RS_C2S_OBJECT5, 64}, /* DataBlock420.clse7da79ad65 AccountAppealLogin */
    {RS_C2S_CAMERA, 67}, /* DataBlock420.clse7da79ad68 CursorTypeFactory */
    {RS_C2S_WINDOW_STATUS, 71}, /* DataBlock420.clse7da79ad72 Client */
    {RS_C2S_WALK_MINIMAP, 78}, /* DataBlock420.clse7da79ad79 AccountAppealLogin */
    {RS_C2S_NPC3, 80}, /* DataBlock420.clse7da79ad81 AccountAppealLogin */
    {RS_C2S_NPC5, 83}, /* DataBlock420.clse7da79ad84 AccountAppealLogin */
    {RS_C2S_CHAT_MODE, 88}, /* DataBlock420.clse7da79ad89 VarClanSettingsDomain */
    {RS_C2S_FOCUS, 90}, /* DataBlock420.clse7da79ad91 CursorTypeFactory */
    {RS_C2S_TRADE, 102}, /* DataBlock420.clse7da79ad103 AccountAppealLogin */
    {RS_C2S_OBJECT4, 103}, /* DataBlock420.clse7da79ad104 AccountAppealLogin */
    {RS_C2S_NPC2, 113}, /* DataBlock420.clse7da79ad114 AccountAppealLogin */
    {RS_C2S_BUTTON2, 115}, /* DataBlock420.clse7da79ad116 Client */
};

void rs_packets_client_902(void) {
    rs_pkt_table_load(902, k_c2s, RS_NELEM(k_c2s), RS_DIR_C2S);
}
