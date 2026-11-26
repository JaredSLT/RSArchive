#ifndef RSLIB_PATH_H
#define RSLIB_PATH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * One mapsquare of collision flags produced by rslib/src/both/content.c.
 * Walking uses only the bits that file stamps:
 *   occupancy: 0x100 (loc) and 0x200000 (full block)
 *   walls from col_wall: 1, 2, 4, 8, 16, 32, 64, 128
 * Projectile bits (512 and above, other than 0x200000) do not block a step.
 *
 * Orthogonal step, matching col_wall's neighbour stamps:
 *   east  (+1, 0): source has bit 8,    or dest has bit 128
 *   west  (-1, 0): source has bit 128,  or dest has bit 8
 *   north (0, +1): source has bit 2,    or dest has bit 32
 *   south (0, -1): source has bit 32,   or dest has bit 2
 * A diagonal step is legal only when both orthogonal edges are open and the
 * destination is free. Corner cutting is rejected.
 * Destination occupancy (0x100 or 0x200000) always rejects the step.
 *
 * flags is row-major [z * stride + x], one plane. Returns the number of
 * tiles written, excluding the start, or -1 when no route fits in cap.
 * out_x/out_z receive the path from the first step through the goal.
 */
int rs_path_step_blocked(int from_flags, int to_flags, int dx, int dz);

int rs_path_find(const int *flags, int stride, int w, int h, int x0, int z0, int x1, int z1,
                 int *out_x, int *out_z, int cap);

#ifdef __cplusplus
}
#endif

#endif
