#ifndef UTIL_H
#define UTIL_H

#include "cms.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
  

/* Existing declarations */
uint32_t ip_to_int(const uint8_t *ip_addr);
void cms_free(CountMinSketch *cms);

/* Cleanup helper (no MPI_Finalize) */
void safe_cleanup(uint8_t *buffer, CountMinSketch *cms, MPI_File *fh);

#endif
