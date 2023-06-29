#pragma once
#include <string>
#include <format>
#include <stdexcept>
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
	Cache(unsigned int sets_count, unsigned int associativity, EvictionPolicy evictionPolicy) { 
		// save parameters to cache variables
		this->sets_count = sets_count;
		this->associativity = associativity;
		this->evictionPolicy = evictionPolicy;

		// if sets_count = 4
		// sets_nextWriteIdx = [0,0,0,0] (for all 4 sets, the next free spot is the 0th idx)
		this->sets_nextWriteIdx = new unsigned int[sets_count];
		for (unsigned int setIdx = 0; setIdx < sets_count; setIdx++) {
			this->sets_nextWriteIdx[setIdx] = 0;
		}

		// if sets_count = 4
		// sets_nextWriteIdx = [false, false, false, false] (in all 4 sets, there is space left)
		this->sets_areFull = new bool[sets_count];
		for (unsigned int setIdx = 0; setIdx < sets_count; setIdx++) {
			this->sets_areFull[setIdx] = false;
		}

		// if sets_count = 4 && associativity = 2
		// sets_array = [[Cell, Cell], [Cell, Cell], [Cell, Cell], [Cell, Cell]] (2D Array of Cells, mapping cache)
		// a inner array of form [Cell, Cell] is referred to as a set
		this->sets_array = new Cell* *[sets_count];
		for(unsigned int setIdx = 0; setIdx<sets_count; setIdx++){
			this->sets_array[setIdx] = new Cell* [associativity];
			for(unsigned int cell = 0; cell<associativity; cell++){
				this->sets_array[setIdx][cell] = new Cell();
			}
		}
	}
	std::string to_string() const {
		std::string s;

		// go through sets
		for(unsigned int setIdx = 0; setIdx<this->sets_count; setIdx++){
			s += "[\n";
			s += std::format("  full: {}, nextIndexToWriteTo: {} \n", this->sets_areFull[setIdx], this->sets_nextWriteIdx[setIdx]);
			// go through cache cells
			for(unsigned int cell = 0; cell<associativity; cell++){
				Cell cellElement = *(this->sets_array[setIdx][cell]);
				s += std::format("  {{ tag: {}, valid: {}, dirty: {} }}\n", cellElement.tag, cellElement.valid, cellElement.dirty);
			}
			s += "]\n";
		}


		return s;
	}
	// return true if exists
	// return false if does not exist
	bool get(unsigned int tag, unsigned int index, unsigned int offset) {
		Cell** set = this->sets_array[index];

		for (unsigned int cellIdx = 0; cellIdx < this->associativity; cellIdx++) {
			if (set[cellIdx]->tag == tag && set[cellIdx]->valid) {
				this->LRU_moveCellToFront(index, cellIdx);
				return true;
			}
		}

		return false;
	}

	// return true if exists
	// return false if does not exist
	bool set(unsigned int tag, unsigned int index, unsigned int offset, bool dirty, bool allocateOnWriteMiss) {
		unsigned int setsIdx = index;
		Cell** set = this->sets_array[setsIdx];
		bool inCache = false;
		int LRUindex = 0;

		// already exists?
		for (unsigned int cellIdx = 0; cellIdx < associativity; cellIdx++) {
			Cell* cell = set[cellIdx];
			if (cell->tag == tag && cell->valid) {
				cell->dirty = dirty;
				this->LRU_moveCellToFront(setsIdx, cellIdx);
				return true;
			}
		}

		if (!allocateOnWriteMiss) return false;

		// does not exist + cache full -> throw
		if (this->sets_areFull[setsIdx]) { // cache full
			throw SetFullException();
		}


		// does not exist + cache not full -> create
		int cellIdx = this->sets_nextWriteIdx[setsIdx];
		Cell* cell = set[cellIdx];
		cell->dirty = dirty;
		cell->valid = true;
		cell->tag = tag;
		this->LRU_moveCellToFront(setsIdx, cellIdx);

		// keep track if set is full
		this->sets_nextWriteIdx[setsIdx]++;
		if (this->sets_nextWriteIdx[setsIdx] >= this->associativity) this->sets_areFull[setsIdx] = true;
		this->sets_nextWriteIdx[setsIdx] %= this->associativity;

		return false;

	}
	void evict(unsigned int tag, unsigned int index, unsigned int offset, bool dirty) {
		unsigned int setsIdx = index;
		Cell** set = this->sets_array[setsIdx];
		unsigned int cellIdx;
		// get cell Idx
		switch (this->evictionPolicy)
		{
		case random:
			cellIdx = rand() % this->associativity;
			break;
		case fifo:
			cellIdx = this->sets_nextWriteIdx[setsIdx];
			this->sets_nextWriteIdx[setsIdx] = (this->sets_nextWriteIdx[setsIdx] + 1) % this->associativity;
			break;
		case LRU:
			// move least recently used to front, to be replaced
			this->LRU_moveCellToFront(setsIdx, this->associativity - 1);       // TODO: check if something breaks
			// replace at front, because most recently used element is at front
			cellIdx = 0;
			break;
		default:
			throw std::invalid_argument("No valid policy present");
		}
		// replace data within cell
		Cell* cell = set[cellIdx];
		cell->dirty = dirty;
		cell->valid = true;
		cell->tag = tag;
	}

	~Cache() {
		for (unsigned int setIdx = 0; setIdx < this->sets_count; setIdx++) {
			delete[] this->sets_array[setIdx];
		}
		delete[] this->sets_array;
		delete[] this->sets_nextWriteIdx;
		delete[] this->sets_areFull;
	}

private:
	void LRU_moveCellToFront(int setIdx, int cellIdxToMove) {
		// take cell from set indicated by setIdx
		// move cell to front of set
		// move all other elements to make space
		if (this->evictionPolicy != LRU || this->associativity <= 1) return;

		Cell** set = this->sets_array[setIdx];
		Cell temp = *(set[cellIdxToMove]);
		for (int i = cellIdxToMove; i >= 1; i--) {
			*set[i] = *set[i - 1];
		}
		*set[0] = temp;
	}
};

std::ostream& operator<< (std::ostream& stream, const Cache& cache) {
	stream << cache.to_string();
	return stream;
}

std::ostream& operator<< (std::ostream& stream, Cache* cache) {
	stream << cache->to_string();
	return stream;
}
