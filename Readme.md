# Notes
+ cxxopts is already included as a way to read console flags
	+ src: https://github.com/jarro2783/cxxopts/tree/master
	+ example (up to date): https://github.com/jarro2783/cxxopts/blob/master/src/example.cpp
	+ example (little outdated): https://medium.com/@mostsignificant/3-ways-to-parse-command-line-arguments-in-c-quick-do-it-yourself-or-comprehensive-36913284460f#6265

# Todos
## Controller.cpp
+ Delete debug functionality when project is finished (parts to delete are marked)
## main.cpp
1. Read parameters from console interface (spec following)
	+ Flags
		+ cell count  			(-c | --cellCount) 			[unsigned int]
		+ block count 			(-b | --blockCount) 		[unsigned int]
		+ associativity 		(-a | --associativity) 	[unsigned int]
		+ eviction policy 	(-e | --evict) 	[string] LRU|fifo|random
		+ write hit policy 	(-w | --hit) 		[string] writeBack|writeThrough
		+ write miss policy	(-m | --miss) 	[string] allocate|noAllocate
		+ trace file path   (-t | --trace)	[string]
		+ help screen 		(-h | --help)
	+ help screen
		+ help screen should list out all required parameters (if no default values are defined, all flags are needed)
		+ maybe give some examples of how it is to be used
		+ if defaults are present, tell the user about them
		+ if flags have a short and a long form, list both
		+ list expected data to a flag
	+ take parameters
		+ check if -h was included, go to help screen
		+ check if all arguments are present / or have default values for some?
		+ if error was throw, hint that there is a help screen (errors are thrown in controller, on creation and when read() and write() are called)
2. Initialize Controller based on parameters
3. Read trace file from provided trace file path 
	+ loop through lines and execute either controller.write(addres) or controller.read(address)
4. Output hits, misses and cache evictions
	+ hits, missed and evictions are currently private on Controller, so a extra output function is needed
```Javascript
// javascript pseudoCode

// wrap line below in try catch, and show thrown error, maybe goto help screen
{cellCount, blockCount, associativity, eviction, writeHit, writeMiss, path, help} = read_parameters()

if(help) return printHelpScreen();
trace = readFile(path);

try {
	controller = Controller(cellCount, blockCount, associativity, eviction, writeHit, writeMiss);
	for(line in trace) {
		if(line.type == read) {
			controller.read(line.address)
		}
		if(line.type == write) {
			controller.write(line.address)
		}
	}
	controller.outputResults()
} catch(error) {
	console.log(error)
	printHelpScreen();
}

```
## optional testing
+ see if it works as intended For reference: https://courses.cs.washington.edu/courses/cse351/cachesim/