# MPI Integral calculator for de Jong function

## Overview

This is an application to numerically calculate integrals of de Jong function. However, it can handle all other functions of two arguments as well. To achieve higher performance OpenMPI is used here, which is however sometimes not a very good idea (e.g. if we have hosts in Lviv, Tokyo and California and our function is something like y = x1 + x2 and we don't need to get very precize result it might be a better idea to run such calculations locally, as data transfer will take more time than parallelism can save in such case.)

## Compilation

To build and run this app on your laptop/server/cluster/whatever you'll need a OpenMPI installed in your system. More details on getting these things done you can find on the official web-site -- https://www.open-mpi.org/.

To compile the main.cpp you should simply call mpic++ main.cpp -o main -std=c++14 and then you can run ./main. Not a rocket science.

## Usage

This application takes 5 arguments from command line (excluding it's own name):
- lower x bound
- upper x bound
- lower y bound
- upper y bound
- step of the integrating (distance between two points calculated)

After building the application, you can run it with mpirun command. That will run it on your local node by default (number of the processes will be equal to all available CPU's of your system). Or you can specify hosts in the text file in the following style:

hostname1

hostname2

hostname2


And run the application with mpirun -hostfile hosts.txt main 0 1 0 1 0.001

## Research

According to the lab task we conducted a small research on the performance of the OpenMPI solution. To do this we used two strategies:
- deployed a virtual machines cluster on a single laptom (Intel Core i7-6700HQ one) 
- deployed OpenMPI on the real network between two our laptops

These two approaches allows to test both synthetic performance of the OpenMPI cluster in case of VM network (as it gives almost zero ping time, no packet loses, good infrastructure and so on) and real efficience of the OpenMPI solution in the ad-hoc cluster built on top of usual smartphone Wi-Fi hotspot with all it's benefits (quite an awful wi-fi power, noisy wireless channel).

So, in case of VM network performance growth was almost linear: increasing number of active processes from 2 to 4 (for [0 < x < 2] and [0 < y < 3], step is 0.001) reduces average time from 14.42 sec to 8.13 seconds. Further imporvement didn't give good result, because each of the virtual machines had only one processor's core.

However, in case of real network, result was't that optimistic. For example, after moving from a single local process to two processes (one per host) performance droped from 0.25 sec (for 0.01 step) to 0.5 seconds. After increasing number of processes (two per host) it gave quite positive result and total time was about 0.23-27 sec. Also, if step was set to 0.001 calculation time becomes large enough so that distributed system could give some benefits -- performance increased for about 60% after we've twiced number of the processes.

So, the conclusion we state is following: using OpenMPI for tasks with very small calculation required is quite useless (unless it's run on a single server or cluster if connected with some ultra-low latency interface, or OpenMPI was used as a part of legacy code of with some view into the future). However, increasing amount of calculations makes such distributed system more tolerant to weak network (and again, that is very task specific. e.g. our integrating task didn't require much communications and has almost no data dependencies, however if lots of communication is required network again becomes the weakes part of the system).

## A word on methodology

Both computers were run under Ubuntu 16.04 LTS and compiled with -O3 optimization level with g++ 5.4.0 and the latest version of the OpenMPI available in stable repository.


## License

Copyright 2017 Sergey Dubovyk

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Contribution

Feel free to contribute to this project as it can be useful for some students learning OpenMPI stack or just for somebody, who has some free time.
