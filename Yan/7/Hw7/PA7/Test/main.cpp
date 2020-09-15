#include <iostream>
#include <random>



inline float get_random_float()
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<float> dist(0.f, 1.f); // distribution in range [1, 6]

    return dist(rng);
}

int main()
{
    int m = 0;
    while(m<10)
    {
        float random_r = get_random_float();

        std::cout<<random_r<<std::endl;

        m++;
    }
    system("pause");
    return 0;
}