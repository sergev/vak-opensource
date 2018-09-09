#include <stdio.h>
#include <math.h>

//
// Compute distance from a point to a line segment
//
double distanceToLineSegment(
    double x, double y,     // point C
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

struct {
    double x1, y1;
    double x2, y2;
    double x,  y;
    double distance;
} tests[] = {
    // point A     point B     point C      result
    {   1,  0,      3,  0,      0,  2,      2.236067977500 },
    {   1,  0,      3,  0,      1,  2,      2.0 },
    {   1,  0,      3,  0,      2,  2,      2.0 },
    {   1,  0,      3,  0,      3,  2,      2.0 },
    {   1,  0,      3,  0,      4,  2,      2.236067977500 },

    {   0,  1,      0,  3,      2,  0,      2.236067977500 },
    {   0,  1,      0,  3,      2,  1,      2.0 },
    {   0,  1,      0,  3,      2,  2,      2.0 },
    {   0,  1,      0,  3,      2,  3,      2.0 },
    {   0,  1,      0,  3,      2,  4,      2.236067977500 },

    {   0,  1,      1,  3,      1,  0,      1.414213562373 },
    {   0,  1,      1,  3,      2,  0,      2.236067977500 },
    {   0,  1,      1,  3,      2,  1,      1.788854382000 },
    {   0,  1,      1,  3,      2,  2,      1.341640786500 },
    {   0,  1,      1,  3,      2,  3,      1.0 },
};

//
// Test the distance function.
//
int main ()
{
    const int ntests = sizeof(tests) / sizeof(tests[0]);
    int nerrors = 0;

    for (int i=0; i<ntests; i++) {
        double x1     = tests[i].x1;
        double y1     = tests[i].y1;
        double x2     = tests[i].x2;
        double y2     = tests[i].y2;
        double x      = tests[i].x;
        double y      = tests[i].y;
        double answer = tests[i].distance;

        double result = distanceToLineSegment(x, y, x1, y1, x2, y2);

        printf ("A = %.0f:%.0f, B = %.0f:%.0f, C = %.0f:%.0f, distance = %.12f",
            x1, y1, x2, y2, x, y, result);

        if (fabs(result - answer) < 1e-12) {
            printf (" -- correct\n");
        } else {
            printf (" -- ERROR! should be %.12f\n", answer);
            nerrors++;
        }
    }

    if (nerrors == 0) {
        printf ("\nTest PASSED.\n");
    } else {
        printf ("\nTest FAILED!\n");
    }
    return 0;
}
