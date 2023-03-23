#pragma once

namespace Math
{

    template<typename T, unsigned size>
    class MiddleAverageFilter
    {
        T data[size] = {};
        unsigned id = 0u;
        T sum = 0;
    public:
        void push(const T& value)
        {
            data[id] = value;
            id = (id + 1) % size;
        }
        T getAverage() const
        {
            T sum = 0;
            for (auto& i : data)	sum += i;
            return sum / size;
        }
    };
}
