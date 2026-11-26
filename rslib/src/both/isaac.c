#include "internal.h"

static void isaac_round(rs_isaac *is) {
    int i;
    is->c++;
    is->b += is->c;
    for (i = 0; i < 256; i++) {
        uint32_t x = is->mem[i];
        switch (i & 3) {
        case 0:
            is->a ^= is->a << 13;
            break;
        case 1:
            is->a ^= is->a >> 6;
            break;
        case 2:
            is->a ^= is->a << 2;
            break;
        default:
            is->a ^= is->a >> 16;
            break;
        }
        is->a += is->mem[(i + 128) & 255];
        {
            uint32_t y = is->mem[(x & 1020) >> 2] + is->a + is->b;
            is->mem[i] = y;
            is->rsl[i] = is->b = is->mem[(y >> 8 & 1020) >> 2] + x;
        }
    }
}

void rs_isaac_init(rs_isaac *is, const uint32_t seed[4]) {
    memset(is, 0, sizeof(*is));
    if (seed) {
        memcpy(is->rsl, seed, 4 * sizeof(uint32_t));
    }
    {
        int var9;
        uint32_t var1 = 0x9E3779B9u, var2 = var1, var3 = var1, var4 = var1;
        uint32_t var5 = var1, var6 = var1, var7 = var1, var8 = var1;
        for (var9 = 0; var9 < 4; var9++) {
            uint32_t var10 = var8 ^ (var7 << 11);
            uint32_t var11 = var5 + var10;
            uint32_t var12 = var7 + var6;
            uint32_t var13 = var12 ^ (var6 >> 2);
            uint32_t var14 = var4 + var13;
            uint32_t var15 = var6 + var11;
            uint32_t var16 = var15 ^ (var11 << 8);
            uint32_t var17 = var3 + var16;
            uint32_t var18 = var11 + var14;
            var5 = var18 ^ (var14 >> 16);
            {
                uint32_t var19 = var2 + var5;
                uint32_t var20 = var14 + var17;
                var4 = var20 ^ (var17 << 10);
                {
                    uint32_t var21 = var1 + var4;
                    uint32_t var22 = var17 + var19;
                    var3 = var22 ^ (var19 >> 4);
                    {
                        uint32_t var23 = var10 + var3;
                        uint32_t var24 = var19 + var21;
                        var2 = var24 ^ (var21 << 8);
                        var7 = var13 + var2;
                        {
                            uint32_t var25 = var21 + var23;
                            var1 = var25 ^ (var23 >> 9);
                            var6 = var16 + var1;
                            var8 = var23 + var7;
                        }
                    }
                }
            }
        }
        for (var9 = 0; var9 < 256; var9 += 8) {
            uint32_t var27 = var8 + is->rsl[var9];
            uint32_t var28 = var7 + is->rsl[var9 + 1];
            uint32_t var29 = var6 + is->rsl[var9 + 2];
            uint32_t var30 = var5 + is->rsl[var9 + 3];
            uint32_t var31 = var4 + is->rsl[var9 + 4];
            uint32_t var32 = var3 + is->rsl[var9 + 5];
            uint32_t var33 = var2 + is->rsl[var9 + 6];
            uint32_t var34 = var1 + is->rsl[var9 + 7];
            uint32_t var35 = var27 ^ (var28 << 11);
            uint32_t var36 = var30 + var35;
            uint32_t var37 = var28 + var29;
            uint32_t var38 = var37 ^ (var29 >> 2);
            uint32_t var39 = var31 + var38;
            uint32_t var40 = var29 + var36;
            uint32_t var41 = var40 ^ (var36 << 8);
            uint32_t var42 = var32 + var41;
            uint32_t var43 = var36 + var39;
            var5 = var43 ^ (var39 >> 16);
            {
                uint32_t var44 = var33 + var5;
                uint32_t var45 = var39 + var42;
                var4 = var45 ^ (var42 << 10);
                {
                    uint32_t var46 = var34 + var4;
                    uint32_t var47 = var42 + var44;
                    var3 = var47 ^ (var44 >> 4);
                    {
                        uint32_t var48 = var35 + var3;
                        uint32_t var49 = var44 + var46;
                        var2 = var49 ^ (var46 << 8);
                        var7 = var38 + var2;
                        {
                            uint32_t var50 = var46 + var48;
                            var1 = var50 ^ (var48 >> 9);
                            var6 = var41 + var1;
                            var8 = var48 + var7;
                            is->mem[var9] = var8;
                            is->mem[var9 + 1] = var7;
                            is->mem[var9 + 2] = var6;
                            is->mem[var9 + 3] = var5;
                            is->mem[var9 + 4] = var4;
                            is->mem[var9 + 5] = var3;
                            is->mem[var9 + 6] = var2;
                            is->mem[var9 + 7] = var1;
                        }
                    }
                }
            }
        }
        for (var9 = 0; var9 < 256; var9 += 8) {
            uint32_t var52 = var8 + is->mem[var9];
            uint32_t var53 = var7 + is->mem[var9 + 1];
            uint32_t var54 = var6 + is->mem[var9 + 2];
            uint32_t var55 = var5 + is->mem[var9 + 3];
            uint32_t var56 = var4 + is->mem[var9 + 4];
            uint32_t var57 = var3 + is->mem[var9 + 5];
            uint32_t var58 = var2 + is->mem[var9 + 6];
            uint32_t var59 = var1 + is->mem[var9 + 7];
            uint32_t var60 = var52 ^ (var53 << 11);
            uint32_t var61 = var55 + var60;
            uint32_t var62 = var53 + var54;
            uint32_t var63 = var62 ^ (var54 >> 2);
            uint32_t var64 = var56 + var63;
            uint32_t var65 = var54 + var61;
            uint32_t var66 = var65 ^ (var61 << 8);
            uint32_t var67 = var57 + var66;
            uint32_t var68 = var61 + var64;
            var5 = var68 ^ (var64 >> 16);
            {
                uint32_t var69 = var58 + var5;
                uint32_t var70 = var64 + var67;
                var4 = var70 ^ (var67 << 10);
                {
                    uint32_t var71 = var59 + var4;
                    uint32_t var72 = var67 + var69;
                    var3 = var72 ^ (var69 >> 4);
                    {
                        uint32_t var73 = var60 + var3;
                        uint32_t var74 = var69 + var71;
                        var2 = var74 ^ (var71 << 8);
                        var7 = var63 + var2;
                        {
                            uint32_t var75 = var71 + var73;
                            var1 = var75 ^ (var73 >> 9);
                            var6 = var66 + var1;
                            var8 = var73 + var7;
                            is->mem[var9] = var8;
                            is->mem[var9 + 1] = var7;
                            is->mem[var9 + 2] = var6;
                            is->mem[var9 + 3] = var5;
                            is->mem[var9 + 4] = var4;
                            is->mem[var9 + 5] = var3;
                            is->mem[var9 + 6] = var2;
                            is->mem[var9 + 7] = var1;
                        }
                    }
                }
            }
        }
        isaac_round(is);
        is->count = 256;
    }
}

uint32_t rs_isaac_next(rs_isaac *is) {
    if (is->count-- == 0) {
        isaac_round(is);
        is->count = 255;
    }
    return is->rsl[is->count];
}
