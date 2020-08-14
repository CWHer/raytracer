#ifndef __ONB__
#define __ONB__

#include "raytracer.h"
#include "vec3.hpp"

class ONB
{
private:
    Vec3 axis[3];

public:
    Vec3 operator[](int i) const { return axis[i]; }

    Vec3 u() const { return axis[0]; }
    Vec3 v() const { return axis[1]; }
    Vec3 w() const { return axis[2]; }

    Vec3 local(double a, double b, double c) const
    {
        return a * u() + b * v() + c * w();
    }

    Vec3 local(const Vec3 &a) const
    {
        return local(a.x(), a.y(), a.z());
    }

    void build_from_w(const Vec3 &n)
    {
        axis[2] = unit_vector(n);
        Vec3 a = (fabs(w().x()) > 0.9) ? Vec3(0, 1, 0) : Vec3(1, 0, 0);
        axis[1] = unit_vector(cross(w(), a));
        axis[0] = cross(w(), v());
    }
};

#endif