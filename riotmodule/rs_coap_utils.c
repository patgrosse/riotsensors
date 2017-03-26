/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include <rs_coap_utils.h>

#include <stdlib.h>
#include <memory.h>
#include <sched.h>

ssize_t index_of(const char *string, char search) {
    const char *moved_string = strchr(string, search);
    /* If not null, return the difference. */
    if (moved_string) {
        return (ssize_t) (moved_string - string);
    }
    /* Character not found. */
    return -1;
}

void free_coap_query(struct coap_queries *queries) {
    struct coap_query *current = queries->first;
    while (current != NULL) {
        struct coap_query *next = current->next;
        free(current->key);
        free(current->value);
        free(current);
        current = next;
    }
}

void split_query(size_t len, const char *strquery, struct coap_queries *result) {
    result->num = 0;
    char *working = (char *) malloc(len + 1);
    memcpy(working, strquery, len);
    working[len] = '\0';
    const char *delimiter = "&";
    char *ptr = strtok(working, delimiter);
    struct coap_query *last = NULL;
    while (ptr != NULL) {
        result->num++;
        size_t lentotal = strlen(ptr);
        ssize_t lenkey = index_of(ptr, '=');
        struct coap_query *current = (struct coap_query *) malloc(sizeof(struct coap_query));
        if (lenkey == -1) {
            current->key = (char *) malloc(lentotal + 1);
            memcpy(current->key, ptr, lentotal + 1);
            current->value = (char *) malloc(1);
            current->value[0] = '\0';
        } else {
            current->key = (char *) malloc((size_t) (lenkey + 1));
            memcpy(current->key, ptr, (size_t) lenkey);
            current->key[lenkey] = '\0';
            current->value = (char *) malloc(lentotal - lenkey);
            memcpy(current->value, ptr + lenkey + 1, (size_t) lentotal - lenkey + 1);
            current->value[lentotal - lenkey] = '\0';
        }
        current->next = NULL;
        if (last == NULL) {
            result->first = current;
        } else {
            last->next = current;
        }
        last = current;
        ptr = strtok(NULL, delimiter);
    }
}
