#include "FIFOPolicy.h"

#include <algorithm>

namespace bufman {

void FIFOPolicy::init(std::size_t pool_size) {
    queue_.clear();
    positions_.clear();
}


void FIFOPolicy::on_access(std::size_t frame) {
//en blanco pq no lo necesito?
}

void FIFOPolicy::on_load(std::size_t frame) {
//guardar en el orden que entran
    queue_.push_back(frame);
    positions_[frame] = std::prev(queue_.end());
}

void FIFOPolicy::on_remove(std::size_t frame) {
    if (positions_[frame] != queue_.end()) {
        queue_.erase(positions_[frame]);
        positions_[frame] = queue_.end();
    }
    
}


}
