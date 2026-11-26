#include "rslib/both/path.h"

#include <stdlib.h>

/*
 * Walking bits stamped by col_wall / col_loc / col_block.
 * Diagonal wall pairs are the type-1/3 stamps: the bit on the source tile
 * and the bit on the diagonal neighbour. Orthogonal edges use bits 2, 8, 32,
 * and 128. Projectile bits are ignored. 0x100 and 0x200000 reject entry.
 * A diagonal also needs both side tiles, so a corner cannot be cut.
 */

enum {
    WALL_NW = 1,
    WALL_N = 2,
    WALL_NE = 4,
    WALL_E = 8,
    WALL_SE = 16,
    WALL_S = 32,
    WALL_SW = 64,
    WALL_W = 128,
    OCC_LOC = 0x100,
    OCC_BLOCK = 0x200000,
    OCC_MASK = OCC_LOC | OCC_BLOCK
};

static int ortho_wall(unsigned from, unsigned to, int dx, int dz) {
    if (dx == 1) {
        return (from & WALL_E) || (to & WALL_W);
    }
    if (dx == -1) {
        return (from & WALL_W) || (to & WALL_E);
    }
    if (dz == 1) {
        return (from & WALL_N) || (to & WALL_S);
    }
    if (dz == -1) {
        return (from & WALL_S) || (to & WALL_N);
    }
    return 1;
}

static int diag_wall(unsigned from, unsigned to, int dx, int dz) {
    if (dx == 1 && dz == 1) {
        return (from & WALL_NE) || (to & WALL_SW);
    }
    if (dx == -1 && dz == 1) {
        return (from & WALL_NW) || (to & WALL_SE);
    }
    if (dx == 1 && dz == -1) {
        return (from & WALL_SE) || (to & WALL_NW);
    }
    if (dx == -1 && dz == -1) {
        return (from & WALL_SW) || (to & WALL_NE);
    }
    return 1;
}

int rs_path_step_blocked(int from_flags, int to_flags, int dx, int dz) {
    unsigned from = (unsigned)from_flags;
    unsigned to = (unsigned)to_flags;

    if (dx < -1 || dx > 1 || dz < -1 || dz > 1 || (dx == 0 && dz == 0)) {
        return 1;
    }
    if (to & OCC_MASK) {
        return 1;
    }
    if (dx == 0 || dz == 0) {
        return ortho_wall(from, to, dx, dz);
    }
    if (diag_wall(from, to, dx, dz)) {
        return 1;
    }
    /* Halves of the four corner edges that live on these two tiles. */
    if (dx == 1 && (from & WALL_E)) {
        return 1;
    }
    if (dx == -1 && (from & WALL_W)) {
        return 1;
    }
    if (dz == 1 && (from & WALL_N)) {
        return 1;
    }
    if (dz == -1 && (from & WALL_S)) {
        return 1;
    }
    if (dx == 1 && (to & WALL_W)) {
        return 1;
    }
    if (dx == -1 && (to & WALL_E)) {
        return 1;
    }
    if (dz == 1 && (to & WALL_S)) {
        return 1;
    }
    if (dz == -1 && (to & WALL_N)) {
        return 1;
    }
    return 0;
}

static int in_map(int x, int z, int w, int h) {
    return x >= 0 && z >= 0 && x < w && z < h;
}

static int flag_at(const int *flags, int stride, int x, int z) {
    return flags[(size_t)z * (size_t)stride + (size_t)x];
}

