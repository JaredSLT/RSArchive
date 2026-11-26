#include "rslib.h"

#include <stdio.h>
#include <string.h>

static int g_fail;

static void fail(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    g_fail = 1;
}

static void expect(const char *what, int cond) {
    if (!cond) {
        fail(what);
    }
}

static int in_list(int rev, const int *ids, int n) {
    int i;
    for (i = 0; i < n; i++) {
        if (ids[i] == rev) {
            return 1;
        }
    }
    return 0;
}

int main(void) {
    rs_build_caps caps;
    const rs_build_info *known;
    size_t i, n;
    static const int no_cache[] = {695, 745, 790, 831, 860, 876, 887, 902, 910};
    static const int login_ok[] = {115, 159, 177, 254, 317, 377, 468, 474, 508, 530, 614, 667, 742};
    static const int region_ok[] = {317, 443, 468, 474, 481, 508, 525, 530, 578, 614, 667, 742, 910};
    static const int sync_ok[] = {254, 317, 350, 377};
    const rs_build_info *b317;
    const rs_build_info *b350;
    const rs_build_info *b578;
    const rs_build_info *b742;
    const rs_build_info *b910;

    rs_build_capabilities(NULL, NULL);
    memset(&caps, 0x5a, sizeof(caps));
    rs_build_capabilities(NULL, &caps);
    expect("null info zeroes caps", caps.frame_c2s == 0 && caps.frame_s2c == 0 && caps.named == 0 &&
                                        caps.region == 0 && caps.player_sync == 0 && caps.cache_on_disk == 0 &&
                                        caps.login_checked == 0);

    known = rs_build_known(&n);
    expect("known builds", known != NULL && n > 0);
    for (i = 0; known && i < n; i++) {
        int rev = known[i].revision;
        char msg[80];
        rs_build_capabilities(&known[i], &caps);
        snprintf(msg, sizeof(msg), "rev %d cache_on_disk", rev);
        expect(msg, caps.cache_on_disk == !in_list(rev, no_cache, (int)(sizeof(no_cache) / sizeof(no_cache[0]))));
        snprintf(msg, sizeof(msg), "rev %d login_checked", rev);
        expect(msg, caps.login_checked == in_list(rev, login_ok, (int)(sizeof(login_ok) / sizeof(login_ok[0]))));
        snprintf(msg, sizeof(msg), "rev %d region", rev);
        expect(msg, caps.region == in_list(rev, region_ok, (int)(sizeof(region_ok) / sizeof(region_ok[0]))));
        snprintf(msg, sizeof(msg), "rev %d player_sync", rev);
        expect(msg, caps.player_sync == in_list(rev, sync_ok, (int)(sizeof(sync_ok) / sizeof(sync_ok[0]))));
        if (known[i].era == RS_ERA_RSC) {
            snprintf(msg, sizeof(msg), "rev %d rsc frames", rev);
            expect(msg, caps.frame_c2s == 1 && caps.frame_s2c == 1);
        } else {
            snprintf(msg, sizeof(msg), "rev %d frames follow size tables", rev);
            expect(msg, caps.frame_c2s == (known[i].sizes_c2s != NULL) && caps.frame_s2c == (known[i].sizes_s2c != NULL));
        }
    }

    b317 = rs_build_lookup(317);
    b350 = rs_build_lookup(350);
    b578 = rs_build_lookup(578);
    b742 = rs_build_lookup(742);
    b910 = rs_build_lookup(910);
    rs_build_capabilities(b317, &caps);
    expect("317 all caps", caps.frame_c2s && caps.frame_s2c && caps.named && caps.region && caps.player_sync &&
                               caps.cache_on_disk && caps.login_checked);
    rs_build_capabilities(b350, &caps);
    expect("350 has no c2s size table", caps.frame_c2s == 0 && caps.frame_s2c == 1);
    rs_build_capabilities(b578, &caps);
    /* Frames stay 0: 578 has no size table. Region is 1 because opkind9583 was read. */
    expect("578 frames are closed", caps.frame_c2s == 0 && caps.frame_s2c == 0 && caps.region == 1);
    rs_build_capabilities(b742, &caps);
    expect("742 region was read", caps.region == 1);
    rs_build_capabilities(b910, &caps);
    expect("910 cache absent and region read", caps.cache_on_disk == 0 && caps.region == 1 && caps.player_sync == 0 &&
                                                   caps.login_checked == 0);
    return g_fail ? 1 : 0;
}
