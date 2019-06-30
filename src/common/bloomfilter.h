
#include <vector>
#include "binary.h"

namespace wftz {

class BloomFilter
{
public:
    BloomFilter(const int32_t n_, const double false_positive_) :
        _n(n_), _p(false_positive_), _m(0), _k(0), _bits()
    {
        _k = static_cast<int32_t>(-std::log(_p)  / std::log(2));
        _m = static_cast<int32_t>(_k * n_ * 1.0 / std::log(2));
        _bits.resize((_m + 7) / 8);
    }

    virtual ~BloomFilter();

    void insert()

    void check();

private:
    wftz::Binary _bits;
    double _p;
    int32_t _n;
    int32_t _m;
    int32_t _k;
};

}