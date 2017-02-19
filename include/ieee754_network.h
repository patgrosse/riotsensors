/**
 * @file ieee754_network.h
 * @brief Functions for converting floating point numbers to IEEE754 standard regardless of the host system
 * @author Copyright 2015 Brian "Beej Jorgensen" Hall
 *
 * These functions are taken from <a href="https://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#serialization">Beej's Guide to Network Programming</a>
 *
 * The source file can be found here: <a href="https://beej.us/guide/bgnet/examples/ieee754.c">ieee754.c</a>
 *
 * Cite from the license section:
 *
 * The C source code presented in this document is hereby granted to the public domain, and is completely free of any license restriction.
 */

#ifndef RIOTSENSORS_IEEE754_NETWORK_H
#define RIOTSENSORS_IEEE754_NETWORK_H

/** @brief Convert a single precision floating point to IEEE754 */
#define pack754_32(f) (pack754((f), 32, 8))
/** @brief Convert a double precision floating point to IEEE754 */
#define pack754_64(f) (pack754((f), 64, 11))
/** @brief Convert to a single precision floating point from IEEE754 */
#define unpack754_32(i) (unpack754((i), 32, 8))
/** @brief Convert to a double precision floating point from IEEE754 */
#define unpack754_64(i) (unpack754((i), 64, 11))

/**
 * Convert a given floating point number to IEEE754 standard
 * @param f The floating point to convert
 * @param bits Size in bits of the floating point
 * @param expbits Size of the exponent in bits
 * @return The converted floating point in IEEE754
 */
uint64_t pack754(long double f, unsigned bits, unsigned expbits);

/**
 * Convert a IEEE754 standard to a floating point number of the host system
 * @param i The IEEE754 number to convert
 * @param bits Size in bits of the floating point
 * @param expbits Size of the exponent in bits
 * @return The host system representation of the given IEEE754 number
 */
long double unpack754(uint64_t i, unsigned bits, unsigned expbits);

#endif //RIOTSENSORS_IEEE754_NETWORK_H
