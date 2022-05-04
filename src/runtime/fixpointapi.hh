#pragma once

#include "wasm-rt.h"

namespace fixpoint {
/* Initialize module instance*/
void* init_module_instance( size_t instance_size );

/* Initialize env instance*/
void* init_env_instance( size_t env_instance_size );

/* Free env instance*/
void free_env_instance( void* env_instance );

void attach_tree( __m256i ro_handle, wasm_rt_externref_table_t* target_memory );

/* Traps if handle is inaccessible, if handle does not refer to a Blob */
void attach_blob( __m256i ro_handle, wasm_rt_memory_t* target_memory );

__m256i detach_mem( wasm_rt_memory_t* target_memory );

/* Traps if the rw_handle does not refer to an MBlob, or the specified size is larger than the size of the MBlob. */
__m256i freeze_blob( __m256i rw_handle, size_t size );
}
