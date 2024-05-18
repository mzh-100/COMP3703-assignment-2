# Problem 1: Stack-based exploitation 

For this problem, you are given a binary `ihex8i` and its source code `ihex8i.c` (with some parts omitted). The `ihex8i` implements a basic version of a converter for [Intel HEX file format](./intelhex.md). To simplify the task slightly, the CRC check is currently disabled, so you can ignore the computation of the CRC byte. 

This problem is divided into four subproblems, which we call `Stack_Level_1` to `Stack_Level_4`, in increasing difficulty. 

Each subproblem requires you to write a solution script that interacts with the binary through the localhost network. To test that your final solutions, first run the binary as an application server as follows: 

```
$ cd stack
$ ./run_ihex8i.sh
```

This will create an application server running `ihex8i`, listening on the tcp socket 1337.  To interact with the application directly, open another terminal and run 

```
$ nc localhost 1337
```

To interact with the application server through python/pwntools, use the `remote()` command, e.g., your python script will likely start with something like:

```python
from pwn import *

proc = remote('localhost', 1337)
```

Then you can use the usual process-related functions to interact with the application, such as `proc.sendline()` to send command or `proc.recvline()` to receive lines. 

### 1.1: Stack_Level_1 -- a simple return pointer hijacking  (Max. mark: 10/100)

This is a warm-up exercise to get you started smashing the stack! 

There is an easy-to-spot stack-based overflow in the `unlock()` function. The stack canary is disabled for that particular function, so there is no protection against linear buffer overflow. Find the vulnerability and exploit it to execute the `Win1()` function. 

**Required artefact submission: [stack_lvl_1.py](./stack/stack_lvl_1.py).**  

Implement your exploitation in the python script [stack_lvl_1.py](./stack/stack_lvl_1.py). A successful exploitation will cause the binary to print the flag `flag{Stack_Level_1}`. The flag must be the last line printed in your solution script. 

### 1.2: Stack_Level_2 -- overwriting stack variables (Max. mark: 10/100)

You may have noticed that the `write` functionality is disabled. You will need to enable this, somehow, for solving the last subproblem (`Stack_Level_4`). You will do this through exploiting the stack-based overflow in the `unlock` function, but the goal here is to by-pass the check on the (secret) key in the function. (Note that the key is randomly generated at every run, and a wrong guess will terminate the program, to ensure you don't brute-force the key to solve this problem). 

**Required artefact submission: [stack_lvl_2.py](./stack/stack_lvl_2.py).**  

Implement your exploitation in the python script [stack_lvl_2.py](./stack/stack_lvl_2.py). A successful exploitation will cause the binary to print the flag `flag{Stack_Level_2}`. The flag must be the last line printed in your solution script. 


### 1.3: Stack_Level_3 -- information leak on the stack (Max. mark: 10/100)

There is an out-of-bound read bug in the program that allows you to disclose some meta data in the stack frame of a certain function. Use that bug to disclose the canary value. When you have successfully obtained the canary value, use the `canary` function to check whether you have obtained the correct value. 

**Required artefact submission: [stack_lvl_3.py](./stack/stack_lvl_3.py).**  

Implement your exploitation in the python script [stack_lvl_3.py](./stack/stack_lvl_3.py). A successful exploitation will cause the binary to print the flag `flag{Stack_Level_3}`. The flag must be the last line printed in your solution script. 

### 1.4: Stack_Level_4 -- code execution via a ROP chain (Max. mark: 15/100)

There is an out-of-bound write bug in the program that allows you to overwrite fully a return pointer (but not much else). Note that this is not the same bug found in `unlock()`. Exploit the bug to hijack the control flow to execute the `Win2()` function. You will need to alter the value of the `magic` variable to successfully output the flag, so you will need to create a ROP chain to do this. Think about your attack strategy carefully -- what pieces of information do you need to successfully execute your ROP chain? Do you have enough space in the buffer to hold the ROP chain, and if not, how would you _pivot_ the stack? You may find the information disclosure bug in `Stack_Level_3` useful for this subproblem. Use the canary value to avoid tripping the stack guard, as you overflow the buffer. Other than the canary value, what other information can you obtain from that bug in `Stack_Level_3`, and how would that help with this subproblem?

Note that you don't need to worry about the libc base, as it is provided to you for free (at the start of the program). 

**Required artefact submission: [stack_lvl_4.py](./stack/stack_lvl_4.py).**  

Implement your exploitation in the python script [stack_lvl_4.py](./stack/stack_lvl_4.py). A successful exploitation will cause the binary to print the flag `flag{Stack_Level_4}`. The flag must be the last line printed in your solution script. 

