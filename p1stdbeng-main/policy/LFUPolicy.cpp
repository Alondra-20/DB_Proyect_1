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
    auto& slot = slots_[frame];
    std::size_t old_count = slot.count;
    std::size_t new_count = old_count + 1;

    // Elimina el marco del bucket de frecuencia anterior
    auto old_bucket_it = buckets_.find(old_count);

    if (old_bucket_it != buckets_.end()) {
        old_bucket_it->second.erase(slot.pos);
        
        if (old_bucket_it->second.empty()) {
            buckets_.erase(old_bucket_it);

            if (min_freq_ == old_count) {
                min_freq_ = new_count;
            }
        }
    }
    // Agrega el marco al frente del bucket de frecuencia nueva
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

    slot.count = 0;
    // Agrega el marco al frente del bucket de frecuencia 0
    buckets_[0].push_front(frame);
    slot.pos = buckets_[0].begin();

    min_freq_ = 0;
}
/** 
**Funcion on_remove para cargar un marco en la política LFU
* @param frame: el marco que se está eliminando
*/
void LFUPolicy::on_remove(std::size_t frame) {
    auto it = slots_.find(frame);
    if (it != slots_.end()) {
 
    std::size_t count = it->second.count;
    auto bucket_it = buckets_.find(count);

       // Elimina el marco del bucket correspondiente y actualiza la frecuencia mínima si es necesario
        if (bucket_it != buckets_.end()) {
            bucket_it->second.erase(it->second.pos);

            if (bucket_it->second.empty()) {
                buckets_.erase(bucket_it);

                if (min_freq_ == count) {
                    min_freq_ = 0;

                    // Encuentra la nueva frecuencia mínima,recorriendo los buckets restantes
                    for (const auto& [freq, bucket] : buckets_) {
                        if (min_freq_ == 0 || freq < min_freq_) {
                            min_freq_ = freq;
                        }
                    }
                }
            }
        }

        slots_.erase(it);
    }
}
/** 
**Funcion pick_victim para seleccionar un marco víctima en la política LFU
* @param candidates: el vector de marcos candidatos
* @return std::optional<std::size_t>: el índice del marco víctima, o std::nullopt si no hay candidatos
*/
std::optional<std::size_t>LFUPolicy::pick_victim(
    const std::vector<std::size_t>& candidates) const {

    if (candidates.empty())
        return std::nullopt;

    auto it = buckets_.find(min_freq_);

    if (it != buckets_.end()) {
        const auto& bucket = it->second;

        for (auto rit = bucket.rbegin();rit != bucket.rend(); ++rit) {

            if (std::find(candidates.begin(), candidates.end(),*rit) != candidates.end()) {
                return *rit;
            }
        }
    }

    return std::nullopt;
}
}