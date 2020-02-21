#include "csv.hpp"

#include <fstream>
#include <sstream>

const char CSV_DELIMITER = ',';

CSVData readCSVFile(std::string fileName)
{
    std::ifstream fin(fileName);
    std::string row;

    std::getline(fin, row);
    CSVHeader header = parseCSVHeader(row);

    CSVData data;
    while (std::getline(fin, row))
    {
        data.push_back(parseCSVRow(row, header));
    }

    return data;
}

CSVHeader parseCSVHeader(std::string row)
{
    CSVHeader header;
    std::string cell;
    std::stringstream rowStream(row);

    while (std::getline(rowStream, cell, CSV_DELIMITER))
    {
        header.push_back(cell);
    }

    return header;
}

CSVRow parseCSVRow(std::string rawRow, CSVHeader header)
{
    CSVRow row;
    std::string cell;
    std::stringstream rowStream(rawRow);
    size_t index = 0;

    while (std::getline(rowStream, cell, CSV_DELIMITER) && index < header.size())
    {
        row[header[index++]] = cell;
    }

    return row;
}
