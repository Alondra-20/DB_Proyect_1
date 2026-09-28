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
----------------------------------------------------------------------
? Instrucciones de PartGenerator
*Implementa PartGenerator.cpp. El generador devuelve el número solicitado
*de registros sintéticos válidos de tipo `Part`. Los identificadores deben
*ser secuenciales, comenzando por el primer identificador proporcionado.
*Selecciona nombres, pesos, colores, precios y materiales de manera determinista,
*de modo que los mismos argumentos produzcan siempre los mismos registros. 
*Asegúrate de que todo el texto generado se ajuste a los tamaños de campo fijos
*y de que todos los valores generados sean válidos.

*/
#include "PartGenerator.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <string>

namespace bufman {
namespace {

// ? Pregunta para el profe si hay que hacer un constepr para colores


// Se le asigna valores de materiales de manera cíclica,aseguranto que tenga valor valido y distinto
constexpr std::array<const char*, 6> kMaterials = {
    "Steel","Iron","Wood","Glass","Copper","Plastic"
};

}
/** 
**Funcion generate_parts para generar piezas
* @param count: el número de piezas a generar
* @param first_pid: el primer identificador 
* @return std::vector<Part>: el vector de "generate_parts"
*/
std::vector<Part> generate_parts(std::size_t count, int first_pid) {
    std::vector<Part> parts;
    parts.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        // Genera un identificador secuencial y valores deterministas para los demás campos
        Part part{};
        part.part_id = first_pid + static_cast<int>(i);
        part.part_weight = 1.0 + (part.part_id % 100);
        part.part_color = part.part_id % 6;  
        part.part_price = 10.0 + (part.part_id % 100);
       
        // Genera un nombre basado en el identificador
        const std::string name = "P" + std::to_string(part.part_id);
        name.copy(part.part_name, std::min(name.size(), sizeof(part.part_name) - 1));

        //!! check this
        // Genera un material basado en el identificador, asegurando que sea válido y distinto
        const char* material = kMaterials[i % kMaterials.size()];
        std::memcpy(part.part_material, material, std::min(strlen(material), sizeof(part.part_material) - 1));
        part.part_material[sizeof(part.part_material) - 1] = '\0';
        
       

        parts.push_back(part);
    }
    return parts;
}

}