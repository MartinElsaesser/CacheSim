#pragma once
#include <string>
#include "Cache.h"

enum WriteHitPolicy { writeThrough, writeBack };
enum WriteMissPolicy { allocate, noAllocate };
typedef struct deconstructedAddress {
	int offset;
	int index;
	int tag;
} deconstructedAddress;

class Controller {
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
	Controller(unsigned int cellCount, unsigned int blockSize, unsigned int associativity, EvictionPolicy evictionPolicy, WriteHitPolicy writeHitPolicy, WriteMissPolicy writeMissPolicy);
	~Controller();
	void read(unsigned int address);

	void write(unsigned int address);
	std::string printResults();

private:
	deconstructedAddress deconstructAddress(unsigned int address);

};