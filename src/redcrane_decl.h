/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides declarations for the engine's C interface, specifically
 * tailored for LuaJIT's FFI facilities.
 */

struct Redc_Engine;

void free(void*);

// We chose this API as opposed to a single function that returns a lua table
// with *all* of the config options because in general, I don't expect all
// options to be necessary all at once. Overall, doing it this way will
// probably be more efficient.

// Must be free'd
const char *redc_get_config_opt(struct Redc_Engine *rce, const char *opt);

void redc_start_connect(struct Redc_Engine *rce, const char *ip,
                        uint16_t port);
