
包围盒求交：完成
BVH查找：完成
提高SAH：完成

IntersectP(): 判断ray与包围盒是否相交。求出tEnter和tExit。dirIsNet是为了判断ray的方向是否为负，如果为负，需要置换相交的min和max值。


getIntersection():实现光线与场景的求交。递归查找node结点，得到光线与node结点下的物体的相交信息。


recursiveBuild_SAH()：自定义SAH函数，递归建bvh树。在进行node划分的时候，运用了SAH方法计算cost，从而得到合理的划分点(cost最小)。桶的数量为10。递归方法同NAIVE方法一样。



