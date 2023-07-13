#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cassert>
#include <iostream>
#include "hash.h"

namespace jnp1 {
	using vi = std::vector<uint64_t>;
	using std::unordered_map;
	using std::unordered_set;
	using std::cerr;
	
	#ifdef NDEBUG
		const bool debug = false;
	#else
		const bool debug = true;
	#endif
	
	class hasher {
	private:
		hash_function_t hash_function;
	public:
		hasher(hash_function_t _hash_function) {
			hash_function = _hash_function;
		}
		
		uint64_t operator()(const vi& vec) const {
			assert(!vec.empty());
			return hash_function(&vec[0], vec.size());
		}
	};
	
	unordered_map<unsigned long, unordered_set<vi, hasher>> &hash_tables() {
		static unordered_map<unsigned long, unordered_set<vi, hasher>> ans;
		return ans;
	}
	
	void print_start() {
		if (debug) {
			static std::ios_base::Init init;
		}
	}
	
	unsigned long hash_create(hash_function_t hash_function) {
		print_start();
		
		unordered_set<vi, hasher> uset(16, hasher(hash_function));
		static unsigned long last_id = 0;
		hash_tables().emplace(last_id, uset);
		
		if (debug) {
			cerr << "hash_create(" << &hash_function << ")\n";
			cerr << "hash_create: hash table #" << last_id << " created\n";
		}
		
		return last_id++;
	}
	
	void hash_delete(unsigned long id) {
		print_start();
		
		if (debug) {
			cerr << "hash_delete(" << id << ")\n";
			cerr << "hash_delete: hash table #" << id;
		}
		if (hash_tables().find(id) != hash_tables().end()) {
			hash_tables().erase(id);
			if (debug) {
				cerr << " deleted\n";
			}
		}
		else if (debug) {
			cerr << " does not exist\n";
		}
	}
	
	size_t hash_size(unsigned long id) {
		print_start();
		
		if (debug) {
			cerr << "hash_size(" << id << ")\n";
			cerr << "hash_size: hash table #" << id;
		}
		if (hash_tables().find(id) != hash_tables().end()) {
			size_t size = hash_tables().find(id)->second.size();
			if (debug) {
				cerr << " contains " << size << " element(s)\n";
			}
			return size;
		}
		else {
			if (debug) {
				cerr << " does not exist\n";
			}
			return 0;
		}
	}
	
	bool hash_insert(unsigned long id, uint64_t const *seq, size_t size) {
		print_start();
		
		if (debug) {
			cerr << "hash_insert(" << id << ", ";
			if (seq != nullptr) {
				cerr << "\"";
				if (size > 0) {
					for (size_t i = 0; i < size - 1; i++) {
						cerr << seq[i] << " ";
					}
					cerr << seq[size - 1];
				}
				cerr << "\"";
			}
			else {
				cerr << "NULL";
			}
			cerr << ", " << size << ")\n";
		}
		
		bool error = false;
		if (seq == nullptr) {
			error = true;
			if (debug) {
				cerr << "hash_insert: invalid pointer (NULL)\n";
			}
		}
		if (size == 0) {
			error = true;
			if (debug) {
				cerr << "hash_insert: invalid size (0)\n";
			}
		}
		
		if (error) {
			return false;
		}
		
		if (debug) {
			cerr << "hash_insert: hash table #" << id;
		}
		if (hash_tables().find(id) == hash_tables().end()) {
			if (debug) {
				cerr << " does not exist\n";
			}
			return false;
		}
		
		vi vec(seq, seq + size);
		if (debug) {
			cerr << ", sequence \"";
			for (size_t i = 0; i < size - 1; i++) {
				cerr << vec[i] << " ";
			}
			cerr << vec[size - 1] << "\" ";
		}
		
		unordered_set<vi, hasher>& uset = hash_tables().find(id)->second;
		if (uset.find(vec) != uset.end()) {
			if (debug) {
				cerr << "was present\n";
			}
			return false;
		}
		uset.insert(vec);
		if (debug) {
			cerr << "inserted\n";
		}
		
		return true;
	}
	
