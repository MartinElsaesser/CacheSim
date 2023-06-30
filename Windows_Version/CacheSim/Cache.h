#pragma once
#include <string>
#include <format>
#include <stdexcept>
#include <iostream>
#include "SetFullException.h"

typedef struct Cell {
	unsigned int tag = 0;
	bool dirty = false;
	bool valid = false;
} Cell;

enum EvictionPolicy { random, fifo, LRU };

class Cache {
public:
	// all elements needed to keep track of sets
	Cell** *		sets_array = 0;			// [[Cell*, Cell*], [Cell*, Cell*]]
	unsigned int*	sets_nextWriteIdx = 0;	// [0, 1]		   (index for fifo and to determine if set is full)
	bool*			sets_areFull = 0;		// [false, false]  (keeps track if set is full)
	unsigned int	sets_count = 0;
	// other cache variables
	unsigned int associativity = 1;
	EvictionPolicy evictionPolicy = random;	// regulates what is replaced, if set is full: 
										    // + random: a random element from the set
										    // + fifo: oldest element
											// + lru: least recently used element

	//default constructor
	Cache(){}
	//builds internal array to represent the cache with "sets" elements, "associativity" entries in second dimension and "blocksize"
	Cache(unsigned int sets_count, unsigned int associativity, EvictionPolicy evictionPolicy);
	std::string to_string() const;
	// return true if exists
	// return false if does not exist
	bool get(unsigned int tag, unsigned int index, unsigned int offset);

	// return true if exists
	// return false if does not exist
	bool set(unsigned int tag, unsigned int index, unsigned int offset, bool dirty, bool allocateOnWriteMiss);
	void evict(unsigned int tag, unsigned int index, unsigned int offset, bool dirty);

	~Cache();

private:
	void LRU_moveCellToFront(int setIdx, int cellIdxToMove);
};

std::ostream& operator<< (std::ostream& stream, const Cache& cache); 
std::ostream& operator<< (std::ostream& stream, Cache* cache);

