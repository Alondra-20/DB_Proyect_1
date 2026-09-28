/*
? Políticas de reemplazo

* Implemente `FIFOPolicy.cpp`, `LFUPolicy.cpp`, `LRUv2Policy.cpp` y `MRUPolicy.cpp`.
* Cada clase implementa la interfaz `ReplacementPolicy` proporcionada. 
* El gestor de búfer llama a `init` una vez, a `on_load` cuando una página pasa a estar 
* residente, a `on_access` ante un acierto y a `on_remove` antes de que una página abandone un marco.
* `pick_victim` recibe únicamente marcos candidatos que no están fijados (*unpinned*) y 
*  debe devolver uno de dichos marcos, o ningún valor si no es posible seleccionar ningún candidato.
---------------------------------------------------------------------------------------------------
? LFU Policy
*Desaloje el candidato con el menor número de accesos durante su permanencia. 
*En caso de empate en la frecuencia, desempate según el acceso menos reciente. 
*Una página recién cargada comienza con una frecuencia de uno, y los contadores 
*se reinician cuando se elimina una página.
*/

#include "LFUPolicy.h"

#include <algorithm>

namespace bufman {
/** 
**Funcion init para cargar un marco en la política LFU
* @param pool_size: el tamaño del grupo de marcos
*/
void LFUPolicy::init(std::size_t pool_size) {
    slots_.clear();
    buckets_.clear();
    min_freq_ = 0;
}
/** 
**Funcion on_access para cargar un marco en la política LFU
* @param frame: el marco que se está accediendo
*/
void LFUPolicy::on_access(std::size_t frame) {
    auto it = slots_.find(frame);

    if (it == slots_.end()) {
        return;
    }

    auto& slot = it->second;

    // Incrementa la frecuencia del marco y actualiza su posición en el bucket correspondiente
    std::size_t old_count = slot.count;
    std::size_t new_count = old_count + 1;

    auto bucket_it = buckets_.find(old_count);

    if (bucket_it != buckets_.end()) {
        bucket_it->second.erase(slot.pos);

        if (bucket_it->second.empty()) {
            buckets_.erase(bucket_it);

            if (min_freq_ == old_count) {
                min_freq_ = new_count;
            }
        }
    }

    buckets_[new_count].push_front(frame);

    slot.count = new_count;
    slot.pos = buckets_[new_count].begin();
}
/** 
**Funcion on_load para cargar un marco en la política LFU
* @param frame: el marco que se está cargando
*/
void LFUPolicy::on_load(std::size_t frame) {
    auto& slot = slots_[frame];

    slot.count = 1;
    // Agrega el marco al frente del bucket de frecuencia 1
    buckets_[1].push_front(frame);
    slot.pos = buckets_[1].begin();

    min_freq_ = 1;
}
/** 
**Funcion on_remove para cargar un marco en la política LFU
* @param frame: el marco que se está eliminando
*/
void LFUPolicy::on_remove(std::size_t frame) {
    auto it = slots_.find(frame);

    if (it == slots_.end()) {
        return;
    }

    std::size_t count = it->second.count;

    auto bucket_it = buckets_.find(count);

    // Elimina el marco del bucket correspondiente y actualiza la frecuencia mínima si es necesario
    if (bucket_it != buckets_.end()) {
        bucket_it->second.erase(it->second.pos);

        if (bucket_it->second.empty()) {
            buckets_.erase(bucket_it);
        }
    }

    slots_.erase(it);
// Actualiza la frecuencia mínima si es necesario
if (buckets_.empty()) {
    min_freq_ = 0;
} else {
    min_freq_ = static_cast<std::size_t>(-1);
    
    for (const auto& entry : buckets_) {
        if (entry.first < min_freq_) {
            min_freq_ = entry.first;
        }
    }

}

}
/** 
**Funcion pick_victim para seleccionar un marco víctima en la política LFU
* @param candidates: el vector de marcos candidatos
* @return std::optional<std::size_t>: el índice del marco víctima, o std::nullopt si no hay candidatos
*/
std::optional<std::size_t>LFUPolicy::pick_victim(
    const std::vector<std::size_t>& candidates) const {

    std::optional<std::size_t> victim;
    std::size_t lowest_frequency = 0;
        // Encuentra el marco con la frecuencia más baja entre los candidatos
    for (std::size_t frame : candidates) {
        auto slot_it = slots_.find(frame);

        if (slot_it == slots_.end()) {
            continue;
        }

        std::size_t frequency = slot_it->second.count;
        // Actualiza el marco víctima si es la primera vez o si tiene una frecuencia más baja
        if (!victim.has_value() || frequency < lowest_frequency) {
            victim = frame;
            lowest_frequency = frequency;
        }
    }
    // Si no se encontró ningún candidato válido, devuelve std::nullopt
    if (!victim.has_value()) {
        return std::nullopt;
    }
    // Busca el bucket correspondiente a la frecuencia más baja y recorre sus marcos desde el final (LRU) hasta el principio (MRU)
    auto bucket_it = buckets_.find(lowest_frequency);

    if (bucket_it == buckets_.end()) {
        return victim;
    }

    const auto& bucket = bucket_it->second;
    // Recorre el bucket desde el final (LRU) hasta el principio (MRU) para encontrar el marco víctima
    for (auto it = bucket.rbegin(); it != bucket.rend(); ++it) {
        if (std::find(candidates.begin(),candidates.end(),*it) != candidates.end()) {
            return *it;
        }
    }

    return victim;
}

}  