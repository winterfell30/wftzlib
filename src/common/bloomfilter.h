
#include <vector>
#include "binary.h"

namespace wftz {

class BloomFilter
{
public:
    BloomFilter();

    virtual ~BloomFilter();

    void add()
    {
    }

    void check();

private:
    std::vector<Binary> _bits;
};

}