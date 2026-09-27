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
? Instrucciones de PartCsv
*Implementa PartCsv.cpp. Cada fila que no esté en blanco contiene exactamente 
*seis campos separados por comas en este orden:

*part_id, part_name, part_weight, part_color, part_price, part_material

*Analiza los valores enteros y de punto flotante de forma estricta:
*se debe procesar el campo de entrada en su totalidad y rechazar aquellos valores
*que sean inválidos, que provoquen desbordamiento o que estén fuera de rango. 
*Las filas inválidas no constituyen un error fatal; imprime un mensaje de diagnóstico 
*con el formato "line N: reason", incrementa el contador de filas omitidas y continúa 
*con las filas siguientes. Las filas válidas se devuelven en el orden en que aparecen en
*el archivo. Si no es posible abrir el archivo de entrada, se debe emitir un mensaje de 
*diagnóstico y no devolver ninguna pieza válida.

*/

#include "PartCsv.h"

#include <charconv>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace bufman {
namespace {

bool parse_integer(const std::string& text, int& value) {
    if (text.empty()) {
        return false;
    }
    const char* first = text.data();
    const char* last = first + text.size();
    const auto result = std::from_chars(first, last, value);
    return result.ec == std::errc{} && result.ptr == last;
}

bool parse_float(const std::string& text, float& value) {
    if (text.empty()) {
        return false;
    }
    const char* first = text.data();
    const char* last = first + text.size();
    const auto result = std::from_chars(first, last, value);
    return result.ec == std::errc{} && result.ptr == last;
}

bool parse_line(const std::string& line, Part& part, std::string& error) {
    std::stringstream input(line);
    std::string part_id_text;
    std::string part_name;
    std::string part_weight_text;
    std::string part_color_text;
    std::string part_price_text;
    std::string part_material;
    std::string extra;

    if (!std::getline(input, part_id_text, ',') ||
        !std::getline(input, part_name, ',') ||
        !std::getline(input, part_weight_text, ',') ||
        !std::getline(input, part_color_text, ',') ||
        !std::getline(input, part_price_text, ',') ||
        !std::getline(input, part_material, ',') ||
        std::getline(input, extra, ',')) {
        error = "expected exactly six comma-separated fields";
        return false;
    }

    int part_id = 0;
    float part_weight = 0.0;
    int part_color = 0;
    float part_price = 0.0;


    if (!parse_integer(part_id_text, part_id) || part_id <= 0) {
        error = "part_id must be a positive integer";
        return false;
    }
    if (part_name.size() >  9) {
        error = "part_name must contain at most 9 characters";
        return false;
    }
    if (!parse_float(part_weight_text, part_weight) || part_weight < 0) {
        error = "part_weight must be a nonnegative float";
        return false;
    }
    if (!parse_integer(part_color_text, part_color) || part_color < 0 || part_color > 5) {
        error = "part_color must be an integer between 0 and 5";
        return false;
    }
    if (!parse_float(part_price_text, part_price) || part_price < 0) {
        error = "part_price must be a nonnegative float";
        return false;
    }

    if (!parse_float(part_weight_text, part_weight) || part_weight < 0) {
        error = "part_weight must be a nonnegative float";
        return false;
    }

    if (part_material.size() >  9) {
        error = "part_material must contain at most 9 characters";
        return false;
    }

    part = Part{};
    part.part_id = part_id;
    part.part_weight = part_weight;
    part.part_color = part_color;
    part.part_price = part_price;

    // Copiar los strigns hastta el character null
    part_name.copy(part.part_name, part_name.size());
    part_material.copy(part.part_material, part_material.size());

    return true;
}

}

PartLoadResult load_parts(const std::string& path, std::ostream& diagnostics) {
    PartLoadResult result;
    std::ifstream input(path);
//!! si no se puede abrir el archivo, imprime un mensaje de error y devuelve un resultado vacío
    if (!input) {
        diagnostics << "cannot open CSV file: " << path << '\n';
         //result.skipped = 1;
        return result;
    }

    std::string line;
    std::size_t line_number = 0;
    while (std::getline(input, line)) {
        ++line_number;
//!! si encuentras una linea vacia, es una fila invalida, aumenta skip y empre error
        if (line.empty()) {
           // ++result.skipped;
           // diagnostics << "line " << line_number << ": blank line\n";
            continue;
        }
        Part part{};
        std::string error;
        if (!parse_line(line, part, error)) {
            ++result.skipped;
            diagnostics << "line " << line_number << ": " << error << '\n';
            continue;
        }
        result.parts.push_back(part);
    }
    return result;
}

}
