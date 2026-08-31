#ifndef STACKALLOCATOR_H
#define STACKALLOCATOR_H

#include "Allocator.h"
#include <vector>

class StackAllocator : public Allocator {
protected:
    void* m_start_ptr = nullptr;
    std::size_t m_offset = 0;
    std::vector<std::size_t> m_markers;
    std::vector<std::size_t> m_checkpoints;
public:
    StackAllocator(const std::size_t totalSize);

    virtual ~StackAllocator();

    virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;

    virtual void Free(void* ptr) override;

    virtual void Init() override;

    virtual void Reset();
    std::size_t Push();
    void Pop(const std::size_t marker);
private:
    StackAllocator(StackAllocator &stackAllocator);

};

#endif /* STACKALLOCATOR_H */