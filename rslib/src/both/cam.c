#include "rslib/both/cam.h"

#include <math.h>
#include <string.h>

static int cam_sin(const int *tab, int angle) {
    angle &= 2047;
    if (tab) {
        return tab[angle];
    }
    return (int)(65536.0 * sin((double)angle * 0.0030679615));
}

static int cam_cos(const int *tab, int angle) {
    angle &= 2047;
    if (tab) {
        return tab[angle];
    }
    return (int)(65536.0 * cos((double)angle * 0.0030679615));
}

static void cam_clamp_pitch(rs_cam_orbit *o) {
    if (o->pitch < o->min_pitch) {
        o->pitch = o->min_pitch;
    }
    if (o->pitch > o->max_pitch) {
        o->pitch = o->max_pitch;
    }
}

void rs_cam_orbit_init(rs_cam_orbit *o) {
    if (!o) {
        return;
    }
    memset(o, 0, sizeof *o);
    o->pitch = 128;
    o->min_pitch = 128;
    o->max_pitch = 383;
}

void rs_cam_orbit_keys(rs_cam_orbit *o, int key_left, int key_right, int key_up, int key_down) {
    if (!o) {
        return;
    }
    if (key_left) {
        o->yaw_vel += (-24 - o->yaw_vel) / 2;
    } else if (key_right) {
        o->yaw_vel += (24 - o->yaw_vel) / 2;
    } else {
        o->yaw_vel /= 2;
    }
    if (key_up) {
        o->pitch_vel += (12 - o->pitch_vel) / 2;
    } else if (key_down) {
        o->pitch_vel += (-12 - o->pitch_vel) / 2;
    } else {
        o->pitch_vel /= 2;
    }
    o->yaw = (o->yaw + o->yaw_vel / 2) & 2047;
    o->pitch += o->pitch_vel / 2;
    cam_clamp_pitch(o);
}

void rs_cam_orbit_drag(rs_cam_orbit *o, int dx, int dy) {
    if (!o) {
        return;
    }
    o->yaw = (o->yaw + dx) & 2047;
    o->pitch -= dy;
    cam_clamp_pitch(o);
}

void rs_cam_orbit_chase(rs_cam_orbit *o, int player_x, int player_z) {
    int dx;
    int dz;

    if (!o) {
        return;
    }
    dx = player_x - o->focus_x;
    dz = player_z - o->focus_z;
    if (dx < -500 || dx > 500) {
        o->focus_x = player_x;
    } else if (o->focus_x != player_x) {
        o->focus_x += dx / 16;
    }
    if (dz < -500 || dz > 500) {
        o->focus_z = player_z;
    } else if (o->focus_z != player_z) {
        o->focus_z += dz / 16;
    }
}

void rs_cam_orbit_place(const rs_cam_orbit *o, int focus_y, const int *sin_tab, const int *cos_tab,
                        rs_cam_eye *eye) {
    int pitch;
    int yaw;
    int distance;
    int inv_pitch;
    int inv_yaw;
    int xoff;
    int yoff;
    int zoff;

    if (!eye) {
        return;
    }
    memset(eye, 0, sizeof *eye);
    if (!o) {
        return;
    }
    pitch = o->pitch;
    yaw = o->yaw & 2047;
    if (pitch < o->min_pitch) {
        pitch = o->min_pitch;
    }
    if (pitch > o->max_pitch) {
        pitch = o->max_pitch;
    }
    distance = 600 + pitch * 3;
    inv_pitch = (2048 - pitch) & 2047;
    inv_yaw = (2048 - yaw) & 2047;
    xoff = 0;
    yoff = 0;
    zoff = distance;
    if (inv_pitch != 0) {
        int sp = cam_sin(sin_tab, inv_pitch);
        int cp = cam_cos(cos_tab, inv_pitch);
        yoff = (0 * cp - distance * sp) >> 16;
        zoff = (0 * sp + distance * cp) >> 16;
    }
    if (inv_yaw != 0) {
        int sy = cam_sin(sin_tab, inv_yaw);
        int cy = cam_cos(cos_tab, inv_yaw);
        int nx = (zoff * sy) >> 16;
        zoff = (zoff * cy) >> 16;
        xoff = nx;
    }
    eye->x = o->focus_x - xoff;
    eye->y = focus_y - yoff;
    eye->z = o->focus_z - zoff;
    eye->pitch = pitch;
    eye->yaw = yaw;
    eye->sin_p = cam_sin(sin_tab, pitch);
    eye->cos_p = cam_cos(cos_tab, pitch);
    eye->sin_y = cam_sin(sin_tab, yaw);
    eye->cos_y = cam_cos(cos_tab, yaw);
}
