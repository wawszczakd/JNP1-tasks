#ifndef HASH
#define HASH

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#endif

#ifdef __cplusplus
namespace jnp1 {
	extern "C" {
#endif
		
		typedef uint64_t (*hash_function_t) (uint64_t const *, size_t);
		
		unsigned long hash_create(hash_function_t);
		
		void hash_delete(unsigned long);
		
		size_t hash_size(unsigned long);
		
		bool hash_insert(unsigned long, uint64_t const *, size_t);
		
		bool hash_remove(unsigned long, uint64_t const *, size_t);
		
		void hash_clear(unsigned long);
		
		bool hash_test(unsigned long, uint64_t const *, size_t);
		
#ifdef __cplusplus
	}
}
#endif

#endif /* HASH */
