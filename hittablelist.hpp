#ifndef __HITTABLELIST__
#define __HITTABLELIST__

#include "raytracer.h"
#include "hittable.h"
#include "aabb.hpp"
#include "bvh.hpp"

#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr;

class HittableList : public Hittable
{
private:
    static const size_t threshold = 10;
    std::vector<shared_ptr<Hittable>> objects;
    shared_ptr<BVHnode> root; //BVH

public:
    HittableList() {}
    HittableList(shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<Hittable> object) { objects.push_back(object); }

    bool hit_force(const Ray &r, double tmin, double tmax, hit_record &rec) const
    {
        hit_record temp_rec;
        bool hit_anything = 0;
        auto closest_so_far = tmax;

        for (const auto &object : objects)
        {
            if (object->hit(r, tmin, closest_so_far, temp_rec)) //only hit the closest one
            {
                hit_anything = 1;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    bool hit(const Ray &r, double tmin, double tmax, hit_record &rec) const override
    {
        if (objects.size() <= threshold)
            return hit_force(r, tmin, tmax, rec);
        return root->hit(r, tmin, tmax, rec);
    }

    void build()
    {
        if (objects.size() <= threshold)
            return;
        root = make_shared<BVHnode>(objects, 0, objects.size(), eps, infinity);
    }

    bool bounding_box(double t0, double t1, AABB &output_box) const override
    {
        if (objects.empty())
            return 0;
        AABB temp_box;
        bool first_box = 1;

        for (const auto &object : objects)
        {
            if (!object->bounding_box(t0, t1, temp_box))
                return 0;
            output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
            first_box = 0;
        }
        return 1;
    }
};

#endif