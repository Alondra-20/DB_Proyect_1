/*
? Políticas de reemplazo

* Implemente `FIFOPolicy.cpp`, `LFUPolicy.cpp`, `LRUv2Policy.cpp` y `MRUPolicy.cpp`.
* Cada clase implementa la interfaz `ReplacementPolicy` proporcionada. 
* El gestor de búfer llama a `init` una vez, a `on_load` cuando una página pasa a estar 
* residente, a `on_access` ante un acierto y a `on_remove` antes de que una página abandone un marco.
* `pick_victim` recibe únicamente marcos candidatos que no están fijados (*unpinned*) y 
*  debe devolver uno de dichos marcos, o ningún valor si no es posible seleccionar ningún candidato.
---------------------------------------------------------------------------------------------------
? FIFO Policy
*Expulse al candidato que haya entrado en el grupo en primer lugar.
*Un acceso posterior no altera el orden FIFO.
*/



#include "FIFOPolicy.h"

#include <algorithm>

namespace bufman {
/** 
**Funcion init para cargar un marco en la política FIFO
* @param pool_size: el tamaño del grupo de marcos
*/
void FIFOPolicy::init(std::size_t pool_size) {
    queue_.clear();
    positions_.clear();
}


void FIFOPolicy::on_access(std::size_t frame) {
// //en blanco pq no lo necesito?
 }

/** 
**Funcion on_load para cargar un marco en la política FIFO
* @param frame: el marco que se está cargando
*/
void FIFOPolicy::on_load(std::size_t frame) {
//guardar en el orden que entran
    queue_.push_back(frame);
    positions_[frame] = std::prev(queue_.end());
}


/** 
**Funcion on_remove para eliminar un marco en la política FIFO
* @param frame: el marco que se está eliminando
*/
void FIFOPolicy::on_remove(std::size_t frame) {
    // Si el marco está presente en la cola, elimínalo y actualiza la posición
    if (positions_[frame] != queue_.end()) {
        queue_.erase(positions_[frame]);
        positions_[frame] = queue_.end();
    }
    
}

/** 
**Funcion pick_victim para seleccionar un marco víctima en la política FIFO
* @param candidates: el vector de marcos candidatos
* @return std::optional<std::size_t>: el índice del marco víctima, o std::nullopt si no hay candidatos
*/
std::optional<std::size_t> FIFOPolicy::pick_victim(
        const std::vector<std::size_t>& candidates) const {
    // Recorre la cola en orden de llegada y devuelve el primer candidato que se encuentre
    for (auto it = queue_.begin(); it != queue_.end(); ++it) {
        if (std::find(candidates.begin(), candidates.end(), *it) != candidates.end()) {
            return *it;
        }
    }

    return std::nullopt;
}

}
