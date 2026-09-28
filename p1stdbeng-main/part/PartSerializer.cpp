/*

? Instrucciones de Part 

*Se proporciona el archivo Part.h, el cual no debe modificarse. 
*Este declara la estructura `Part` y la enumeración `PartColor`. 
*Una pieza (`Part`) consta de un identificador entero, 
*un nombre de hasta nueve caracteres más el carácter nulo de termi-
*nación,un peso en coma flotante, un valor de color, 
* un precio en coma flotante y un material de hasta nueve caracteres 
* más el carácter nulo de terminación.

*Una pieza válida tiene un `part_id` positivo, un `part_weight` 
*y un `part_price` no negativos, y un `part_color` dentro del rango
* inclusivo de 0 a 5. El valor cero para `part_id` está reservado como
* marcador de espacio libre.
---------------------------------------------------------------------
? Instrucciones de PartSerializer
*Implemente `PartSerializer.cpp`. La disposición en disco ocupa exactamente 36 bytes, 
*escritos campo a campo en el siguiente orden:

*  `part_id` (4 bytes), `part_name` (10 bytes), `part_weight` (4 bytes),
*  `part_color` (4 bytes), `part_price` (4 bytes), `part_material` (10 bytes)

*No escriba la estructura `Part` tal como reside en memoria mediante una única operación 
*de copia, ya que el relleno (*padding*) introducido por el compilador no forma parte del 
*formato en disco. Implemente la serialización y deserialización, la inicialización de bloques,
* la serialización y deserialización completas de bloques, así como las funciones auxiliares a nivel 
* de ranura (*slot*) declaradas en `PartSerializer.h`.

*Un bloque de 4096 bytes almacena 113 registros de piezas. Un bloque inicializado a cero se considera vacío.
*Los lectores de bloques se detienen en la primera ranura cuyo `part_id` sea cero; los escritores deben rechazar
*cualquier `Part` cuyo identificador sea cero.
*/


#include "PartSerializer.h"
#include <algorithm>
#include <cassert>
#include <cstring>

