#include <string>
#include <vector>
#include <map>

typedef std::string Key;
typedef std::vector<Key> CSVHeader;
typedef std::map<Key, std::string> CSVRow;
typedef std::vector<CSVRow> CSVData;

CSVData readCSVFile(std::string fileName);
CSVHeader parseCSVHeader(std::string row);
CSVRow parseCSVRow(std::string rawRow, CSVHeader header);
