/*
? Políticas de reemplazo

* Implemente `FIFOPolicy.cpp`, `LFUPolicy.cpp`, `LRUv2Policy.cpp` y `MRUPolicy.cpp`.
* Cada clase implementa la interfaz `ReplacementPolicy` proporcionada. 
* El gestor de búfer llama a `init` una vez, a `on_load` cuando una página pasa a estar 
* residente, a `on_access` ante un acierto y a `on_remove` antes de que una página abandone un marco.
* `pick_victim` recibe únicamente marcos candidatos que no están fijados (*unpinned*) y 
*  debe devolver uno de dichos marcos, o ningún valor si no es posible seleccionar ningún candidato.
---------------------------------------------------------------------------------------------------
? MRU Policy
*Desaloje el candidato utilizado más recientemente. 
*Su orden de seguimiento es el mismo que el de LRU, 
*pero la selección de la víctima se realiza desde el extremo opuesto.
*/


#include "MRUPolicy.h"

#include <algorithm>

namespace bufman {
/** 
**Funcion init para cargar un marco en la política MRU
* @param pool_size: el tamaño del grupo de marcos
*/
void MRUPolicy::init(std::size_t pool_size) {
    order_.clear();
    positions_.assign(pool_size, order_.end());
}
/** 
**Funcion touch para actualizar el orden de los marcos en la política MRU
* @param frame: el marco que se está tocando
*/
void MRUPolicy::touch(std::size_t frame) {
    if (positions_[frame] != order_.end()) {
        order_.erase(positions_[frame]);
    }
    order_.push_front(frame);
    positions_[frame] = order_.begin();
}
/** 
**Funcion on_access para cargar un marco en la política MRU
* @param frame: el marco que se está accediendo
*/
void MRUPolicy::on_access(std::size_t frame) {
    touch(frame);
}
/** 
**Funcion on_load para cargar un marco en la política MRU
* @param frame: el marco que se está cargando
*/
void MRUPolicy::on_load(std::size_t frame) {
    touch(frame);
}
/** 
**Funcion on_remove para cargar un marco en la política MRU
* @param frame: el marco que se está eliminando
*/
void MRUPolicy::on_remove(std::size_t frame) {
    if (positions_[frame] != order_.end()) {
        order_.erase(positions_[frame]);
        positions_[frame] = order_.end();
    }
}
/** 
**Funcion pick_victim para seleccionar una víctima en la política MRU
* @param candidates: los marcos candidatos a ser desalojados
* @return std::optional<std::size_t>: el marco víctima seleccionado, o std::nullopt si no hay candidatos válidos
*/
std::optional<std::size_t> MRUPolicy::pick_victim(
        const std::vector<std::size_t>& candidates) const {
    for (auto it = order_.begin(); it != order_.end(); ++it) {
        if (std::find(candidates.begin(), candidates.end(), *it) != candidates.end()) {
            return *it;
        }
    }
    return std::nullopt;
}

}

