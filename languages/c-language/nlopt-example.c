#include <stdio.h>
#include <math.h>
#include <nlopt.h>

int count = 0;

//
// Objective function.
//
double myfunc(unsigned n, const double *x, double *grad, void *my_func_data)
{
    count++;
    if (grad) {
        grad[0] = 0.0;
        grad[1] = 0.5 / sqrt(x[1]);
    }
    return sqrt(x[1]);
}

typedef struct {
    double a, b;
} my_constraint_data;

//
// Constraint function.
//
double myconstraint(unsigned n, const double *x, double *grad, void *data)
{
    my_constraint_data *d = (my_constraint_data*) data;
    double a = d->a,
           b = d->b;
    if (grad) {
        grad[0] = 3 * a * (a*x[0] + b) * (a*x[0] + b);
        grad[1] = -1.0;
    }
    return ((a*x[0] + b) * (a*x[0] + b) * (a*x[0] + b) - x[1]);
}

int main()
{
    // Select algorithm.

    //
    // Global optimization
    //
//  nlopt_algorithm algorithm = NLOPT_GN_DIRECT;        // DIviding RECTangles algorithm for global optimization
//  nlopt_algorithm algorithm = NLOPT_GN_DIRECT_L;      // locally biased variant of DIRECT
//  nlopt_algorithm algorithm = NLOPT_GN_DIRECT_L_RAND; // slightly randomized variant of DIRECT-L
//  nlopt_algorithm algorithm = NLOPT_GN_CRS2_LM;       // Controlled Random Search with local mutation
//  nlopt_algorithm algorithm = NLOPT_G_MLSL;           // Multi-Level Single-Linkage
//  nlopt_algorithm algorithm = NLOPT_G_MLSL_LDS;       // modification of MLSL with low-discrepancy sequence
//  nlopt_algorithm algorithm = NLOPT_GD_STOGO;         // StoGO
//  nlopt_algorithm algorithm = NLOPT_GD_STOGO_RAND;    // randomized variant of StoGO
//  nlopt_algorithm algorithm = NLOPT_GN_ISRES;         // Improved Stochastic Ranking Evolution Strategy
//  nlopt_algorithm algorithm = NLOPT_GN_ESCH;          // evolutionary algorithm

    //
    // Local derivative-free optimization
    //
//  nlopt_algorithm algorithm = NLOPT_LN_COBYLA;        // Constrained Optimization BY Linear Approximations
//  nlopt_algorithm algorithm = NLOPT_LN_BOBYQA;        // BOBYQA, iterative quadratic approximation
//  nlopt_algorithm algorithm = NLOPT_LN_NEWUOA;        // NEWUOA, quadratic approximation
//  nlopt_algorithm algorithm = NLOPT_LN_NEWUOA_BOUND;  // variant of NEWUOA with bound constraints
//  nlopt_algorithm algorithm = NLOPT_LN_PRAXIS;        // PRincipal AXIS
//  nlopt_algorithm algorithm = NLOPT_LN_NELDERMEAD;    // Nelder-Mead Simplex
//  nlopt_algorithm algorithm = NLOPT_LN_SBPLX;         // re-implementation of Subplex algorithm

    //
    // Local gradient-based optimization
    //
    nlopt_algorithm algorithm = NLOPT_LD_MMA;           // Method of Moving Asymptotes
//  nlopt_algorithm algorithm = NLOPT_LD_CCSAQ;         // conservative convex separable approximation, quadratic
//  nlopt_algorithm algorithm = NLOPT_LD_SLSQP;         // Sequential Least-Squares Quadratic Programming
//  nlopt_algorithm algorithm = NLOPT_LD_LBFGS;         // Low-storage BFGS
//  nlopt_algorithm algorithm = NLOPT_LD_TNEWTON;       // Truncated Newton
//  nlopt_algorithm algorithm = NLOPT_LD_VAR1;          // shifted limited-memory variable-metric algorithm, rank-1
//  nlopt_algorithm algorithm = NLOPT_LD_VAR2;          // shifted limited-memory variable-metric algorithm, rank-2

    // Create optimizer object.
    // Select dimensionality.
    nlopt_opt opt = nlopt_create(algorithm, 2);

    // Set objective function.
    nlopt_set_min_objective(opt, myfunc, NULL);

    // Specify bounds.
    double lb[2] = { -HUGE_VAL, 0 }; // lower bounds
    nlopt_set_lower_bounds(opt, lb);
    //TODO: nlopt_set_upper_bounds();

    // Add constraints.
    my_constraint_data data[2] = { {2, 0}, {-1, 1} };
    nlopt_add_inequality_constraint(opt, myconstraint, &data[0], 1e-8);
    nlopt_add_inequality_constraint(opt, myconstraint, &data[1], 1e-8);

    // Stopping criteria, or. a relative tolerance on the optimization parameters.
    nlopt_set_xtol_rel(opt, 1e-4);

    // Perform the optimization, starting with some initial guess.
    double x[2] = { 1.234, 5.678 }; // initial guess
    double minf;                    // the minimum objective value upon return

    if (nlopt_optimize(opt, x, &minf) < 0) {
        printf("nlopt failed!\n");
    } else {
        printf("found minimum after %d evaluations\n", count);
        printf("found minimum at f(%g, %g) = %0.10g\n", x[0], x[1], minf);
    }

    nlopt_destroy(opt);
    return 0;
}
