#ifndef RSLIB_ERR_H
#define RSLIB_ERR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum rs_err {
    RS_OK = 0,
    RS_ERR_NOMEM = -1,
    RS_ERR_IO = -2,
    RS_ERR_FORMAT = -3,
    RS_ERR_UNSUPPORTED = -4,
    RS_ERR_NOT_FOUND = -5,
    RS_ERR_RANGE = -6,
    RS_ERR_CRC = -7,
    RS_ERR_VERSION = -8,
    RS_ERR_PROTO = -9,
    RS_ERR_ARG = -10,
    RS_ERR_NEED_MORE = -11
} rs_err;

const char *rs_strerror(rs_err err);

#ifdef __cplusplus
}
#endif

#endif
