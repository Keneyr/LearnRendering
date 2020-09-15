#include <algorithm>
#include <cassert>
#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode, SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod), primitives(std::move(p))
{
    time_t start, stop;
    time(&start);

    if (primitives.empty())
        return;
   
    if(splitMethod == SplitMethod::NAIVE) 
        root = recursiveBuild(primitives);
    else if(splitMethod == SplitMethod::SAH)
        root = recursiveBuild_SAH(primitives,bucketNum);

    time(&stop);


    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}

//对所有的objects递归构建bvh二叉树
BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i) //4968
        bounds = Union(bounds, objects[i]->getBounds()); //最大的包围盒
    
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds = Union(centroidBounds, objects[i]->getBounds().Centroid()); //按照物体包围盒重心再求一次更紧密的包围盒
            
        
        int dim = centroidBounds.maxExtent(); //求最长的轴，x为0，y为1，z为2

        //按照该轴上的重心坐标值对物体进行排序
        switch (dim) {
        
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x < f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y < f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z < f2->getBounds().Centroid().z;
            });
            break;
        }

        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuild(leftshapes);
        node->right = recursiveBuild(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}

BVHBuildNode* BVHAccel::recursiveBuild_SAH(std::vector<Object*> objects,int bucketnum)
{
    //std::cout<<"1"<<" ";
    BVHBuildNode* node = new BVHBuildNode();
    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;

    //Bounds3 buckets[bucketnum]; //初始化桶
    //Vector3f offset[objects.size()];
    
    //有一个问题需要思考，那就是到底递归结束的条件是三角形的数量为1或2，还是说bucket的数量为1？
    //还是说如果刚开始bucket是10个，分完之后的bucket还是10个，一直都是10个，直到某个区域的三角形数量是1或2
    
    //经过思考，觉得条件应该是一直递归直到一个区域三角形数量为1个或者两个，然后建立叶子节点，所以bucket应该一直为常数

    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild_SAH(std::vector{objects[0]},1);
        node->right = recursiveBuild_SAH(std::vector{objects[1]},1);

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {

        Bounds3 buckets[bucketnum]; //初始化32个桶
        Vector3f offset[objects.size()];

        Bounds3 centroidBuckets;
        for (int i = 0; i < objects.size(); ++i)
            centroidBuckets = Union(centroidBuckets, objects[i]->getBounds().Centroid()); //按照物体包围盒重心再求一次更紧密的包围盒

        int dim = centroidBuckets.maxExtent(); //求最长的轴，x为0，y为1，z为2

        //无需进行排序？？
        switch (dim) {
        
        case 0:
            //对物体进行排序
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x < f2->getBounds().Centroid().x;
            });
             //得到物体所属的桶索引
            for (int i = 0; i < objects.size(); ++i)
            {
                    offset[i] = centroidBuckets.Offset(objects[i]->getBounds().Centroid());
                    offset[i] = offset[i] * bucketnum;
            }
           
            for(int i = 0;i < objects.size(); ++i)
            {
                //如果offset[i].x = 32，也要放在buckets[31]的桶内
                if(int(offset[i].x) == bucketnum) offset[i].x = bucketnum-1;

                //这里很奇怪，prim_count全都是1????????
                //我知道为啥了，fuck，因为下面的Union直接把prim_count清零了
                
                buckets[int(offset[i].x)].prim_count++; //该bucket内部的物体数量

                //std::cout<<int(offset[i].x)<<std::endl;

                buckets[int(offset[i].x)] = Union(buckets[int(offset[i].x)],objects[i]->getBounds());
                
                
                
            } //此刻buckets以及内部的物体全都布局over
            break;
        case 1:
            //对物体进行排序
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y < f2->getBounds().Centroid().y;
            });
             //得到物体所属的桶索引
            for (int i = 0; i < objects.size(); ++i)
            {
                    offset[i] = centroidBuckets.Offset(objects[i]->getBounds().Centroid());
                    offset[i] = offset[i] * bucketnum;
            }
            for(int i = 0;i < objects.size(); ++i)
            {   
                if(int(offset[i].y) == bucketnum) offset[i].y = bucketnum-1;
                buckets[int(offset[i].y)].prim_count++; //该bucket内部的物体数量
                buckets[int(offset[i].y)] = Union(buckets[int(offset[i].y)],objects[i]->getBounds());
                
            } //此刻buckets以及内部的物体全都布局over
            
            break;
        case 2:
            //对物体进行排序
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z < f2->getBounds().Centroid().z;
            });
             //得到物体所属的桶索引
            for (int i = 0; i < objects.size(); ++i)
            {
                    offset[i] = centroidBuckets.Offset(objects[i]->getBounds().Centroid());
                    offset[i] = offset[i] * bucketnum;
            }
            for(int i = 0;i < objects.size(); ++i)
            {
                if(int(offset[i].z == bucketnum)) offset[i].z = bucketnum-1;
                buckets[int(offset[i].z)].prim_count++; //该bucket内部的物体数量
                buckets[int(offset[i].z)] = Union(buckets[int(offset[i].z)],objects[i]->getBounds());
                
            } //此刻buckets以及内部的物体全都布局over
           
            break;
        }


        auto beginning = objects.begin(); //指针
        auto ending = objects.end();

        /*//根据cost最小值判断partition的位置*/
        
        double costMin = std::numeric_limits<double>::max();
        double cost;

         //求出第0---i个桶的表面积和其下面的物体数量的总和,i+1---bucketnum-1的桶的表面积和其下面的物体数量的总和
        
        double sa,sb; //A区域的表面积,B区域的表面积
        int na = 0; //A区域的物体数量
        int partition_index;
        Bounds3 tmp_a = buckets[0];
        
        for(int i = 0; i < bucketnum; i++)
        {
            na += buckets[i].prim_count;
            
            //实验区域A是求并
            tmp_a = Union(tmp_a,buckets[i]);
            sa = tmp_a.SurfaceArea();

            //实验区域B是求减--但是减好像没法求，所以只能遍历求并
            Bounds3 tmp_b = buckets[bucketnum-1];
            for(int j = bucketnum -1; j > i; j--)
            {
                tmp_b = Union(tmp_b,buckets[j]);
            }
            sb = tmp_b.SurfaceArea();
            
            cost = na*sa/centroidBuckets.SurfaceArea()+(objects.size()-na)*sb/centroidBuckets.SurfaceArea();

            if(cost < costMin)
            {
                std::swap(cost,costMin);
                partition_index = i;
            }
        }
        //如何从partition_index转到vector<Object*>指针？在这里我怎么觉得得对Object进行排序呢？而且函数参数是否得有桶的index
        //auto partition = objects.begin() + (objects.size() / 2);
        int d = 0;
        for(int i = 0; i <= partition_index; i++)
        {
            d += buckets[i].prim_count;
        }
        auto partition = objects.begin() + d; //如果这么写的话，物体在之前一定要排序吧

        auto leftshapes = std::vector<Object*>(beginning, partition);
        auto rightshapes = std::vector<Object*>(partition, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        //node->left = recursiveBuild_SAH(leftshapes,partition_index + 1);
        //node->right = recursiveBuild_SAH(rightshapes,bucketnum - partition_index -1);
        node->left = recursiveBuild_SAH(leftshapes,bucketnum);
        node->right = recursiveBuild_SAH(rightshapes,bucketnum);

        node->bounds = Union(node->left->bounds, node->right->bounds);
           
    }

    return node;
}


Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    // TODO Traverse the BVH to find intersection
    
    //????
    std::array<int,3> dirIsNeg = {int(ray.direction.x > 0),int(ray.direction.y > 0),int(ray.direction.z > 0 )};
    
    if(!(node->bounds.IntersectP(ray,ray.direction_inv,dirIsNeg)))
        return Intersection();
    
    if(node->left == nullptr && node->right == nullptr)
    {
        //std::cout<<"reach to the leaf node, begin to test the object with ray..."<<std::endl;
       
        //test intersection with all objs; return closet intersection
        return node->object->getIntersection(ray);
        
    }
    
    Intersection hit1,hit2;
    hit1 = getIntersection(node->left,ray);
    hit2 = getIntersection(node->right,ray);
    
    return hit1.distance < hit2.distance ? hit1 : hit2;
}