static int can_step(const int *flags, int stride, int w, int h, int x, int z, int dx, int dz) {
    int nx = x + dx;
    int nz = z + dz;
    int from;
    int to;

    if (!in_map(x, z, w, h) || !in_map(nx, nz, w, h)) {
        return 0;
    }
    from = flag_at(flags, stride, x, z);
    to = flag_at(flags, stride, nx, nz);
    if (rs_path_step_blocked(from, to, dx, dz)) {
        return 0;
    }
    if (dx != 0 && dz != 0) {
        int sx = x + dx;
        int sz = z;
        int tx = x;
        int tz = z + dz;
        int side_h;
        int side_v;
        /* Both orthogonal edges, including the far side stored on the side tiles. */
        side_h = flag_at(flags, stride, sx, sz);
        side_v = flag_at(flags, stride, tx, tz);
        if (rs_path_step_blocked(from, side_h, dx, 0)) {
            return 0;
        }
        if (rs_path_step_blocked(from, side_v, 0, dz)) {
            return 0;
        }
        if (rs_path_step_blocked(side_h, to, 0, dz)) {
            return 0;
        }
        if (rs_path_step_blocked(side_v, to, dx, 0)) {
            return 0;
        }
    }
    return 1;
}

int rs_path_find(const int *flags, int stride, int w, int h, int x0, int z0, int x1, int z1,
                 int *out_x, int *out_z, int cap) {
    static const int DX[8] = {1, -1, 0, 0, 1, 1, -1, -1};
    static const int DZ[8] = {0, 0, 1, -1, 1, -1, 1, -1};
    size_t cells;
    size_t qh;
    size_t qt;
    int *qx = NULL;
    int *qz = NULL;
    int *par = NULL;
    unsigned char *seen = NULL;
    int found = -1;
    int len;
    int cur;
    int idx;

    if (!flags || w <= 0 || h <= 0 || stride <= 0 || cap < 0) {
        return -1;
    }
    if ((size_t)h > (size_t)-1 / (size_t)stride) {
        return -1;
    }
    if ((size_t)w > (size_t)-1 / (size_t)h || (size_t)w * (size_t)h > (size_t)0x7fffffff) {
        return -1;
    }
    if (!in_map(x0, z0, w, h) || !in_map(x1, z1, w, h)) {
        return -1;
    }
    if (x0 == x1 && z0 == z1) {
        return 0;
    }

    cells = (size_t)w * (size_t)h;
    qx = malloc(cells * sizeof(*qx));
    qz = malloc(cells * sizeof(*qz));
    par = malloc(cells * sizeof(*par));
    seen = calloc(cells, 1);
    if (!qx || !qz || !par || !seen) {
        free(qx);
        free(qz);
        free(par);
        free(seen);
        return -1;
    }

    qx[0] = x0;
    qz[0] = z0;
    par[0] = -1;
    seen[(size_t)z0 * (size_t)w + (size_t)x0] = 1;
    qh = 0;
    qt = 1;
    while (qh < qt) {
        int x = qx[qh];
        int z = qz[qh];
        int dir;
        for (dir = 0; dir < 8; dir++) {
            int nx = x + DX[dir];
            int nz = z + DZ[dir];
            size_t si;
            if (!can_step(flags, stride, w, h, x, z, DX[dir], DZ[dir])) {
                continue;
            }
            si = (size_t)nz * (size_t)w + (size_t)nx;
            if (seen[si]) {
                continue;
            }
            if (qt >= cells) {
                continue;
            }
            seen[si] = 1;
            qx[qt] = nx;
            qz[qt] = nz;
            par[qt] = (int)qh;
            if (nx == x1 && nz == z1) {
                found = (int)qt;
            }
            qt++;
            if (found >= 0) {
                break;
            }
        }
        if (found >= 0) {
            break;
        }
        qh++;
    }

    if (found < 0) {
        len = -1;
    } else {
        len = 0;
        for (cur = found; par[cur] >= 0; cur = par[cur]) {
            len++;
        }
        if (len > cap || !out_x || !out_z) {
            len = -1;
        } else {
            idx = len;
            for (cur = found; par[cur] >= 0; cur = par[cur]) {
                idx--;
                out_x[idx] = qx[cur];
                out_z[idx] = qz[cur];
            }
        }
    }

    free(qx);
    free(qz);
    free(par);
    free(seen);
    return len;
}
