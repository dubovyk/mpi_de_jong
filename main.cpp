/*
 * 
 * This is a simple application to calculate numerically integrals of the functions of two arguments.
 * 
 *  Copyright 2017 Sergey Dubovyk
 * 
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <mpi.h>
#include <iostream>
#include <cmath>

double dejong_func(double x1, double x2);
double integrate(double (*func)(double, double), double x_from, double x_to, double y_from, double y_to, double step);


/*
 * This procedure calculates the value of the de Jong's function in given (x1,x2) point and returns
 * it's double value.
 */
double dejong_func( double x1, double x2) {
    double res = 0.002;

    // main loop of the de Jong function (more details on function itself can be found here -- https://www.sfu.ca/~ssurjano/dejong5.html
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++){
		double divisor = 1 / std::abs(5*(i+2) + j + 3 + std::pow(x1 - 16 * j, 6) + std::pow(x2 - 16 * i, 6));
		res += divisor;
	}
    }

    return 1 / res;
}

/*
 * This is a procedure to calculate integral of the given function of two arguments numerically.
 *
 *
 * It takes limits of integrating and 'step' argument which is a smallest delta between two points
 * calculated during intergration.
 *
 * Another important note is that this function takes also a target function as well. That allows to use it to calculate
 * integrals for all set of functions of two variables.
 */
double integrate(double (*func)(double, double), double x_from, double x_to, double y_from, double y_to, double step) {
    double result = 0;
    for (double x = x_from; x <= x_to; x += step) {
        for (double y = y_from; y <= y_to; y += step) {
	    // on each iteration we calculate value in the next point and then use step value to extrapolate that result to the single
	    // square
            result += (*func)(x + step / 2.0, y + step / 2.0) * step * step;
        }
    }
    return result;
}


/*
 * This is a main function of this sample application. It handles I/O, MPI operations
 * and manages using integration function. And yes, that was a comment from Captain Obvious. Have a nice day || night.
 */
int main(int argc, char *argv[]) {
    /*
    if we didn't receive enough arguments from the command line -- abort
    */
    if (argc != 6){
	return -1;
    }

    // firstly lets define our variables used for calculating integral
    double x0, x1, y0, y1, pres, res, x_interval;

    // get arguments and convert into double
    try {
	x0 = std::stod(argv[1]);
	x1 = std::stod(argv[2]);
	y0 = std::stod(argv[3]);
	y1 = std::stod(argv[4]);
	pres = std::stod(argv[5]);
    } catch (...){
	std::cout << "Error parsing arguments. Exiting with -1." << std::endl;
	return -1;
    }
    // and some service variables
    int commsize, rank, len;


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    res = 0;

    // define size of the single integration unit by x axis
    x_interval = (x1 - x0) / commsize;

    // our kind of DTO (well, it's just an array, but it wants to be DTO)
    double init_data[] = {x0, x0 + x_interval, y0, y1, pres};
    if (rank == 0) {
	// start measuring time for performance benchmark
        double start_time = MPI_Wtime();
        for (int i = 1; i < commsize; ++i) {
	    // send DTO to other units and start computing its value
            MPI_Send(init_data, 5, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            init_data[0] = init_data[1];
            init_data[1] += x_interval;
        }

	// and then the main point of the application -- call the integrate function for the process' chunk of data
	// pay attention, that we pass the function to be integrated as well -- that adds a lot of flexibilty.
        res = integrate(dejong_func, init_data[0], init_data[1], init_data[2], init_data[3], init_data[4]);

	// and collect results from other hosts
	for (size_t i = 1; i < commsize; ++i) {
            double tr;
            MPI_Recv(&tr, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            res += tr;
        }

	// final output
        std::cout << "Result: " << res << std::endl;
        std::cout << "Total time: " <<  MPI_Wtime() - start_time << std::endl;
    } else {
	// on other hosts just receive data, compute and reply with a result
        
	//again start measuring time
	double start_time = MPI_Wtime();

        // get its part of data to work on
	MPI_Recv(init_data, 5, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	// call the main function to get partial value of the integral for given data
        res = integrate(dejong_func, init_data[0], init_data[1], init_data[2], init_data[3], init_data[4]);
        
	// send this partial result to the master node
	MPI_Send(&res, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD);
        
	// notify user about time spent by this node
	std::cout << "Process " << rank << " of " << commsize << " execution time: "  << MPI_Wtime() - start_time << std::endl;
    }

    // clean all the MPI stuff
    MPI_Finalize();
    return 0;
}
