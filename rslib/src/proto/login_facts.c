#include "rslib/proto.h"

/* k_builds stays as registered. These two writers were counted from source
 * and do not match that field:
 *   530 SoundBank.java writes 28 opJs5MasterIndex20 CRCs (the p4s above
 *       that list are not CRCs).
 *   742 LoginManager.opLoginManager32 writes 37 checksum p4s.
 * 910 pushJS5CRCs walks 42 Js5Archive.values and skips LOADING_SPRITES,
 * which is already login_crc_count 41. */
int rs_login_crc_count(const rs_build_info *b) {
    int n;

    if (!b) {
        return 9;
    }
    if (b->revision == 530) {
        n = 28;
    } else if (b->revision == 742) {
        n = 37;
    } else {
        n = b->login_crc_count > 0 ? b->login_crc_count : 9;
    }
    if (n > RS_LOGIN_CRC_MAX) {
        n = RS_LOGIN_CRC_MAX;
    }
    if (n < 0) {
        n = 0;
    }
    return n;
}
