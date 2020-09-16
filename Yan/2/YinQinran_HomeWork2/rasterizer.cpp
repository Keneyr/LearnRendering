// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>
#include <array>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}


static bool insideTriangle(float x, float y, const std::array<Vector4f,3> _v) //array<Vector3f,3> | const Vector3f* _v
{   
    // TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
    //(a,b) x (c,d) = ad-bc

    int flag = 0;

    Eigen::Vector2f t(x-_v[0].x(),y-_v[0].y()); //v0p
    Eigen::Vector2f g(_v[1].x()-_v[0].x(),_v[1].y()-_v[0].y()); //v0v1
    if(g.x()*t.y()-g.y()*t.x() > 0) flag++; //v0v1 x v0p

    t << x-_v[1].x(), y-_v[1].y(); //v1p
    g << _v[2].x()-_v[1].x(), _v[2].y()-_v[1].y(); //v1v2
    if(g.x()*t.y()-g.y()*t.x() > 0) flag++; 

    t << x-_v[2].x(),y-_v[2].y(); //v2p
    g << _v[0].x()-_v[2].x(), _v[0].y()-_v[2].y(); //v2v0 
    if(g.x()*t.y()-g.y()*t.x() > 0) flag++;

    if(flag == 3 || flag == 0) return true;

    //std::cout<<"calculating...."<<std::endl;

    return false;

}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    //???
    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;

    for (auto& i : ind)
    {
        Triangle t;
        //convert the positions of three vertexs to the camera coordinate
        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };

        
        // for(auto & vert : v)
        // {
        //      std::cout<<vert.z()<<std::endl;
        // } //-2-->7,-5-->10
 
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w();
        }

        //check the vertices z-value after projection operation
        // for(auto & vert : v)
        // {
        //      std::cout<<vert.z()<<std::endl;
        // } //-2-->1.03,-5-->1.02
        
        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        //check the vertices z-value after viewport operation
        // for(auto & vert : v)
        // {
        //      std::cout<<vert.z()<<std::endl;
        // } //-2-->7-->50.8,-5-->10-->50.6



        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t);
        //rasterizer_triangle_ssaa(t);
    }
}



