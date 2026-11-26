#ifndef RSLIB_CAM_H
#define RSLIB_CAM_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 317 orbit camera from Client.updateOrbitCamera and Client.orbitCamera.
 * Angles are 0..2047. Pitch stays in [min_pitch, max_pitch], default 128..383.
 * The eye offset uses sin/cos of (2048 - angle). The view basis uses the
 * angle itself. Tables, when passed, are 2048 entries of sin/cos * 65536,
 * the same series as Pix3D (65536 * sin(i * 0.0030679615)).
 * Focus is in fine units (tile << 7). Yaw 0 leaves the eye at a smaller z
 * than the focus.
 */

typedef struct rs_cam_orbit {
    int yaw;
    int pitch;
    int yaw_vel;
    int pitch_vel;
    int focus_x;
    int focus_z;
    int min_pitch;
    int max_pitch;
} rs_cam_orbit;

typedef struct rs_cam_eye {
    int x;
    int y;
    int z;
    int pitch;
    int yaw;
    int sin_p;
    int cos_p;
    int sin_y;
    int cos_y;
} rs_cam_eye;

void rs_cam_orbit_init(rs_cam_orbit *o);

/* key_* are 1 while that arrow is held. Left/right are actionKey 1/2, up/down are 3/4. */
void rs_cam_orbit_keys(rs_cam_orbit *o, int key_left, int key_right, int key_up, int key_down);

/* Viewport drag. dx > 0 turns yaw the same way as the right arrow. dy > 0 is a downward drag and lowers pitch. */
void rs_cam_orbit_drag(rs_cam_orbit *o, int dx, int dy);

/* player_x/z are fine units. A gap wider than 500 snaps. Otherwise the focus steps by the difference / 16. */
void rs_cam_orbit_chase(rs_cam_orbit *o, int player_x, int player_z);

/* focus_y is the height under the focus, already adjusted (317 passes heightmap - 50). */
void rs_cam_orbit_place(const rs_cam_orbit *o, int focus_y, const int *sin_tab, const int *cos_tab,
                        rs_cam_eye *eye);

#ifdef __cplusplus
}
#endif

#endif
