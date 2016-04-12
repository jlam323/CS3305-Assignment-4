# CS3305-Assignment-4

The simulator.c program performs the action of a virtual memory system that uses a single level page table. It reads a file that contains memory address referenced by the instructions of a process. It then takes these addresses and loads them as pages in a table. The number of frames in the table is decided by the user upon execution. The simulator uses the algorithm of either Least Recently Used (LRU) or Least Frequently Used (LFU) to decide how to replace frames when the table is full.

Simulator is executed using the number of frames, followed by the trace file, and lastly the algorithm type.
Ex: ./simulator 4 trace LRU
