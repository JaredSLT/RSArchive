#include "both/packets_impl.h"

/* Sizes from the client packet constructors. Missing opcodes are -3. */
const int8_t rs_sizes_c2s_745[256] = {
      16,   12,   -1,    7,    1,    4,    2,   -1,   -1,    6,    3,   -2,    3,   16,    8,    3,
       8,    0,    0,    4,    0,    3,   -2,    1,    9,    9,   -1,    7,    7,    7,    8,   -1,
      -1,   -1,    1,   15,    4,   -1,    8,    8,    9,    8,   -1,   15,    6,   -1,    8,    9,
       2,    4,    6,   17,   -1,   -1,   -1,    3,   -2,    8,    4,    7,    3,   -1,    3,    2,
       3,    1,   -1,    4,    9,    3,   -1,   -1,    5,    9,    4,   -1,   11,    3,   18,   -1,
       3,   15,    3,   -1,   -2,   11,   -1,    3,    7,    3,   -1,    2,   -1,   -2,    9,    0,
       3,    7,    8,    3,    1,   -2,    8,    4,    4,   -2,    4,   -1,    3,   -2,   -3,   -3,
      -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,
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
    {RS_C2S_CLICK, 9}, /* C10c700cf.valuekind337010 Ca1ac0f8e */
    {RS_C2S_BUTTON3, 14}, /* C10c700cf.valuekind337015 Client.opClient537 */
    {RS_C2S_NPC2, 15}, /* C10c700cf.valuekind337052 MiniMenu */
    {RS_C2S_BUTTON4, 16}, /* C10c700cf.valuekind337017 Client.opClient537 */
    {RS_C2S_CLOSE_INTERFACE, 17}, /* C10c700cf.valuekind337090 Client.opClient454 */
    {RS_C2S_NO_TIMEOUT, 20}, /* C10c700cf.valuekind337021 Cf9cb6c0f14 */
    {RS_C2S_NPC4, 21}, /* C10c700cf.valuekind337022 MiniMenu */
    {RS_C2S_OBJECT2, 25}, /* C10c700cf.valuekind337026 MiniMenu */
    {RS_C2S_CHAT, 31}, /* C10c700cf.valuekind337074 ScriptRunner */
    {RS_C2S_COMMAND, 37}, /* C10c700cf.valuekind337038 DeveloperConsole */
    {RS_C2S_BUTTON, 38}, /* C10c700cf.valuekind337039 Client.opClient537 */
    {RS_C2S_OBJECT3, 40}, /* C10c700cf.valuekind3370102 MiniMenu */
    {RS_C2S_BUTTON2, 46}, /* C10c700cf.valuekind337018 Client.opClient537 */
    {RS_C2S_OBJECT4, 47}, /* C10c700cf.valuekind337047 MiniMenu */
    {RS_C2S_WINDOW_STATUS, 50}, /* C10c700cf.valuekind3370110 Client.opClient522 */
    {RS_C2S_ENTER_AMOUNT, 58}, /* C10c700cf.valuekind33706 ScriptRunner */
    {RS_C2S_ATTACK_PLAYER, 62}, /* C10c700cf.valuekind337063 MiniMenu */
    {RS_C2S_FOLLOW, 64}, /* C10c700cf.valuekind337065 MiniMenu */
    {RS_C2S_CHAT_MODE, 65}, /* C10c700cf.valuekind337066 ScriptRunner */
    {RS_C2S_OBJECT5, 68}, /* C10c700cf.valuekind337037 MiniMenu */
    {RS_C2S_WALK, 72}, /* C10c700cf.valuekind337073 Statics.opStatics116 */
    {RS_C2S_OBJECT1, 73}, /* C10c700cf.valuekind337081 MiniMenu */
    {RS_C2S_CAMERA, 74}, /* C10c700cf.valuekind337075 ClientWatch */
    {RS_C2S_MAGIC_ON_PLAYER, 76}, /* C10c700cf.valuekind337077 MiniMenu */
    {RS_C2S_NPC3, 77}, /* C10c700cf.valuekind337078 MiniMenu */
    {RS_C2S_WALK_MINIMAP, 78}, /* C10c700cf.valuekind337079 Statics.opStatics116 */
    {RS_C2S_TRADE, 80}, /* C10c700cf.valuekind337036 MiniMenu */
    {RS_C2S_PRIVATE_MESSAGE, 84}, /* C10c700cf.valuekind337085 ScriptRunner */
    {RS_C2S_MAGIC_ON_NPC, 85}, /* C10c700cf.valuekind337086 MiniMenu */
    {RS_C2S_NPC5, 89}, /* C10c700cf.valuekind33709 MiniMenu */
    {RS_C2S_GROUND_ITEM, 97}, /* C10c700cf.valuekind337098 MiniMenu */
    {RS_C2S_BUTTON5, 98}, /* C10c700cf.valuekind337099 Client.opClient537 */
    {RS_C2S_NPC1, 99}, /* C10c700cf.valuekind3370100 MiniMenu */
    {RS_C2S_FOCUS, 100}, /* C10c700cf.valuekind3370101 ClientWatch */
};

void rs_packets_client_745(void) {
    rs_pkt_table_load(745, k_c2s, RS_NELEM(k_c2s), RS_DIR_C2S);
}
