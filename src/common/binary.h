
namespace wftz {

class Binary
{
public:
    Binary() : _data(""), _size(0) {}

    Binary(size_t size_) : _size(size_)
    {
        _data = new char[size_];
    }

    ~Binary() { delete []_data; _data = nullptr; }
    
    char* data() const { return _data; }

    size_t size() const { return _size; }

    size_t resize(size_t new_size, char val = char())
    {
        if (size() > new_size)
        {
            _size = new_size;
        }
        else
        {
            char* new_ptr = new char[new_size];
            for (size_t i = 0; i < size(); i++)
            {
                new_ptr[i] = _data[i];
            }
            memset(new_ptr + size(), val, sizeof(char) * (new_size - size()));
            delete []_data;
            _data = new_ptr;
            _size = new_size;
        }
    }

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

    Binary(char* data_) = delete;

    Binary(char* data_, size_t size_) = delete;

    char* _data;
    size_t _size;
};

}