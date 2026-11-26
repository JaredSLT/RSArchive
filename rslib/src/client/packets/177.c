#include "both/packets_impl.h"

static const rs_pkt_bind_row k_c2s[] = {
    {RS_C2S_IDLE, 5},
    {RS_C2S_CHAT, 193},
    {RS_C2S_COMMAND, 253},
    {RS_C2S_WALK, 221},
    {RS_C2S_ADD_FRIEND, 26},
    {RS_C2S_DEL_FRIEND, 29},
    {RS_C2S_ADD_IGNORE, 27},
    {RS_C2S_DEL_IGNORE, 30},
    {RS_C2S_PRIVATE_MESSAGE, 28},
    {RS_C2S_IDLE_LOGOUT, 1},
    {RS_C2S_APPEARANCE, 218},
    {RS_C2S_CLOSE_INTERFACE, 233},
    {RS_C2S_TRADE, 232},
    {RS_C2S_DROP, 246},
    {RS_C2S_EQUIP, 249},
    {RS_C2S_OBJECT1, 224},
    {RS_C2S_OBJECT2, 250},
    {RS_C2S_OBJECT3, 252},
    {RS_C2S_NPC1, 245},
    {RS_C2S_ATTACK_NPC, 247},
    {RS_C2S_ATTACK_PLAYER, 241},
    {RS_C2S_FOLLOW, 214},
    {RS_C2S_GROUND_ITEM, 248},
};

void rs_packets_client_177(void) {
    rs_pkt_table_load(177, k_c2s, RS_NELEM(k_c2s), RS_DIR_C2S);
}
