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

// ? IM NOT SURE IF WE HACE TO DELETE  THIS and change it to materials hacer algo v=basic and lo de color should i  do  for colors?

//  constexpr std::array<const char*, 8> kCityCodes = {
//     "PR", "NY", "LA", "TX", "FL", "CA", "WA", "OH"
// };

constexpr std::array<const char*, 6> kMaterials = {
    "Steel","Iron","Wood","Glass","Copper","Plastic"
};

}

std::vector<Part> generate(std::size_t count, int first_pid) {
    std::vector<Part> parts;
    parts.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        Part part{};
        part.part_id = first_pid + static_cast<int>(i);
        
        
        //!! check this
       // part.part_name[0] = '\0'; 
        part.part_weight = 1.0 + (part.part_id % 100);
        part.part_color = part.part_id % 6;  
        part.part_price = 10.0 + (part.part_id % 100);
       // part.part_material[0] = '\0';
        
        const std::string name = "P" + std::to_string(part.part_id);
        name.copy(part.part_name, std::min(name.size(), sizeof(part.part_name) - 1));

        const char* material = kMaterials[i % kMaterials.size()];
        std::memcpy(part.part_material, material, std::min(strlen(material), sizeof(part.part_material) - 1));
        part.part_material[sizeof(part.part_material) - 1] = '\0';
        
        // const char* city = kCityCodes[i % kCityCodes.size()];
        // std::memcpy(part.part_material, city, 2);

        parts.push_back(part);
    }
    return parts;
}

}