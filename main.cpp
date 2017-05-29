//
// Created by knidarkness on 27.05.17.
//

#include <mpi.h>
#include <iostream>
#include <cmath>

double dejong_func(double x1, double x2);
double integrate(double (*func)(double, double), double x_from, double x_to, double y_from, double y_to, double step);


double dejong_func( double x1, double x2) {
    /*
     * This procedure calculates the value of the de Jong's function in given (x1,x2) point and returns
     * it's double value.
     */
    double res = 0.002;

    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++){
		double divisor = 1 / std::abs(5*(i+2) + j + 3 + std::pow(x1 - 16 * j, 6) + std::pow(x2 - 16 * i, 6));
		res += divisor;
	}
    }

    return 1 / res;
}

double integrate(double (*func)(double, double), double x_from, double x_to, double y_from, double y_to, double step) {
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
    double result = 0;
    for (double x = x_from; x <= x_to; x += step) {
        for (double y = y_from; y <= y_to; y += step) {
            result += (*func)(x + step / 2.0, y + step / 2.0) * step * step;
        }
    }
    return result;
}


int main(int argc, char *argv[]) {
    /*
    if we didn't receive enough arguments from the command line -- abort
    */
    if (argc != 6){
	return -1;
    }

    double x0, x1, y0, y1, pres, res, x_interval;

    // get arguments and convert into double
    x0 = std::stod(argv[1]);
    x1 = std::stod(argv[2]);
    y0 = std::stod(argv[3]);
    y1 = std::stod(argv[4]);
    pres = std::stod(argv[5]);

    int commsize, rank, len;
    char procname[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(procname, &len);

    res = 0;
    // define size of the single integration unit by x axis
    x_interval = (x1 - x0) / commsize;

    // our kind of DTO (well, it's just an array, but it wants to be DTO)
    double init_data[] = {x0, x0 + x_interval, y0, y1, pres};
    if (rank == 0) {
	// send DTO to other units and start computing its value
        double start_time = MPI_Wtime();
        for (int i = 1; i < commsize; ++i) {
            MPI_Send(init_data, 5, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            init_data[0] = init_data[1];
            init_data[1] += x_interval;
        }
        res = integrate(dejong_func, init_data[0], init_data[1], init_data[2], init_data[3], init_data[4]);
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
        double start_time = MPI_Wtime();
        MPI_Recv(init_data, 5, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        res = integrate(dejong_func, init_data[0], init_data[1], init_data[2], init_data[3], init_data[4]);
        MPI_Send(&res, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD);
        std::cout << "Process " << rank << " of " << commsize << " execution time: "  << MPI_Wtime() - start_time << std::endl;
    }

    MPI_Finalize();
    return 0;
}