	bool hash_remove(unsigned long id, uint64_t const *seq, size_t size) {
		print_start();
		
		if (debug) {
			cerr << "hash_remove(" << id << ", ";
			if (seq != nullptr) {
				cerr << "\"";
				if (size > 0) {
					for (size_t i = 0; i < size - 1; i++) {
						cerr << seq[i] << " ";
					}
					cerr << seq[size - 1];
				}
				cerr << "\"";
			}
			else {
				cerr << "NULL";
			}
			cerr << ", " << size << ")\n";
		}
		
		bool error = false;
		if (seq == nullptr) {
			error = true;
			if (debug) {
				cerr << "hash_remove: invalid pointer (NULL)\n";
			}
		}
		if (size == 0) {
			error = true;
			if (debug) {
				cerr << "hash_remove: invalid size (0)\n";
			}
		}
		
		if (error) {
			return false;
		}
		
		if (debug) {
			cerr << "hash_remove: hash table #" << id;
		}
		if (hash_tables().find(id) == hash_tables().end()) {
			if (debug) {
				cerr << " does not exist\n";
			}
			return false;
		}
		
		vi vec(seq, seq + size);
		if (debug) {
			cerr << ", sequence \"";
			for (size_t i = 0; i < size - 1; i++) {
				cerr << vec[i] << " ";
			}
			cerr << vec[size - 1] << "\" ";
		}
		
		unordered_set<vi, hasher>& uset = hash_tables().find(id)->second;
		if (uset.find(vec) == uset.end()) {
			if (debug) {
				cerr << "was not present\n";
			}
			return false;
		}
		uset.erase(vec);
		if (debug) {
			cerr << "removed\n";
		}
		
		return true;
	}
	
	void hash_clear(unsigned long id) {
		print_start();
		
		if (debug) {
			cerr << "hash_clear(" << id << ")\n";
			cerr << "hash_clear: hash table #" << id;
		}
		if (hash_tables().find(id) != hash_tables().end()) {
			unordered_set<vi, hasher>& uset = hash_tables().find(id)->second;
			if (debug) {
				cerr << (uset.empty() ? " was empty\n" : " cleared\n");
			}
			uset.clear();
		}
		else if (debug) {
			cerr << " does not exist\n";
		}
	}
	
	bool hash_test(unsigned long id, uint64_t const *seq, size_t size) {
		print_start();
		
		if (debug) {
			cerr << "hash_test(" << id << ", ";
			if (seq != nullptr) {
				cerr << "\"";
				if (size > 0) {
					for (size_t i = 0; i < size - 1; i++) {
						cerr << seq[i] << " ";
					}
					cerr << seq[size - 1];
				}
				cerr << "\"";
			}
			else {
				cerr << "NULL";
			}
			cerr << ", " << size << ")\n";
		}
		
		bool error = false;
		if (seq == nullptr) {
			error = true;
			if (debug) {
				cerr << "hash_test: invalid pointer (NULL)\n";
			}
		}
		if (size == 0) {
			error = true;
			if (debug) {
				cerr << "hash_test: invalid size (0)\n";
			}
		}
		
		if (error) {
			return false;
		}
		
		if (debug) {
			cerr << "hash_test: hash table #" << id;
		}
		if (hash_tables().find(id) == hash_tables().end()) {
			if (debug) {
				cerr << " does not exist\n";
			}
			return false;
		}
		
		vi vec(seq, seq + size);
		if (debug) {
			cerr << ", sequence \"";
			for (size_t i = 0; i < size - 1; i++) {
				cerr << vec[i] << " ";
			}
			cerr << vec[size - 1] << "\" ";
		}
		
		unordered_set<vi, hasher>& uset = hash_tables().find(id)->second;
		if (uset.find(vec) == uset.end()) {
			if (debug) {
				cerr << "is not present\n";
			}
			return false;
		}
		else {
			if (debug) {
				cerr << "is present\n";
			}
			return true;
		}
	}
}
