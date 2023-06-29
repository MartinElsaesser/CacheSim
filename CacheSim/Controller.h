#pragma once
#include <stdexcept>
#include "helper.h"
#include <format>
#include <iostream>
#include <string>
#include <string_view>
#include <cmath>
#include "Cache.h"

/***************************************************************
******************   DELETE (only to DEBUG) ********************
****************************************************************/
typedef struct Operation {
	std::string type;
	unsigned int address;
} Operation;
Operation lastOP = {"", 0};
/***************************************************************
********   DELETE block end, do not delete any further *********
****************************************************************/



// TODO: testing
// TODO: deiffernce write through, write back
enum WriteHitPolicy { writeThrough, writeBack };
enum WriteMissPolicy { allocate, noAllocate };

class Controller {
	//Memory memory;
	Cache* cache;
	int hits = 0;
	int misses = 0;
	int evictions = 0;
	const int addressWidth = 32;

	int tagBits = 0;
	int indexBits = 0;
	int offsetBits = 0;
	bool dirtyValueForWrite = true;
	bool allocateOnWriteMiss = true;

	int blockSize = 0;

public:
	Controller(unsigned int cellCount, unsigned int blockSize, unsigned int associativity, EvictionPolicy evictionPolicy, WriteHitPolicy writeHitPolicy, WriteMissPolicy writeMissPolicy) {
		// check data
		if (associativity > cellCount) {
			std::string err = std::format("associativity({}) bigger than cellCount({})", associativity, cellCount);
			throw std::logic_error(err);
		}
		double setCountD = (double)cellCount / (double)associativity;
		if (!isOfBase2(setCountD)) {
			std::string err = std::format("setCountD({}) is not of base 2", setCountD);
			throw std::logic_error(err);
		}
		if (!isOfBase2(blockSize)) {
			std::string err = std::format("blockSize({}) is not of base 2", blockSize);
			throw std::logic_error(err);
		}

		// set variables according to config
		if (writeHitPolicy == writeThrough) {
			this->dirtyValueForWrite = false;
		}
		else if (writeHitPolicy == writeBack) {
			this->dirtyValueForWrite = true;
		}

		if (writeMissPolicy == noAllocate) {
			this->allocateOnWriteMiss = false;
		}
		else if (writeMissPolicy == allocate) {
			this->allocateOnWriteMiss = true;
		}

		this->blockSize = blockSize;
		this->offsetBits = log2i(blockSize);
		this->indexBits = log2i(setCountD);
		this->tagBits = this->addressWidth - (this->offsetBits + this->indexBits);
		unsigned int setCount = setCountD;

		this->cache = new Cache(setCount, associativity, evictionPolicy);
	}
	~Controller() {
		delete this->cache;
	}
	void read(unsigned int address) {
		/***************************************************************
		******************   DELETE (only to DEBUG) ********************
		****************************************************************/
		lastOP.type = "read";
		lastOP.address = address;
		/***************************************************************
		********   DELETE block end, do not delete any further *********
		****************************************************************/


		deconstructedAddress a = this->deconstructAddress(address);

		// HIT
		bool wasAHit = this->cache->get(a.tag, a.index, a.offset);
		if (wasAHit) {
			this->hits++;
			return;
		}

		// MISS
		try
		{
			this->misses++;
			this->cache->set(a.tag, a.index, a.offset, false, true);
		}
		catch (SetFullException& e) {
			// EVICTION
			this->cache->evict(a.tag, a.index, a.offset, false);
			this->evictions++;
		}
	}

	void write(unsigned int address) {
		/***************************************************************
		******************   DELETE (only to DEBUG) ********************
		****************************************************************/
		lastOP.type = "write";
		lastOP.address = address;
		/***************************************************************
		********   DELETE block end, do not delete any further *********
		****************************************************************/



		deconstructedAddress a = this->deconstructAddress(address);

		try
		{
			// HIT
			bool wasAHit = this->cache->set(a.tag, a.index, a.offset, this->dirtyValueForWrite, this->allocateOnWriteMiss);
			if (wasAHit) {
				this->hits++;
				return;
			}

			// MISS
			this->misses++;
		}
		catch (SetFullException& e) {
			// EVICTION
			this->cache->evict(a.tag, a.index, a.offset, this->dirtyValueForWrite);
			this->evictions++;
			this->misses++;

		}
	}
	std::string printResults() {
		return std::format("Results:\n  misses: {}\n  hits: {}\n  evictions: {}", this->misses, this->hits, this->evictions);
	}


	/***************************************************************
	******************   DELETE (only to DEBUG) ********************
	****************************************************************/
	void debugPrint() {// TODO:remove
		std::cout << std::format("Cache (hits: {}, misses: {}, evictions: {}, {}: ({})):\n", this->hits, this->misses, this->evictions, lastOP.type, lastOP.address) << this->cache << "\n\n";
	}
	/***************************************************************
	********   DELETE block end, do not delete any further *********
	****************************************************************/

private:
	typedef struct deconstructedAddress {
		int offset;
		int index;
		int tag;
	} deconstructedAddress;
	deconstructedAddress deconstructAddress(unsigned int address) {
		if (address >= pow(2, this->addressWidth)) {
			std::string err = std::format("address({}) bigger than({})", address, pow(2, this->addressWidth));
			throw std::logic_error(err);
		}
		unsigned int indexMask = pow(2, this->indexBits) - 1;
		unsigned int offsetMask = pow(2, this->offsetBits) - 1;
		unsigned int tagMask = pow(2, this->tagBits) - 1;
		
		// deconstruct offset
		deconstructedAddress decAdd;
		decAdd.offset = address & offsetMask;
		address >>= this->offsetBits;

		// deconstruct index
		decAdd.index = address & indexMask;
		address >>= this->indexBits;

		// deconstruct tag
		decAdd.tag = address & tagMask;
		return decAdd;
	}

};