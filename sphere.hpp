#ifndef __SPHERE__
#define __SPHERE__

#include "hittable.h"
#include "vec3.hpp"

class Sphere : public Hittable
{
private:
    Point3 center;
    double radius;

public:
    Sphere() {}
    Sphere(Point3 _c, double _r) : center(_c), radius(_r) {}

    bool hit(const Ray &r, double tmin, double tmax, hit_record &rec) const
    {
        Vec3 oc = r.origin() - center;
        auto a = r.direction().length_sqr();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_sqr() - radius * radius;
        auto discriminant = half_b * half_b - a * c;

        if (discriminant > 0)
        {
            auto root = sqrt(discriminant);
            auto temp = (-half_b - root) / a; //negative root
            if (temp < tmax && temp > tmin)
            {
                rec.t = temp;
                rec.p = r.at(rec.t);
                rec.norm = (rec.p - center) / radius;
                return 1;
            }
            temp = (-half_b + root) / a; //positive root
            if (temp < tmax && temp > tmin)
            {
                rec.t = temp;
                rec.p = r.at(rec.t);
                rec.norm = (rec.p - center) / radius;
                return 1;
            }
        }
        return 0;
    }
};

#endif