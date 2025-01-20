// This code is to test how much while loop iterates
// in random_unit_vector() at vec3.h to show there is small overhead.
#include <iostream>
#include <random>
#include <cmath>

struct vec3
{
    vec3() : e{0,0,0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}
        
    double e[3];
    
    double length_squared()
    {
        return e[0]*e[0]+e[1]*e[1]+e[2]*e[2];
    }
};

double random_double(double min, double max)
{
    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}


vec3 random_vec3(double min, double max)
{
    return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

inline vec3 operator*(double t, const vec3& v)
{
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3& v, double t)
{
    // use previous overloaded operator
    return t * v;
}

inline vec3 operator/(const vec3& v, double t)
{
    return (1/t) * v;
}

inline std::ostream& operator<<(std::ostream& out, const vec3& v) 
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

int main()
{
    int loops = 1000;
    int tot_cnt = 0;
    
    for(int i = 0; i < loops; i++)
    {
        int cnt = 0;
        while(1)
        {
            cnt++;
            auto p = random_vec3(-1,1);
            auto lensq = p.length_squared();
            if(1e-160 < lensq && lensq <= 1) 
            {
                std::cout << p / std::sqrt(lensq) << '\n';
                std::cout << cnt << '\n';
                break;
            }
        }
        tot_cnt += cnt;
    }
    std::cout << "loops : " << loops << '\n' << "tot_cnt : "<< tot_cnt << '\n';
    std::cout << "avg. cnt : " << static_cast<double>(tot_cnt) / loops << '\n';
}