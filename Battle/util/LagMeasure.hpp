#pragma once

#include <vector>

#define INITIAL_LAG_TESTS 10

class LagMeasure
{
public:

    LagMeasure(int numMeasures)
        : numMeasures_(numMeasures)
    {
        lag_.reserve(numMeasures);
        for (int i=0; i<numMeasures; i++)
            lag_.push_back(-1);
    }
    ~LagMeasure() {}


    void add(float measure)
    {
        for (int i=0; i<numMeasures_; i++)
            lag_[i] = i == numMeasures_ - 1 ? measure : lag_[i + 1];
    }

    float avg()
    {
        float value = 0;
		float denom = 0;
        for (std::vector<float>::iterator i = lag_.begin(); i!= lag_.end(); i++)
        {
			float v = *i;
			if (v == -1)
				continue;

            value += v;
			denom++;
        }
        return value / denom;
    }

private:
    int numMeasures_;
    std::vector<float> lag_;
};
