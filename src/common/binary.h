
namespace wftz {

class Binary
{
public:
    Binary() : _data(""), _size(0) {}
    
    Binary(const char* data_) : _data(data_), _size(strlen(data_)) {}

    Binary(const char* data_, size_t size_) : _data(data_), _size(size_) {}

    const char* data() const { return _data; }

    size_t size() const { return _size; }

    char operator[](size_t n) const 
    {
        assert(n < size());
         return _data[n]; 
    }

    void clear()
    {
        _data = "";
        _size = 0;
    }

private:
    char* _data;
    size_t _size;
};

}