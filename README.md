# COMP3703 Software Security 2024 -- Assignment 2

# The assignment problems

There are two problems in this assignment:

* [Problem 1 (stack-based exploitation)](./stack/). You are given a binary (implementing a parser for Intel HEX format for binary files), containing one or more vulnerabilities. You are asked to exploit these vulnerabilities, using stack-based exploitation methods, to achieve a number of objectives, culminating in an arbitrary code execution. There are **4 (four) subproblems** within this problem, with increasing difficulty. The subproblems are designed in a way that helps you in building exploit primitives that would eventually lead to arbitrary code execution.  

* [Problem 2 (heap-based exploitation)](./heap/). There are two binaries in this problem, with similar functionalities but different exploit mitigations in place. You are asked to find heap-related vulnerabilities (use-after-free and double-free) and exploit them to achieve a number of objectives. This problem is divided into **7 (seven) subproblems** with increasing difficulty. Just as in Problem 1, these subproblems are designed to help you building the necessary primitives for the final goal of achieving arbitrary code execution.  

To see the details for each problem, read the instructions in the respective folders ([./stack](./stack) for Problem 1 and [./heap](./heap/) for Problem 2).

# Submission requirements and deadline

**Deadline: Wednesday, 22 May 2024, 5pm Canberra time**

**Late submission penalty: 100% of total mark** 

There are two components to this assignment:

* __Artefact:__ This consists of python scripts implementing the exploitation steps.  

* __Report:__ This is a PDF document explaining how you solve the assignment problems. The report file must be named following the convention: `<your_uni_id>_report.pdf`, e.g., `u1234567_report.pdf`. Limit your report length to around 3500 words. 

The artefact component accounts for `50%` of the total marks, and the report component accounts for the remaining `50%`. The break down of total marks for each assignment problem can be found below. 

The submission instruction and the assessment guidelines will be provided in a separate document to be published on Wattle. 


# Execution environment

This assignment assumes all the associated binaries are run inside the lab VM. You need to install two additonal software packages to run and test these binaries: `socat` and `libreadline-dev`. Install them in the lab VM using the following commands:

```bash
$ sudo apt install socat libreadline-dev
```

# Testing your solution scripts

Each subproblem requires you to write a python script to launch the exploit. For the purpose of assessment, you must assume that the binaries will be run in a remote server, so in particular you have no direct visibility into the program states (so, e.g., you cannot run gdb and inspect the buffer addresses or libc addresses while executing your attacks). To assist you in testing your solution, to make sure that they comply with this requirement, each problem comes with a script to run the binary as a local server application that interacts only through the (localhost) network socket. Your submitted solution scripts must interact with the binaries only through the network sockets. This can be done through `pwntools`'s `remote()` command. You are, however, free to use the `process()` command to interact with the binaries while you are developing your solutions, but in the final submission, the `process()` command must be replaced by the `remote()` command. 