namespace bufman {


/** 
**Funcion serialize_part para serializar una pieza
* @param part: la pieza a serializar
* @param buffer: el buffer donde se almacenará la pieza serializada
* @param max_len: la longitud máxima del buffer
*/
//* Packs one Part into `kPartRecordSize` bytes, field by field. We copy each
//* field individually (rather than memcpy'ing the whole struct) because the
//* compiler is free to insert padding between struct members; only this
//* explicit, gap-free layout is guaranteed to match on disk.
void serialize_part(const Part& part, char* buffer, std::size_t max_len) {
    assert(buffer != nullptr);
    if (max_len < kPartRecordSize) {
        return;
    }

    std::size_t offset = 0;
    std::memcpy(buffer + offset, &part.part_id, sizeof(part.part_id));
    offset += sizeof(part.part_id);
    std::memcpy(buffer + offset, part.part_name, sizeof(part.part_name));
    offset += sizeof(part.part_name);
    std::memcpy(buffer + offset, &part.part_weight, sizeof(part.part_weight));
    offset += sizeof(part.part_weight);
    std::memcpy(buffer + offset, &part.part_color, sizeof(part.part_color));
    offset += sizeof(part.part_color);
    std::memcpy(buffer + offset, &part.part_price, sizeof(part.part_price));
    offset += sizeof(part.part_price);
    std::memcpy(buffer + offset, part.part_material, sizeof(part.part_material));
}
/** 
**Funcion deserialize_part para deserializar una pieza
* @param part: la pieza a deserializar
* @param buffer: el buffer donde se encuentra la pieza serializada
* @param max_len: la longitud máxima del buffer
*/
//* Mirror image of serialize: walks the same fields in the same order at the
//* same offsets, copying bytes out of the buffer and into a fresh Part.
bool deserialize_part(const char* buffer, std::size_t max_len, Part& part) {
    if (buffer == nullptr || max_len < kPartRecordSize) {
        return false;
    }

    part = Part{};
    std::size_t offset = 0;
    std::memcpy(&part.part_id, buffer + offset, sizeof(part.part_id));
    offset += sizeof(part.part_id);
    std::memcpy(part.part_name, buffer + offset, sizeof(part.part_name));
    offset += sizeof(part.part_name);
    std::memcpy(&part.part_weight, buffer + offset, sizeof(part.part_weight));
    offset += sizeof(part.part_weight);
    std::memcpy(&part.part_color, buffer + offset, sizeof(part.part_color));
    offset += sizeof(part.part_color);
    std::memcpy(&part.part_price, buffer + offset, sizeof(part.part_price));
    offset += sizeof(part.part_price);
    std::memcpy(part.part_material, buffer + offset, sizeof(part.part_material));
    return true;
}
/** 
**Funcion initialize_part para inicializar una pieza
* @param part: la pieza a inicializar
*/
//* Zero-fills a block so every unused record slot reads back as pid == 0,
//* which deserialize_block below treats as "end of data in this block".
void initialize_part_block(char* block) {
    assert(block != nullptr);
    std::memset(block, 0, kPartBlockSize);
}

/** 
**Funcion serialize_part_block para serializar un bloque de piezas
* @param parts: el vector de piezas a serializar
* @param block: el buffer donde se almacenarán las piezas serializadas
* @return std::size_t: el número de piezas serializadas
*/
//* Lays out up to kRecordsPerBlock records back to back: record i starts at
//* byte i * kRecordSize. Any leftover slots stay zeroed by initialize_block.
std::size_t serialize_part_block(const std::vector<Part>& parts, char* block) {
    assert(block != nullptr);
    initialize_part_block (block);
    const std::size_t count = std::min(parts.size(), kPartsPerBlock);
    for (std::size_t i = 0; i < count; ++i) {
        serialize_part (parts[i], block + i * kPartRecordSize, kPartRecordSize);
    }
    return count;
}
/** 
**Funcion deserialize_part_block para deserializar un bloque de piezas
* @param block: el buffer donde se encuentran las piezas serializadas
* @return std::vector<Part>: el vector de piezas deserializadas
*/
//* Reads records out of a block in the same fixed-slot order, stopping at the
//* first pid == 0 slot (see initialize_block) since that marks unused space.
std::vector<Part> deserialize_part_block(const char* block) {
    std::vector<Part> parts;
    if (block == nullptr) {
        return parts;
    }

    for (std::size_t i = 0; i < kPartsPerBlock; ++i) {
        Part part{};
        if (!deserialize_part(block + i * kPartRecordSize, kPartRecordSize, part) || part.part_id == 0) {
            break;
        }
        parts.push_back(part);
    }
    return parts;
}
/** 
**Funcion part_record_count para contar el número de registros de piezas en un bloque
* @param block: el buffer donde se encuentran las piezas serializadas
* @return std::size_t: el número de piezas en el bloque
*/
std::size_t part_record_count(const char* block) {
    if (block == nullptr) {
        return 0;
    }
    for (std::size_t i = 0; i < kPartsPerBlock; ++i) {
        Part part{};
        if (!deserialize_part(block + i * kPartRecordSize, kPartRecordSize, part) || part.part_id == 0) {
            return i;
        }
    }
    return kPartsPerBlock;
}
/** 
**Funcion first_free_part_slot para encontrar el primer slot libre en un bloque
* @param block: el buffer donde se encuentran las piezas serializadas
* @return std::optional<std::size_t>: el índice del primer slot libre, o std::nullopt si no hay slots libres
*/
std::optional<std::size_t> first_free_part_slot(const char* block) {
    if (block == nullptr) {
        return std::nullopt;
    }
    for (std::size_t i = 0; i < kPartsPerBlock; ++i) {
        Part part{};
        if (!deserialize_part(block + i * kPartRecordSize, kPartRecordSize, part) || part.part_id == 0) {
            return i;
        }
    }
    return std::nullopt;
}
/** 
**Funcion get_part_record para obtener un registro de pieza
* @param block: el buffer donde se encuentran las piezas serializadas
* @param slot: el índice del slot a obtener
* @param part: la pieza donde se almacenará el resultado
* @return bool: true si se obtuvo el registro, false en caso contrario
*/
bool get_part_record(const char* block, std::size_t slot, Part& part) {
    if (block == nullptr || slot >= kPartsPerBlock) {
        return false;
    }
    if (!deserialize_part(block + slot * kPartRecordSize, kPartRecordSize, part)) {
        return false;
    }
    return part.part_id != 0;
}
/** 
**Funcion put_part_record para insertar un registro de pieza
* @param block: el buffer donde se encuentran las piezas serializadas
* @param slot: el índice del slot a obtener
* @param part: la pieza donde se almacenará el resultado
* @return bool: true si se obtuvo el registro, false en caso contrario
*/
bool put_part_record(char* block, std::size_t slot, const Part& part) {
    if (block == nullptr || slot >= kPartsPerBlock || part.part_id == 0) {
        return false;
    }
    serialize_part(part, block + slot * kPartRecordSize, kPartRecordSize);
    return true;
}

}
