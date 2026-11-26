/*
 * c2s size search. No rs_sizes_c2s_* array was added.
 *
 * 282, 350, 443, 481, 525: each src tree has one int[256] and it is already
 * rs_sizes_s2c_*. The second array (valuekind181 / valuekind1191 /
 * clientProtScrambled) is a 0..255 permutation, not sizes. No ClientProt
 * (opcode, size) constructor list. Outgoing length is implicit at each
 * p1isaac write. A zero-filled table would mark every opcode as an empty
 * packet, so sizes_c2s stays NULL.
 *
 * 578, 634, 695: the decompiled gamepack was searched for new int[256],
 * int[] initialisers containing -1/-2, and (opcode, size) constructors.
 * None of those is a packet size array. sizes stay NULL.
 *
 * 831, 860: no sources, only an outer jar. Not searched for a table that
 * is not there. sizes stay NULL.
 *
 * 614, 667, 745, 790, 876, 887, 902 already have size tables. Their
 * ClientProt/ServerProt class names are generify mis-names (not opcode
 * lists). The real constructor lists have no stable field names, so no
 * rs_pkt was bound.
 *
 * Opcode names seen and not bound, because no rs_pkt is that packet:
 *   RSC walk-here / minimap click: 115 opcode 255, 159 and 177 opcode 194.
 *     RS_C2S_WALK stays 221 (shared cast-on-ground writer). The same
 *     writer serves both "Walk here" and the minimap, so it is not
 *     RS_C2S_WALK_MINIMAP alone.
 *   RSC approach walk, opcode 215 in 115, 159, and 177.
 *   177 login text "Please wait...", opcode 193. 115 and 159 do not write it.
 *   159 and 177 contact-details form, opcode 253.
 *   159 and 177 privacy-menu click, opcode 247.
 *   159 and 177 configurable NPC option (ClientConfig.npcOption, action
 *     725), opcode 195. Absent in 115. The label is config text, not NPC2.
 */

const char *rs_packet_wire_report(void) {
    return
        "c2s tables added: none\n"
        "searched, no c2s table: 282 350 443 481 525 "
        "(only the existing s2c int[256]; the other array is an opcode scramble)\n"
        "searched, no size table: 578 634 695\n"
        "no sources: 831 860\n"
        "seen, not bound: "
        "walk-here/minimap (115=255, 159/177=194); "
        "approach-walk (215); "
        "login-text (177=193); "
        "contact-details (159/177=253); "
        "privacy-click (159/177=247); "
        "npc-option (159/177=195)\n";
}
