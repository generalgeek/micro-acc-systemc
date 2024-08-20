# micro-acc-systemc
> simulating connection of micro processor and accelerator on a bus context with SystemC library 

![micro-acc-systemc image](./doc/micro-acc-systemc.png)

<hr />


## Getting Started
This repository exposed here, is actually a university project of the course `hardware software co-design`

And implemented with `SystemC` library.

- ### Build and Run
  - You can execute **./build.sh** to compile this project, and then execute **./run.sh** to run the program.

  - Perhaps you need to modify the content of **Cmakelists.txt** appropriately according to your environment.

  - The configuration files in the .vscode directory can help you to debug this project in VSCode.

- ### Prerequisites
  - #### Installing SystemC
  
    SystemC is a set of C++ classes and macros which provide an event-driven simulation interface.
    
    Download SystemC from [here](https://github.com/accellera-official/systemc/tags), then execute the following command on Linux to install.
    ```
    cd systemc-2.3.3
    mkdir build
    cd systemc-2.3.3/build
    cmake .. -DCMAKE_CXX_STANDARD=11 -DCMAKE_INSTALL_PREFIX=/usr/local
    make -j
    sudo make install
    ```
    
- ### Subject
  - #### Problem
    
    You can figure it out what the project exactly is about here: [project description](./doc/hw-sw_project-description.pdf)
    
  - #### Solution
    
    You can see the design, architecture and the datapath of this project here: [project-design](./doc/HWSW-PROJECT-ISA.pdf) 
    
<hr />
<hr />

## Proof Of Concepts
      
  The datapath of the hardware consists of:
    
  - PC (program counter)
  - IR (instruction memory)
  - IF (instruction fetch phase middle register - for pipelining)
  - RegFile (register file)
  - ID (instruction decode phase middle register - for piplining)
  - ALU (arithmetic/logic unit)
  - EXE (instruction execution phase middle register - for piplining)
  - MEM (share memory between micro and accelerator)
  - WB (instruction write back phase middle register - for piplinig)
  - Mux3 (multiplexer with 3 bits data and 1 bit selector)
  - Mux8 (multiplexer with 8 bits data and 1 bit selector)
    
### <i> Some More Documentation </i>


THIS PROJECT IS ABOUT ARCHITECTING A HARDWARE-SOFTWARE ARCHITECTURE USING SYSTEMC

The version of SystemC which is used in this project is 2.3.1

The hierarchy display of the modules is demonstrated here:


----System

-----------| 

-----------Micro

------------------|

------------------PC         //program counter

------------------IR         //Instruction Memory

------------------IF         //Instruction Fetch Register (for pipeline)

------------------RegFile    //Register File

------------------ID         //Instruction Decode Register (for pipeline)

------------------ALU        //Arithmetic/Logic Unit

------------------Mux3       //Multiplexer 2 inputs with 3 bits

------------------Mux8       //Multiplexer 2 inputs with 8 bits

------------------EXE        //Instruction Execution Register (for pipeline)

------------------WB         //Instruction WriteBack Register (for pipeline) 

-----------Bus

-----------Memory

-----------Acc

----/

<hr />

> NOTES

***Note that Micro is a micro processor with synch clk which its rate is a quarter of bus-clk***

***Note that the accelerator is a separate component helping the main micro in computing***

***Note that the accelerator works asynchronous so that we need events for using wait/notify***

***Note that the System works with `bus-clk` (clk_bus) which is `faster than` the `micro clk` (clk)***

<br />

***Note that there is two subfoler in this repository named `instructions` and `test`***

  - test folder : all testbenches for all components (some of these files are empty -> you can fill it yourself)
  - instructions folder: there are files you can see the binary micro-processor instructions you can put them into IR.cpp file.
  
***Note That the main testbench is named main.cpp, tests the System.cpp module***

***Note that some couple of the opselect values for ALU are the same, so we get help from the main opcode and recognize the aluop*** 

***Note that some needed signals like controlling bits and immediate value (also known as offset) are propagated through the middle registers  (IF, ID, EXE, WB)*** 

<br />

***Note that these modules work with `micro-clk`:***

  - Micro
  - PC
  - IF
  - ID
  - EXE
  - WB
  
***Note that these modules work with `bus-clk`:***

  - Bus
  - System
  
***Note that these modules works with `no clk` at all:***

  - IR
  - Controller
  - Mux3
  - Mux8
  - RegFile
  - ALU
  - MEM
  

***Note that these modules work with `SC_THREAD` simulation:***

  - Micro
  - IF
  - ID
  - EXE
  - WB

<hr />
<hr />

## Results

You can install the **WaveTrace** plugin in VSCode, then open the .vcd file generated after running the program to see the waveform effect.

![waveform](./doc/vcd_waveform.png)






