#include "palette.h"
#include <unordered_map>

class Collector
{
private:
    std::unordered_map<uint32_t, int> m_histogram;
    float m_tolerance;
public:
    Collector(float tolerance = 5.0f);
    ~Collector();

    void add_color(const rgba8888& color);
    void report_palette();
};

Collector::Collector(float tolerance) :
    m_tolerance(tolerance)
{
}

Collector::~Collector()
{
}

void Collector::add_color(const rgba8888& color)
{
    auto h = hash(color);
    if(m_histogram.find(h) != m_histogram.end())
    {
        m_histogram[h] += 1;
    }
    else
    {
        m_histogram[h] = 1;
    }
}

void Collector::report_palette()
{
    for(auto c: m_histogram)
    {
        if(c.second > 500)
        {
            auto color = unhash(c.first);
            fmt::print("{}, {}, {}, {}\n", color.x, color.y, color.z, c.second);
        }
    }
}
