# Cache Simulator Project
## Build Folders
There is a Linux_Version and a Windows_Version folder for building the project.  
The project needs C++20 to run, for a linux build this requires gcc/g++ version 13.  
The CacheSim.exe is already a finished windows build file, ready to be executed.

## Running the project
Open a terminal an cd into where the compiled CacheSim  executable lies.  
For a quick start run
```cmd
CacheSim.exe -t <path-to-trace->
```
If there was a art.trace file within the folder traces, you would need to run
```cmd
CacheSim.exe -t ./traces/art.trace
```

Normally the CacheSim runs on default values, if you want to change them, use the following options  
cache options:  
	-c, --cellCount arg      Number of cache cells in cache     [uint]   (default: 1024)  
	-b, --blockSize arg      Number of bytes a cache cell holds [uint]   (default: 16)  
	-a, --associativity arg  Cache associativity                [uint]   (default: 1)  
	-e, --evict arg          Evicton Policy    [LRU|fifo|random]         (default: LRU)  
	-w, --hit arg            Write hit Policy  [writeBack|writeThrough]  (default: writeBack)  
	-m, --miss arg           Write miss Policy [allocate|noAllocate]     (default: allocate)  

simulator options:  
	-h, --help        Print help screen  
	-o, --output arg  Path to output file [string] (default: "")  
	-t, --trace arg   Path to trace file  [string]  

-t is the only needed argument

Example:
CacheSim.exe -t ./traces/art.trace -a 2 -c 16 --blockSize 2  
Would result in 16 total cache cells all with 2 byte blocks. There would be 8 sets.  
The cache would be 32 bytes in total
