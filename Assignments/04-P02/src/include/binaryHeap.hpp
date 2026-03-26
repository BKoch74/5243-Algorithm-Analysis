#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

class BinaryHeap {
private:
    std::vector<int> data;

    int parent(int i) const {
        return (i - 1) / 2;
    }

    int left(int i) const {
        return 2 * i + 1;
    }

    int right(int i) const {
        return 2 * i + 2;
    }

    void siftUp(int i) {
        while (i > 0 && data[i] < data[parent(i)]) {
            std::swap(data[i], data[parent(i)]);
            i = parent(i);
        }
    }

    void siftDown(int i) {
        int n = static_cast<int>(data.size());

        while (true) {
            int l = left(i);
            int r = right(i);
            int smallest = i;

            if (l < n && data[l] < data[smallest]) {
                smallest = l;
            }

            if (r < n && data[r] < data[smallest]) {
                smallest = r;
            }

            if (smallest == i) {
                break;
            }

            std::swap(data[i], data[smallest]);
            i = smallest;
        }
    }

public:
    BinaryHeap() = default;

    explicit BinaryHeap(const std::vector<int> &values) : data(values) {
        buildHeap();
    }

    bool empty() const {
        return data.empty();
    }

    std::size_t size() const {
        return data.size();
    }

    int top() const {
        if (data.empty()) {
            throw std::runtime_error("heap is empty");
        }
        return data[0];
    }

    void push(int value) {
        data.push_back(value);
        siftUp(static_cast<int>(data.size()) - 1);
    }

    bool pop() {
        if (data.empty()) {
            return false;
        }

        if (data.size() == 1) {
            data.pop_back();
            return true;
        }

        data[0] = data.back();
        data.pop_back();
        siftDown(0);
        return true;
    }

    void buildHeap() {
        for (int i = static_cast<int>(data.size()) / 2 - 1; i >= 0; --i) {
            siftDown(i);
        }
    }
};