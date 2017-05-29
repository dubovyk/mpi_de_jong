# MPI Integral calculator for de Jong function

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

## License

Copyright 2017 Sergey Dubovyk

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Contribution

Feel free to contribute to this project as it can be useful for some students learning OpenMPI stack or just for somebody, who has some free time.
