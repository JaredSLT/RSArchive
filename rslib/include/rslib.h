#ifndef RSLIB_H
#define RSLIB_H

#include "rslib/both/err.h"
#include "rslib/both/build.h"
#include "rslib/both/buf.h"
#include "rslib/both/crypto.h"
#include "rslib/both/cache.h"
#include "rslib/both/packets.h"
#include "rslib/both/codec.h"
#include "rslib/both/cam.h"
#include "rslib/both/action.h"
#include "rslib/both/asset.h"
#include "rslib/both/content.h"
#include "rslib/both/dat.h"
#include "rslib/both/era_body.h"
#include "rslib/both/move.h"
#include "rslib/both/login_ext.h"
#include "rslib/both/rsc_game.h"
#include "rslib/both/sync.h"
#include "rslib/both/zonebuf.h"
#include "rslib/both/worldlist.h"
#include "rslib/both/pack.h"
#include "rslib/both/path.h"
#include "rslib/proto.h"
#include "rslib/client/client.h"
#include "rslib/client/packets.h"
#include "rslib/server/server.h"
#include "rslib/server/packets.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RSLIB_VERSION_MAJOR 0
#define RSLIB_VERSION_MINOR 6
#define RSLIB_VERSION_PATCH 0

const char *rs_version(void);

#ifdef __cplusplus
}
#endif

#endif
