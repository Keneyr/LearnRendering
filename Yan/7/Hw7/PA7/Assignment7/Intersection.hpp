//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_INTERSECTION_H
#define RAYTRACING_INTERSECTION_H
#include "Vector.hpp"
#include "Material.hpp"
class Object;
class Sphere;

struct Intersection
{
    Intersection(){
        happened=false;
        coords=Vector3f();
        normal=Vector3f();
        distance= std::numeric_limits<double>::max();
        obj =nullptr;
        m=nullptr;
    }
    bool happened; //是否相交
    Vector3f coords; //位置坐标
    Vector3f tcoords; 
    Vector3f normal; //法线
    Vector3f emit; //采样得到的光源的emission(Li)
    double distance; //到屏幕的深度
    Object* obj; //相交的物体
    Material* m; //相交的材质
};
#endif //RAYTRACING_INTERSECTION_H
