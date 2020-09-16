附加题：yes
图片：results文件夹下

rasterize()：求出bounding box，然后遍历盒内的所有像素，如果该像素在三角形内，进行z-buffer处理。、。
rasterize_ssaa()：创建4倍的color_buf和depth_buf，存储每个采样点的颜色，最后进行平均处理。