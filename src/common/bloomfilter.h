#pragma once

#include <cstdint>
#include <cmath>
#include <iostream>
#include <vector>
#include <string>

#include "binary.h"

namespace {
    int murmurhash64(std::string& key)
    {   
        const uint64_t m = 0xc6a4a7935bd1e995;
        const int r = 47; 
        int len = key.length();
        uint64_t h = 0 ^ (len * m); 
        const uint64_t *data = (const uint64_t *)key.c_str();
        const uint64_t *end = data + (len / 8); 

        while (data != end)
        {   
            uint64_t k = *data++;

            k *= m;
            k ^= k >> r;
            k *= m;

            h ^= k;
            h *= m;
        }        

        const u_char *data2 = (const u_char *)data;
        switch (len & 7)
        {   
            case 7: h ^= (uint64_t)(data2[6]) << 48; 
            case 6: h ^= (uint64_t)(data2[5]) << 40; 
            case 5: h ^= (uint64_t)(data2[4]) << 32; 
            case 4: h ^= (uint64_t)(data2[3]) << 24; 
            case 3: h ^= (uint64_t)(data2[2]) << 16; 
            case 2: h ^= (uint64_t)(data2[1]) << 8;
            case 1: h ^= (uint64_t)(data2[0]);
                h *= m;
        };   

        h ^= h >> r;
        h *= m;
        h ^= h >> r;
        int hash_id = h % 39601;
        return hash_id % 100;
    }
}

namespace wftz {

static unsigned char _bloomfilter_mask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

class BloomFilter
{
public:
    BloomFilter(const uint32_t n_, const double false_positive_) :
        _n(n_), _p(false_positive_), _m(0), _k(0), _bytes()
    {
        _k = static_cast<int32_t>(-std::log(_p)  / std::log(2));
        _m = static_cast<int32_t>(_k * n_ * 1.0 / std::log(2));
        _bytes = new char[(_m + 7) / 8];
    }

    virtual ~BloomFilter()
    {
        delete []_bytes;
        _bytes = nullptr;
    }

    void insert(std::string& key)
    {
        uint32_t bits_num = _m;

        uint64_t hash_1 = murmurhash64(key);
        uint32_t num = 0;
        uint64_t delta = (hash_1 >> 17) | (hash_1 << 15);
        for (uint32_t i = 0; i < _k; ++i)
        {
            num = hash_1 % bits_num;
            _bytes[num >> 3] |= _bloomfilter_mask[num & 7];
            hash_1 += delta;
        }
    }

    bool check(std::string& key)
    {
        uint32_t bits_num = _m;

        uint64_t hash_1 = murmurhash64(key);
        uint32_t num = 0;
        uint64_t delta = (hash_1 >> 17) | (hash_1 << 15);

        for (uint32_t i = 0; i < _k; ++i)
        {
            num = hash_1 % bits_num;
            if (!(_bytes[num >> 3] & _bloomfilter_mask[num & 7]))
            {
                return false;
            }
            hash_1 += delta;
        }
        return true;
    }

private:
    char* _bytes;
    double _p;
    uint32_t _n;
    uint32_t _m;
    uint32_t _k;
};

}