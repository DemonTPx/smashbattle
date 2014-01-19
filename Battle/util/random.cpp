
#include <random>
#include <iostream>
#include <cstdint>

struct MT19937 {
private:
    static std::mt19937_64 rng;
public:
    // This is equivalent to srand().
    static void seed(uint64_t new_seed = std::mt19937_64::default_seed) {
        rng.seed(new_seed);
    }

    // This is equivalent to rand().
    static uint64_t get() {
        return rng();
    }
};

std::mt19937_64 MT19937::rng;

void rand_init()
{
	MT19937::seed(1001);
}

uint64_t rand_get()
{
	return MT19937::get();
}
   