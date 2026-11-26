#include "rslib/both/login_ext.h"

rs_err rs_login_facts(int revision, int *crc_count, int *success_extra, int *user37) {
    int crc = 0;
    int extra = 0;
    int u37 = 0;

    switch (revision) {
    case 317:
        /* Client.java login: for (var11 < 9) p4 checksum. RSA is p4 uid +
         * pjstr user + pjstr pass. After status 2: rights, flagged, game. */
        crc = 9;
        extra = 0;
        u37 = 0;
        break;
    case 474:
        /* TextureOp29SubOp: 16 checksum p4s. RSA is p8 base37 + pjstr password.
         * Login-ok reads rights, flagged, u16 index, one more byte, then
         * the ISAAC opcode. Plaintext before ISAAC is 5, so extra is 3. */
        crc = 16;
        extra = 3;
        u37 = 1;
        break;
    case 508:
        /* Cf7f6c0c6: 29 checksum p4s (size constant 151). RSA is p8 username.
         * Success state 8 reads 8 plaintext bytes before the ISAAC opcode
         * (5 g1 + g2 + g1), so extra is 6. */
        crc = 29;
        extra = 6;
        u37 = 1;
        break;
    case 530:
        /* SoundBank.java: 28 opJs5MasterIndex20 p4s. The p4s above that loop
         * are affiliate and PlayerEntity, and the following p2 is not a CRC.
         * RSA is p8 username. Success state 8 reads 11 plaintext bytes
         * (9 g1 + g2) before the ISAAC opcode, so extra is 9. */
        crc = 28;
        extra = 9;
        u37 = 1;
        break;
    case 742:
        /* LoginManager.opLoginManager32 writes 37 checksum p4s. Username is
         * outside the RSA block (pjstr or p8 socialKey). Login-ok (state 136)
         * is a u8 length plus a variable record, not a fixed trailer. */
        crc = 37;
        extra = 0;
        u37 = 0;
        break;
    case 910:
        /* pushJS5CRCs walks Js5Archive.values() (42) and skips
         * LOADING_SPRITES, so 41 p4s. Username is outside RSA. After status
         * 2 the game reads a u16 block and the lobby reads a u8 length plus
         * a variable record. Neither is a fixed rights+flagged trailer. */
        crc = 41;
        extra = 0;
        u37 = 0;
        break;
    default:
        /* 695's CRC loop bound is obfuscated. 831 and 860 have no sources. */
        return RS_ERR_NOT_FOUND;
    }
    if (crc_count) {
        *crc_count = crc;
    }
    if (success_extra) {
        *success_extra = extra;
    }
    if (user37) {
        *user37 = u37;
    }
    return RS_OK;
}
