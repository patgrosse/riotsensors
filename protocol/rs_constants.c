/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include <rs_constants.h>
#include <stddef.h>

const char *stringify_rs_register_result(int8_t c) {
    static const char *strings[] = {NULL, "RS_REGISTER_INVALNAME", "RS_REGISTER_DUPLICATE", "RS_REGISTER_LIMIT_REACHED",
                                    "RS_REGISTER_NOMEM", "RS_REGISTER_INVALPARAM"};
    if (c < -5 || c > -1) {
        return NULL;
    } else {
        return strings[-c];
    }
}

const char *stringify_rs_unregister_result(int8_t c) {
    static const char *strings[] = {"RS_UNREGISTER_SUCCESS", "RS_UNREGISTER_NOTFOUND"};
    if (c < -1 || c > 0) {
        return NULL;
    } else {
        return strings[-c];
    }
}

const char *stringify_rs_call_result(int8_t c) {
    static const char *strings[] = {"RS_CALL_CACHE_TIMEOUT_EMPTY", "RS_CALL_CACHE_EMPTY", "RS_CALL_TIMEOUT",
                                    "RS_CALL_WRONGTYPE", "RS_CALL_NOTFOUND", "RS_CALL_SUCCESS", "RS_CALL_CACHE",
                                    "RS_CALL_CACHE_TIMEOUT"};
    if (c < -5 || c > 2) {
        return NULL;
    } else {
        return strings[c + 5];
    }
}
