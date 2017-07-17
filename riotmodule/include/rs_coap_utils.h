/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

/**
 * @brief   CoAP utility functions
 * @file    rs_coap_utils.h
 * @author  Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#ifndef RIOTSENSORS_RS_COAP_UTILS_H
#define RIOTSENSORS_RS_COAP_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <sched.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief An URI query parameter consisting of a key and a value
 */
struct coap_query {
    /** @brief Key of the parameter, NULL terminated */
    char *key;
    /** @brief Value of the parameter, NULL terminated */
    char *value;
    /** @brief Next parameter in the URI if any */
    struct coap_query *next;
};

/**
 * @brief Query parameters of an URI
 */
struct coap_queries {
    /** @brief Number of query parameters */
    uint8_t num;
    /** @brief First query parameter if any */
    struct coap_query *first;
};

/**
 * @brief Split the query part of an URI into parameters
 *
 * @param len String length of the query
 * @param strquery The query
 * @param result A pointer where to store the results (already allocated)
 */
void split_coap_query(size_t len, const char *strquery, struct coap_queries *result);

/**
 * @brief Free the results of an URI query split
 *
 * @param queries Result to free
 */
void free_coap_queries(struct coap_queries *queries);

/**
 * @brief Get the index of the first matching char in a string
 *
 * @param string The string to search in
 * @param search Character to search for
 * @return Index if found, -1 otherwise
 */
ssize_t str_index_of(const char *string, char search);

#ifdef __cplusplus
}
#endif

#endif //RIOTSENSORS_RS_COAP_UTILS_H
