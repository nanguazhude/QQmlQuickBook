#pragma once

extern "C" {


__attribute__((visibility("default"))) void* je_malloc(size_t size);
__attribute__((visibility("default"))) void  je_free(void *);
__attribute__((visibility("default"))) void* je_aligned_alloc(size_t,size_t);



}


