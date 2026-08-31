#include "StackAllocator.h"
#include "Utils.h"  /* CalculatePadding */
#include <stdlib.h>     /* malloc, free */
#include <algorithm>    /* max */
#include <cassert>
#ifdef _DEBUG
#include <iostream>
#endif

StackAllocator::StackAllocator(const std::size_t totalSize)
: Allocator(totalSize) {

}

void StackAllocator::Init() {
    if (m_start_ptr != nullptr) {
        free(m_start_ptr);
        m_start_ptr = nullptr;
    }
    m_start_ptr = malloc(m_totalSize);
    Reset();
}

StackAllocator::~StackAllocator() {
    free(m_start_ptr);
    m_start_ptr = nullptr;
}

void* StackAllocator::Allocate(const std::size_t size, const std::size_t alignment) {
    const std::size_t currentAddress = (std::size_t)m_start_ptr + m_offset;

    std::size_t padding = 0;
    if (alignment != 0) {
        padding = Utils::CalculatePadding(currentAddress, alignment);
    }

    if (m_offset + padding + size > m_totalSize) {
        return nullptr;
    }

    m_markers.push_back(m_offset);
    const std::size_t nextAddress = currentAddress + padding;
    m_offset += padding + size;

#ifdef _DEBUG
    std::cout << "A" << "\t@C " << (void*) currentAddress << "\t@R " << (void*) nextAddress << "\tO " << m_offset << "\tP " << padding << std::endl;
#endif
    m_used = m_offset;
    m_peak = std::max(m_peak, m_used);

    return (void*) nextAddress;
}

void StackAllocator::Free(void *ptr) {
    (void)ptr;
    assert(!m_markers.empty() && "StackAllocator::Free must follow LIFO order");

    m_offset = m_markers.back();
    m_markers.pop_back();
    m_used = m_offset;

#ifdef _DEBUG
    std::cout << "F" << "\t@F " << ptr << "\tO " << m_offset << std::endl;
#endif
}

void StackAllocator::Reset() {
    m_offset = 0;
    m_used = 0;
    m_peak = 0;
    m_markers.clear();
    m_checkpoints.clear();
}

std::size_t StackAllocator::Push() {
    m_checkpoints.push_back(m_offset);
    return m_offset;
}

void StackAllocator::Pop(const std::size_t marker) {
    assert(marker <= m_offset && "StackAllocator::Pop marker must be <= current offset");

    while (!m_checkpoints.empty() && m_checkpoints.back() >= marker) {
        m_checkpoints.pop_back();
    }

    m_offset = marker;
    m_used = m_offset;
}