void rst::rasterizer::rasterizer_triangle_ssaa(const Triangle& t)
{    
    auto v = t.toVector4();
    float x_max = v[0].x() > v[1].x() ? (v[0].x() > v[2].x() ? v[0].x() : v[2].x()) : (v[1].x() > v[2].x() ? v[1].x() : v[2].x());
    float x_min = v[0].x() < v[1].x() ? (v[0].x() < v[2].x() ? v[0].x() : v[2].x()) : (v[1].x() < v[2].x() ? v[1].x() : v[2].x());

    float y_max = v[0].y() > v[1].y() ? (v[0].y() > v[2].y() ? v[0].y() : v[2].y()) : (v[1].y() > v[2].y() ? v[1].y() : v[2].y());
    float y_min = v[0].y() < v[1].y() ? (v[0].y() < v[2].y() ? v[0].y() : v[2].y()) : (v[1].y() < v[2].y() ? v[1].y() : v[2].y());
    //SSAA
    for(int x = int(x_min); x < int(x_max); x++)
    {
        for(int y = int(y_min); y < int(y_max); y++)
        {
            //set_pixel(Eigen::Vector3f(x,y,1),Vector3f(255,255,255));

            int occupy_percent = 0;
            

            if(insideTriangle(float(x) + 0.25,float(y) + 0.25,v)) 
            {
                auto[alpha, beta, gamma] = computeBarycentric2D(float(x) + 0.25, float(y) + 0.25, t.v);
                float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                z_interpolated *= w_reciprocal;

                if(z_interpolated < depth_buf[4*(x+(height-1-y)*width)])
                {
                    depth_buf[4*(x+(height-1-y)*width)] = z_interpolated;
                    color_buf[4*(x+(height-1-y)*width)] = t.getColor();

                    occupy_percent++;
                }
            }
            if(insideTriangle(float(x) + 0.75,float(y) + 0.25,v))
            {
                auto[alpha, beta, gamma] = computeBarycentric2D(float(x) + 0.75, float(y) + 0.25, t.v);
                float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                z_interpolated *= w_reciprocal;

                if(z_interpolated < depth_buf[4*(x+(height-1-y)*width)+1])
                {
                    depth_buf[4*(x+(height-1-y)*width)+1] = z_interpolated;
                    color_buf[4*(x+(height-1-y)*width)+1] = t.getColor();

                    occupy_percent++;
                }
            } 
            if(insideTriangle(float(x) + 0.75,float(y) + 0.75,v))
            {
                auto[alpha, beta, gamma] = computeBarycentric2D(float(x) + 0.75, float(y) + 0.75, t.v);
                float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                z_interpolated *= w_reciprocal;

                if(z_interpolated < depth_buf[4*(x+(height-1-y)*width)+2])
                {
                    depth_buf[4*(x+(height-1-y)*width)+2] = z_interpolated;
                    color_buf[4*(x+(height-1-y)*width)+2] = t.getColor();

                    occupy_percent++;
                }
            } 
            if(insideTriangle(float(x) + 0.25,float(y) + 0.75,v))
            {
                auto[alpha, beta, gamma] = computeBarycentric2D(float(x) + 0.25, float(y) + 0.75, t.v);
                float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                z_interpolated *= w_reciprocal;

                if(z_interpolated < depth_buf[4*(x+(height-1-y)*width)+3])
                {
                    depth_buf[4*(x+(height-1-y)*width)+3] = z_interpolated;
                    color_buf[4*(x+(height-1-y)*width)+3] = t.getColor();

                    occupy_percent++; 
                }
            }
            
            if(occupy_percent >= 1) //>=75%
            {
                //std::cout<<occupy_percent<<std::endl;

                Vector3f color(0,0,0);
                for(int i=0;i<4;i++)
                {
                        color += color_buf[4*(x+(height-1-y)*width)+i]/4.0f;
                }
                //color/=4.0f;
                //std::cout<<color<<std::endl;
                set_pixel(Eigen::Vector3f(x, y, 1), color);
                //set_pixel(Eigen::Vector3f(x+0.25, y+0.25, depth_buf[4*x+(height-1-y)*width]), color);
                //set_pixel(Eigen::Vector3f(x+0.75, y+0.25, depth_buf[4*x+(height-1-y)*width+1]), color);
                //set_pixel(Eigen::Vector3f(x+0.75, y+0.75, depth_buf[4*x+(height-1-y)*width+2]), color);
                //set_pixel(Eigen::Vector3f(x+0.25, y+0.75, depth_buf[4*x+(height-1-y)*width+3]), color);
                
            }
            

        }
    }

}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t) {
    
    auto v = t.toVector4();
    
    // TODO : Find out the bounding box of current triangle.
    float x_max = v[0].x() > v[1].x() ? (v[0].x() > v[2].x() ? v[0].x() : v[2].x()) : (v[1].x() > v[2].x() ? v[1].x() : v[2].x());
    float x_min = v[0].x() < v[1].x() ? (v[0].x() < v[2].x() ? v[0].x() : v[2].x()) : (v[1].x() < v[2].x() ? v[1].x() : v[2].x());

    float y_max = v[0].y() > v[1].y() ? (v[0].y() > v[2].y() ? v[0].y() : v[2].y()) : (v[1].y() > v[2].y() ? v[1].y() : v[2].y());
    float y_min = v[0].y() < v[1].y() ? (v[0].y() < v[2].y() ? v[0].y() : v[2].y()) : (v[1].y() < v[2].y() ? v[1].y() : v[2].y());

    // iterate through the pixel and find if the current pixel is inside the triangle
    // If so, use the following code to get the interpolated z value.
    //auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
    //float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
    //float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
    //z_interpolated *= w_reciprocal;

    // TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.
    
    for(int x = int(x_min); x < int(x_max); x++)
    {
        for(int y = int(y_min); y< int(y_max);y++)
        {
            if(insideTriangle(float(x) + 0.5, float(y) + 0.5, v)) //array<Vector3f,3> --> const Vector3f* _v
            {
                auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
                float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                z_interpolated *= w_reciprocal;

                //std::cout<<z_interpolated<<" ";

                //compare z_interpolated to z_buffer,update the depth_buf
                if(z_interpolated < depth_buf[x+(height-1-y)*width])
                {
                    depth_buf[x+(height-1-y)*width] = z_interpolated;
                    set_pixel(Eigen::Vector3f(x, y, z_interpolated), t.getColor()); //const Eigen::Vector3f& color
                }
                
            }
        }
    }       

}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
        std::fill(color_buf.begin(),color_buf.end(),Eigen::Vector3f{0, 0, 0}); //SSAA
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity()); //positive infinity
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    
    frame_buf.resize(w * h);

    w = w * 4;
    color_buf.resize(w * h);
    depth_buf.resize(w * h);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-1-y)*width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    
    //
    auto ind = (height-1-point.y())*width + point.x();
    frame_buf[ind] = color;

}

// clang-format on