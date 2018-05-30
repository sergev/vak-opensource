/*
 * Example of NLopt application for CarND project.
 *
 * Copyright (C) 2018 Serge Vakulenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <nlopt.h>

#define NPOINTS 6

const double MAX_SPEED      = 90;               // maximum speed, miles per hour
const double PRED_TIME_STEP = 0.1;              // prediction time step in seconds
const int    N_STEP         = 14;               // number of prediction steps
const double MAX_STEERING   = 25 * M_PI/180;    // max 25 degrees
const double MAX_THROTTLE   = 1.0;

int count_obj = 0;
int count_constraint = 0;

struct car_data {
    double x, y, psi, v;    // state
    double ref_x[NPOINTS];  // reference trajectory
    double ref_y[NPOINTS];  // reference trajectory
    double max_cte;         // computed cross-track error
};

//
// The global kinematic model.
// Update the state of the car after time dt.
//
void updateKinematic(double state[4], const double actuator[2], double dt)
{
    // distance between the front wheel and the vehicle center
    const double LF = 3.4;

    // Conversion from MPH to meters per second.
    const double MPH_TO_METERS_PER_SEC = 0.44704;
    const double METERS_PER_SEC_TO_MPH = 2.23694;

    double px       = state[0];
    double py       = state[1];
    double psi      = state[2];
    double v        = state[3] * MPH_TO_METERS_PER_SEC;
    double steering = actuator[0];
    double throttle = actuator[1];

    // Acceleration in meters per sec^2 at maximum throttle.
    const double accel = (throttle >= 0) ? 6.0 : 20.0;

    state[0] = px  + (v * cos(psi) * dt);
    state[1] = py  + (v * sin(psi) * dt);
    state[2] = psi - (v * steering/LF * dt);
    state[3] = (v + (throttle * accel * dt)) * METERS_PER_SEC_TO_MPH;
}

//
// Compute distance from a point to a line segment
//
double distanceToLineSegment(
    double x,  double y,    // point C
    double x1, double y1,   // segment end A
    double x2, double y2)   // segment end B
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    double dx1 = x1 - x;
    double dy1 = y1 - y;
    double dx2 = x2 - x;
    double dy2 = y2 - y;

    // Dot product AB*AC is positive, when the angle is acute.
    double dot_product_ab_ac = - dx*dx1 - dy*dy1;
    if (dot_product_ab_ac <= 0) {
        // Angle A is obtuse: use distance |AC|.
        return sqrt(dx1*dx1 + dy1*dy1);
    }

    // Dot product BA*BC is positive, when the angle is acute.
    double dot_product_ba_bc = dx*dx2 + dy*dy2;
    if (dot_product_ba_bc <= 0) {
        // Angle B is obtuse: use distance |BC|.
        return sqrt(dx2*dx2 + dy2*dy2);
    }

    // Both angles A and B are acute.
    // Compute distance to the line.
    return fabs(dy*x - dx*y + x2*y1 - y2*x1) / sqrt(dy*dy + dx*dx);
}

//
// Compute distance from a point to a reference trajectory
//
double distanceToRefTrajectory(struct car_data *data, double x, double y)
{
    // Get distance to the first trajectory segment.
    double distance = distanceToLineSegment(x, y,
        data->ref_x[0], data->ref_y[0], data->ref_x[1], data->ref_y[1]);

    for (unsigned i=2; i<NPOINTS; ++i) {

        // Distance to the next trajectory segment.
        double d = distanceToLineSegment(x, y,
            data->ref_x[i-1], data->ref_y[i-1], data->ref_x[i], data->ref_y[i]);

        if (d < distance)
            distance = d;
    }
    return distance;
}

//
// Check whether a point lies beyond the reference trajectory
//
int beyondTrajectory(struct car_data *data, double x, double y)
{
    const int last = NPOINTS - 1;

    double x1 = data->ref_x[last],   y1 = data->ref_y[last];      // last point A
    double x2 = data->ref_x[last-1], y2 = data->ref_y[last-1];    // previous point B

    // Dot product AB*AC is negative, when the angle is obtuse.
    double dot_product_ab_ac = (x2 - x1)*(x - x1) +
                               (y2 - y1)*(y - y1);
    return (dot_product_ab_ac < 0);
}

void checkBounds(double steering, double throttle)
{
    if (steering < -MAX_STEERING || steering > MAX_STEERING) {
        printf("--- %s() wrong steering = %g\n", __func__, steering);
        exit(1);
    }
    if (throttle < -MAX_THROTTLE || throttle > MAX_THROTTLE) {
        printf("--- %s() wrong throttle = %g\n", __func__, throttle);
        exit(1);
    }
}

//
// Objective function.
//
double obj_func(unsigned n, const double *actuator, double *grad, void *arg)
{
    struct car_data *data = arg;

    count_obj++;
    printf("--- %s(actuator = %g, %g) eval #%d\n", __func__,
        actuator[0], actuator[1], count_obj);
    checkBounds(actuator[0], actuator[1]);

    double t        = 0;
    double penalty  = 0.0;
    double cte;              // current cross track error
    double max_cte  = 0.0;
    double ss_speed = 0.0;   // sum of square of speed

    // Initial state.
    double next_state[4] = { data->x, data->y, data->psi, data->v };

    for (int i=0; i<N_STEP; ++i) {
        t += PRED_TIME_STEP;

        // compute the next state
        updateKinematic(next_state, actuator, PRED_TIME_STEP);

        if (beyondTrajectory(data, next_state[0], next_state[1])) {
            // Predicted point is beyond the reference trajectory
            printf("beyond trajectory!\n");
            exit(1);
            break;
        }

        // calculate the cross track error
        cte = distanceToRefTrajectory(data, next_state[0], next_state[1]);

        if (cte > max_cte) {
            max_cte = cte;
        }
        penalty += cte * cte * t;
        ss_speed += next_state[3] * next_state[3];
    }

    // speed control
    if (data->v < MAX_SPEED) {
        penalty -= (MAX_SPEED - data->v) * 1e-4 * ss_speed;
    }

    // constraint value
    data->max_cte = max_cte;

    printf("---     return penalty %g\n", penalty);
    return penalty;
}

//
// Constraint function.
//
double constraint_func(unsigned n, const double *actuator, double *grad, void *arg)
{
    struct car_data *data = arg;

    count_constraint++;
    printf("--- %s(actuator = %g, %g) eval #%d, return %g\n", __func__,
        actuator[0], actuator[1], count_constraint, data->max_cte - 1.0);
    checkBounds(actuator[0], actuator[1]);

    // Cross-track error should not exceed the track width.
    // Use the value precomputed by obj_func().
    return data->max_cte - 1.0;
}

int main()
{
    // Use local derivative-free optimization algorithm:
    // variant of NEWUOA with bound constraints
    nlopt_algorithm algorithm = NLOPT_LN_NEWUOA_BOUND;

    // Create optimizer object.
    // Select dimensionality.
    nlopt_opt opt = nlopt_create(algorithm, 2);

    // Prepare data for the objective function.
    // Data from CarND MPC run, time 8.6s.
    struct car_data data = {
        -175.319, -77.9237, 4.6791, 74.6941,
        { -176.962, -176.886, -175.082, -170.362, -164.422, -158.942 },
        { -76.8506, -90.6406, -100.321, -115.129, -124.521, -131.399 },
    };

    // Set objective function.
    nlopt_set_min_objective(opt, obj_func, &data);

    // Specify bounds.
    double lower_bounds[2] = {-MAX_STEERING, -MAX_THROTTLE};
    double upper_bounds[2] = {MAX_STEERING,  MAX_THROTTLE};
    nlopt_set_lower_bounds(opt, lower_bounds);
    nlopt_set_upper_bounds(opt, upper_bounds);

    // Add constraint.
    nlopt_add_inequality_constraint(opt, constraint_func, &data, 0);

    // Stopping criteria, or. a relative tolerance on the optimization parameters.
    nlopt_set_xtol_rel(opt, 1e-4);

    // Perform the optimization, starting with some initial guess.
    double actuator[2] = { -0.0350148, -0.272836 }; // initial guess
    double cost;                                    // the minimum objective value upon return
    nlopt_result status = nlopt_optimize(opt, actuator, &cost);

    if (status < 0) {
        printf("nlopt failed!\n");
    } else if (count_constraint == 0) {
        printf("constraint function never called!\n");
    } else {
        printf("found minimum after %d evaluations\n", count_obj);
        printf("cost = %g\n", cost);                                        // expect -112.098
        printf("steering = %g, throttle = %g\n", actuator[0], actuator[1]); // expect -0.0374338, 0.359192
    }

    nlopt_destroy(opt);
    return 0;
}
