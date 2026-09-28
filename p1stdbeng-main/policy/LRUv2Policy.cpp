/*
? Políticas de reemplazo

* Implemente `FIFOPolicy.cpp`, `LFUPolicy.cpp`, `LRUv2Policy.cpp` y `MRUPolicy.cpp`.
* Cada clase implementa la interfaz `ReplacementPolicy` proporcionada. 
* El gestor de búfer llama a `init` una vez, a `on_load` cuando una página pasa a estar 
* residente, a `on_access` ante un acierto y a `on_remove` antes de que una página abandone un marco.
* `pick_victim` recibe únicamente marcos candidatos que no están fijados (*unpinned*) y 
*  debe devolver uno de dichos marcos, o ningún valor si no es posible seleccionar ningún candidato.
---------------------------------------------------------------------------------------------------
? LRUv2 Policy
*Desaloje el candidato utilizado hace más tiempo. 
*Mantenga el elemento utilizado más recientemente 
*en un extremo del orden y el utilizado hace más tiempo en el otro.
*Utilice el diseño de lista y mapa hash sugerido en la cabecera.
*/


#include "LRUv2Policy.h"

#include <algorithm>

namespace bufman {
/** 
**Funcion init para cargar un marco en la política LRUv2
* @param pool_size: el tamaño del grupo de marcos
*/
void LRUv2Policy::init(std::size_t pool_size) {
    order_.clear();
    nodes_.clear();
}

/** 
**Funcion on_access cagar un marco en la política LRUv2
* @param frame: el marco que se está accediendo
*/
void LRUv2Policy::on_access(std::size_t frame) {
    order_.erase(nodes_[frame]);
    order_.push_front(frame);
    nodes_[frame] = order_.begin();
}
/** 
**Funcion on_load cagar un marco en la política LRUv2
* @param frame: el marco que se está cargando
*/
void LRUv2Policy::on_load(std::size_t frame) {
    order_.push_front(frame);
    nodes_[frame] = order_.begin();
}
/** 
**Funcion on_remove cagar un marco en la política LRUv2
* @param frame: el marco que se está eliminando
*/
void LRUv2Policy::on_remove(std::size_t frame) {
    order_.erase(nodes_[frame]);
    nodes_.erase(frame);  
}
/** 
**Funcion pick_victim para seleccionar una víctima en la política LRUv2
* @param candidates: los marcos candidatos a ser desalojados
* @return std::optional<std::size_t>: el marco víctima seleccionado, o std::nullopt si no hay candidatos válidos
*/

std::optional<std::size_t> LRUv2Policy::pick_victim(
        const std::vector<std::size_t>& candidates) const {
    // Recorre la lista de marcos en orden de uso, desde el más reciente hasta el menos reciente
    for (auto it = order_.rbegin(); it != order_.rend(); ++it) {
        if (std::find(candidates.begin(), candidates.end(), *it) != candidates.end()) {
            return *it;
        }
    }
    return std::nullopt;
}

}
