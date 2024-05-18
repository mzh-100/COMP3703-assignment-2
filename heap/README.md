# Problem 2: Heap-based exploitation 

For this problem, you are given two binaries, `heap1` and `heap2`, and their corresponding source code, in the directory [./heap](./heap/). 
The two binaries are very similar in functionalities, but with some subtle differences that would require different exploitation methods. 
The binary `heap1` was compiled with some security features disabled, e.g., it is a non-PIE binary and GOT entries are writeable at runtime (partial RELRO). The binary `heap2` has all the binary level security features enabled. 

There are 7 (seven) subproblems in this problem, `Heap_Level_1` to `Heap_Level_7`. The subproblems `Heap_Level_1` to `Heap_Level_4` are all about exploiting the binary `heap1`, whereas `Heap_Level_5` to `Heap_Level_7` are about exploiting the binary `heap2.`

Just as is the case with Problem 1, you must implement your attack assuming you will interact with a remote application server running these binaries. The application server for `heap1` can be launched by running `run_heap1.sh` (exposing the tcp port 1338), and the application server for `heap2` can be launched by running `run_heap2.sh` (exposing the port 1339). 


## 2.1: Heap_Level_1 -- use-after-free and tcache poisoning (Max. mark: 5/100)

For this subproblem, you are tasked with identifying a use-after-free bug in the binary `heap1`, and create a write-what-where primitive out of it using tcache poisoning. Then exploit it to write a certain number to the global variable `check_win1`. If a correct value is written, upon exiting the program, a flag will be printed. 

**Required artefact submission: [heap_lvl_1.py](./heap_lvl_1.py).**  

Implement your exploitation in the python script [heap_lvl_1.py](./heap_lvl_1.py). A successful exploitation will cause the binary to print the flag `flag{Heap_Level_1}`. The flag must be the last line printed in your solution script. 


## 2.2: Heap_Level_2 -- GOT hijacking (Max. mark: 7/100)

This is still about exploiting the UAF bug found in `Heap_Level_1`, but this time, you will use it to hijack a GOT entry to execute the `win2()` function. Since `heap1` is a non-PIE binary, you can determine the GOT address statically. There are more than one GOT entries to choose from -- think carefully about your selection, and make sure that you do not accidentally modify other GOT entries (causing the program to crash). 

__Hint:__ When you poison a tcache entry and request the target chunk using malloc, pay attention to how malloc returns that entry. The actual function that returns that malloc chunk from tcache is implemented in the `tcache_get()` function from glibc 2.31:  
```C
/* Caller must ensure that we know tc_idx is valid and there's
   available chunks to remove.  */
static __always_inline void *
tcache_get (size_t tc_idx)
{
  tcache_entry *e = tcache->entries[tc_idx];
  tcache->entries[tc_idx] = e->next;
  --(tcache->counts[tc_idx]);
  e->key = NULL;
  return (void *) e;
}
```
Note that the second field in the `tcache_entry`, i.e., the `key` field, is set to NULL. An implication of this is that if the target `tcache_entry` corresponds to some address you want to access, the bytes at offset 8-15 from the target will be zeroed when it is returned by malloc. So if you want to modify a GOT entry using tcache poisoning, you may want to ensure that adjacent entries are not zeroed accidentally, or if they are zeroed, it would not affect the execution of your exploit code.

**Required artefact submission: [heap_lvl_2.py](./heap_lvl_2.py).**  

Implement your exploitation in the python script [heap_lvl_2.py](./heap/heap_lvl_2.py). A successful exploitation will cause the binary to print the flag `flag{Heap_Level_2}`. The flag must be the last line printed in your solution script. 

## 2.3: Heap_Level_3 -- information leak via GOT (Max. mark: 8/100)

For this subproblem, you will exploit the same UAF bug from `Heap_Level_3` to disclose the libc base address. This is to be done through reading a GOT entry (that has been patched by the runtime linker), so you can then calculate the libc base address. When you have obtained the libc base, check it using the `libc` menu from the program -- a correct libc value will result in the flag being printed. 

**Required artefact submission: [heap_lvl_3.py](./heap_lvl_3.py).**  

Implement your exploitation in the python script [heap_lvl_3.py](./heap/heap_lvl_3.py). A successful exploitation will cause the binary to print the flag `flag{Heap_Level_3}`. The flag must be the last line printed in your solution script. 

