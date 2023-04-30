//
// Created by Gzalo on 29/4/2023.
//

#include "collisions.h"
#include <cmath>
using namespace std;
// From ChatGPT (Snippet to calculate distance between line (defined by two points) and a point and can you make it so that points ouside the defined ones work as pills?)
double distance_between_line_and_point(pair<double, double> p1, pair<double, double> p2, pair<double, double> p) {
    double x1 = p1.first;
    double y1 = p1.second;
    double x2 = p2.first;
    double y2 = p2.second;
    double xp = p.first;
    double yp = p.second;

    double dx = x2 - x1;
    double dy = y2 - y1;
    double u = ((xp - x1) * dx + (yp - y1) * dy) / (dx * dx + dy * dy);
    double x = x1 + u * dx;
    double y = y1 + u * dy;

    if (u < 0) {
        return sqrt((xp - x1) * (xp - x1) + (yp - y1) * (yp - y1));
    } else if (u > 1) {
        return sqrt((xp - x2) * (xp - x2) + (yp - y2) * (yp - y2));
    } else {
        return sqrt((xp - x) * (xp - x) + (yp - y) * (yp - y));
    }
}