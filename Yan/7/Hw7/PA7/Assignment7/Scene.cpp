//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"
#include <iostream>

void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}
//在场景的所有光源上按面积uniform地sample一个点，并计算该sample的概率密度
void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;

    
    for (uint32_t k = 0; k < objects.size(); ++k) {
        //判断该物体是不是光源，如果是，获取光源面积
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    //为什么要用p？？？
    //为了对光源进行随机采样把
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }
    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    if(depth>this->maxDepth) //this->maxDepth初始化为1
    {
        return Vector3f(0.0,0.0,0.0);
    }

    Intersection intersection = Scene::intersect(ray); //射线是否与场景中的物体相交
    
    
    Object *hitObject = intersection.obj; //?????似乎没啥用
    Vector3f hitColor = this->backgroundColor;
    Vector3f p = intersection.coords;

    uint32_t index = 0;
    Vector2f uv,st;

    Material *m = intersection.m;
    Vector3f N = normalize(intersection.normal); //是否需要再次normalize??

    if(intersection.happened) //如果光线和场景中物体表面相交
    {
        
        //判断这个是物体是光源还是普通物体
        if(m->hasEmission())
        {
            //该物体是光源,难道这里的光源是嵌套在天花板上，自成物体？
            // 经实验发现，光源就是光源，就那一个小方块，跟其他无关
            hitColor =  m->m_emission;
        }
        //普通物体
        else
        {
            //获取该相交处的光源
            Intersection inter_light; //光源
            float pdf_light; //
            //sampleLight(inter,pdf_light)
            sampleLight(inter_light,pdf_light);

           
            //get x,ws,NN from inter
            
            Vector3f light_x = inter_light.coords; //光的位置
            Vector3f ws = normalize(light_x - p); //光的位置-交点p的位置
            Vector3f NN = normalize(inter_light.normal);
            
            //Vector3f wo = p - ray.origin; //还是ray.dir呢？？
            //Vector3f wo = ray.direction; //难道得是ray.direction取反？
            Vector3f wo = -ray.direction; //改了以后感觉没啥影响啊??
            Vector3f li = inter_light.emit;

            Vector3f l_dir;

            //shoot a ray from p to x
            Ray block_ray(light_x,normalize(p-light_x)); //origin,dir
            
            //if the ray is not blocked in the middle
            //从light_x到p发出一根光线，如果最终交点是p，则说明中间没有其他物体
            Intersection block_intersection = intersect(block_ray);

            //只判断直接光源这块儿发现天花板依然是纯黑色，但是直观的想，如果只有直接光源，天花板应该就是黑的，因为它和光源在一个平面内
            if(block_intersection.coords.Equal(intersection.coords))
            {
                //L_dir = L_i * eval(wo,ws,N) * dot(ws,N) * dot(ws,NN) / |x-p|^2 /pdf_light
                //Li 是对光源进行采样得到的 emission，保存在 Intersection 类的 emit 参数中
                l_dir = li * m->eval(wo,ws,N) * dotProduct(ws,N) * dotProduct(-ws,NN) / pow((light_x - p).norm(),2) / pdf_light;
            }
            //L_indir = 0
            Vector3f l_indir(0.f,0.f,0.f);
            
            //如果这个点被(眼睛)发出的射线碰到了，就让这个点的颜色为白色（为了试验天花板是否全被碰撞到）
            //经试验，天花板的确被(眼睛)发出的射线碰撞到了
            //l_indir = Vector3f(0.6f,0.6f,0.6f);
            
            //Test Russian Roulette with probability RussianRoulette
            //难道这里需要写一个随机数？进行轮盘赌
            float probability = get_random_float(); //这随机数是从0-1吗？
            std::cout<<"probability: "<<probability<<std::endl;
            
            if(probability <= RussianRoulette)
            {
                //wi = sample(wo,N)
                //Vector3f wi = normalize(m->sample(wo,N));

                Vector3f wi = m->sample(wo,N);

                //Trace a ray r(p,wi)
                Ray object_ray(p,normalize(wi-p));
                //if ray r hit a non-emitting object at q
                Intersection q = intersect(object_ray);

                //测试天花板靠前那块儿是否能够弹射光到其他物体
                //实验发现，在这里就有一部分天花板无法弹射光到其他物体导致全黑，这明显不符合正确结果
                //if(q.happened)
                //{
                    //l_indir = Vector3f(0.6,0.6,0.6);
                //}
            
                //碰到了不发光的物体
                if(q.happened && (!q.m->hasEmission())) //q.m->hasEmission()
                {
                    l_indir = castRay(Ray(p,normalize(q.coords-p)),0) * m->eval(wo,wi,N) * dotProduct(wi,N) / m->pdf(wo,wi,N) / RussianRoulette;
                
                }
            }

            //return L_dir + L_indir
            hitColor = l_dir + l_indir;
        }
        
    }

    return hitColor;
}