### 2.4: Heap_Level_4 -- It's shell time! (Max. mark: 10/100)

Now that you have managed to create a write-what-where primitive and a libc base address leak, you are asked to hijack `__free_hook` to launch a shell (`/bin/sh`). 

**Required artefact submission: [heap_lvl_4.py](./heap_lvl_4.py).**  

Implement your exploitation in the python script [heap_lvl_4.py](./heap/heap_lvl_4.py). There is no flag to print in this case. A successful exploitation will cause the binary to spawn a shell. Try to execute a shell command to see if your exploit is working. If you test it on the application server, running the `whoami` command in the shell should show the username `nobody`. 


## 2.5: Heap_Level_5 -- a double-free in fastbin (Max. mark: 7/100)

For this subproblem, we will be looking at the binary `heap2`. This binary is fully fortified -- GOT is now not writeable after it's loaded, and the binary is now position-independent. There are also some subtle changes to the program that result in the use-after-free bug not being directly exploitable. That is, you can't directly use it to overwrite a freed pointer, though reading a freed pointer is still possible. 
However, there is a double-free bug! Your task is to find that double-free bug, and turn it into an arbitrary write primitive. Since we are using glibc 2.31 for this assignment, there is a double-free check in the tcache, so you will need to try to trigger a double-free in fastbin -- undetected by glibc!
The goal for this subtask is to create a loop in the fastbin. To demonstrate that you had in fact created a loop in fastbin (through a double-free), empty the tcache and the fastbin so that the loop ends up in the cells in the program. Then use the menu `chkdup` to detect the loop. The `chkdup` function simply checks whether there are indices `i` and `j` such that `i != j` but `cells[i] == cells[j]`. If this is the case, a flag will be printed. 

**Required artefact submission: [heap_lvl_5.py](./heap_lvl_5.py).**  

Implement your exploitation in the python script [heap_lvl_5.py](./heap/heap_lvl_5.py). A successful exploitation will cause the binary to print the flag `flag{Heap_Level_5}`. The flag must be the last line printed in your solution script. 

## 2.6: Heap_Level_6 -- bypassing the size restriction (Max. mark: 8/100)

Now that you have managed to trigger a double-free undetected, and created a loop in tcache, you should be able to manipulate the heap into a state where an allocated chunk is simultaneously also a free chunk in the tcache. You will use this to alter the cell size field, such that it goes beyond the maximum value allowed. To do this, you need to create a state where the data field of cells[i] points to cells[j], for some `i` and `j`. This would then allow you to manipulate the fields of cells[j] by overwriting cells[i]->data. 
For this task, change cells[j]->size to a particular value (see the source code `heap2.c` to determine which value), and trigger a call to write to cells[j]. This would then cause the flag to be printed. 

**Required artefact submission: [heap_lvl_6.py](./heap_lvl_6.py).**  

Implement your exploitation in the python script [heap_lvl_6.py](./heap/heap_lvl_6.py). A successful exploitation will cause the binary to print the flag `flag{Heap_Level_6}`. The flag must be the last line printed in your solution script. 


## 2.7: Heap_Level_7 -- a harder shell (Max. mark: 10/100)

In this final task, you will exploit the binary `heap2` to launch a shell. The double-free bug you exploited in `Heap_Level_5` and `Heap_Level_6` should allow you to re-create tcache poisoning, and through it, arbitrary write. What is missing for a complete exploitation is the information leak to give us the libc base address. How do we go about obtaining the libc base address? One common technique is to read into the `fd` field of the unsorted bin. But for that to happen, we must first create an entry in the unsorted bin. This is where the `Heap_Level_6` solution can be useful. Try to create a cell that has a size that will not fit in the tcache, so that when it is freed, it will go to the unsorted bin. Once you managed to read-off the `fd` field of the chunk in the unsorted bin, you should be ready to complete this final exploit to launch a shell. 

**Required artefact submission: [heap_lvl_7.py](./heap_lvl_7.py).**  

Implement your exploitation in the python script [heap_lvl_7.py](./heap/heap_lvl_7.py). There is no flag to print in this case. A successful exploitation will cause the binary to spawn a shell. Try to execute a shell command to see if your exploit is working. If you test it on the application server, running the `whoami` command in the shell should show the username `nobody`. 
