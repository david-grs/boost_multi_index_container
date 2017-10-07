#pragma once

struct malloc_counter
{
    std::size_t malloc_calls() const { return _malloc_calls; }
    std::size_t free_calls() const { return _free_calls; }
    std::size_t realloc_calls() const { return _realloc_calls; }
    std::size_t malloc_bytes() const { return _malloc_bytes; }
    std::size_t realloc_bytes() const { return _realloc_bytes; }

    void pre_malloc(size_t /*size*/) {}
    void post_malloc(size_t size, const void* mem)
    {
        ++_malloc_calls;
        _malloc_bytes += size;
    }

    void pre_free(const void* /*mem*/) {}
    void post_free(const void* mem)
    {
        ++_free_calls;
    }

    void pre_realloc(const void* /*mem*/, size_t /*size*/) {}
    void post_realloc(const void* mem, size_t size, const void* new_mem)
    {
        ++_realloc_calls;
        _realloc_bytes += size;
    }

private:
    std::size_t _malloc_calls = {};
    std::size_t _free_calls = {};
    std::size_t _realloc_calls = {};
    std::size_t _malloc_bytes = {};
    std::size_t _realloc_bytes = {};
};
