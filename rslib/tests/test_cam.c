#include "rslib/both/cam.h"

#include <stdio.h>

static int g_fail;

static void expect(int cond, const char *msg) {
    if (!cond) {
        fprintf(stderr, "fail: %s\n", msg);
        g_fail = 1;
    }
}

int main(void) {
    rs_cam_orbit orbit;
    rs_cam_eye eye;
    int yaw_before;

    rs_cam_orbit_init(&orbit);
    expect(orbit.pitch == 128, "init pitch");
    expect(orbit.yaw == 0, "init yaw");
    orbit.focus_x = 1000;
    orbit.focus_z = 2000;
    rs_cam_orbit_place(&orbit, 500, NULL, NULL, &eye);
    expect(eye.x == orbit.focus_x, "yaw 0 keeps eye on the focus x");
    expect(eye.z < orbit.focus_z, "yaw 0 places the eye at a smaller z");
    expect(eye.pitch == 128, "placed pitch");
    expect(eye.sin_p != 0 && eye.cos_p != 0, "pitch basis");

    rs_cam_orbit_init(&orbit);
    rs_cam_orbit_keys(&orbit, 0, 1, 0, 0);
    expect(orbit.yaw > 0, "right arrow increases yaw");
    yaw_before = orbit.yaw;
    rs_cam_orbit_keys(&orbit, 1, 0, 0, 0);
    expect(orbit.yaw < yaw_before, "left arrow decreases yaw");

    rs_cam_orbit_init(&orbit);
    rs_cam_orbit_keys(&orbit, 0, 0, 0, 1);
    expect(orbit.pitch == 128, "pitch stays at the low clamp");
    rs_cam_orbit_keys(&orbit, 0, 0, 1, 0);
    expect(orbit.pitch > 128, "up arrow raises pitch");

    rs_cam_orbit_init(&orbit);
    orbit.pitch = 383;
    rs_cam_orbit_keys(&orbit, 0, 0, 1, 0);
    expect(orbit.pitch == 383, "pitch stays at the high clamp");

    rs_cam_orbit_init(&orbit);
    rs_cam_orbit_drag(&orbit, 30, -10);
    expect(orbit.yaw == 30, "drag right increases yaw");
    expect(orbit.pitch == 138, "drag up raises pitch");
    rs_cam_orbit_drag(&orbit, -40, 0);
    expect(orbit.yaw == (30 - 40 + 2048), "yaw wraps");

    rs_cam_orbit_init(&orbit);
    orbit.focus_x = 0;
    orbit.focus_z = 0;
    rs_cam_orbit_chase(&orbit, 2880, 2880);
    expect(orbit.focus_x == 2880 && orbit.focus_z == 2880, "chase snaps past 500");
    rs_cam_orbit_chase(&orbit, 2896, 2880);
    expect(orbit.focus_x == 2881, "chase steps by the difference / 16");
    expect(orbit.focus_z == 2880, "chase leaves a matched axis");

    return g_fail ? 1 : 0;
}
