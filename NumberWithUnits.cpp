#include <string>
#include <iostream>
#include <map>
#include <algorithm>

using namespace std;

#include "NumberWithUnits.hpp"

const float TOLERANCE = 0.001;

namespace ariel {
     // Ordered map that stores the string it's unit and number worth as an inner map
    static std::map<std::string, std::map<std::string, double>> unitsMap;
    

    // Constructor
    NumberWithUnits::NumberWithUnits(double numOfUnit, const string &unitType) {
        // Check if unit is in map, count returns 0 if unit does not exists
        if (unitsMap.count(unitType) == 0) {
            throw invalid_argument("There is no unit named " + unitType);
        }
        this->numOfUnit = numOfUnit;
        this->unitType = unitType;
    }

    // Private function that Inserts the given units to the map
    void insertUnitsToMap(const string &unit1, const string &unit2) {
        // Loop over the inner map
         for(auto &innerMap : unitsMap[unit1]) {
            // Continue if it is the same unit so it won't enter to the map
            if (unit2 == innerMap.first) {
                continue;
            }
            // Calculate the converted number
            double convertNumber = unitsMap[unit2][unit1] * innerMap.second;
            // Insert to map the convertedNumber and the 1\convertedNumber to the oppisote stored map
            unitsMap[unit2][innerMap.first] = convertNumber;
            unitsMap[innerMap.first][unit2] = 1/convertNumber;
        }
    } 

    // Function to read from a given file
    void NumberWithUnits::read_units(ifstream& units_file) {
        // If file has errors, throw execption of error in file
        if (units_file.fail() || units_file.bad()) {
            throw runtime_error("Error in file");
        }
        // Using clear function to clear all flags
        // Using seekg function to move the pointer to the beginning of the file 
        units_file.clear();
        units_file.seekg(0);

        string unit1;
        string unit2;
        string buffer;
        double number1 = 0;
        double number2 = 0;

        // Loop while not reached eof and insert from the file the numbers and strings
        // The buffer stores the garbage from the file (as = or spaces between)
        while (units_file >> number1 >> unit1 >> buffer >> number2 >> unit2) {
            // Insert to the map the received units with the matched numbers from the file
            unitsMap[unit1][unit2] = number2;
            unitsMap[unit2][unit1] = number1/number2;

            // Calling private function to add harder to get entrys to the map in both ways (like km -> cm and cm -> km)
            insertUnitsToMap(unit1, unit2);
            insertUnitsToMap(unit2, unit1);
        }
        // Using clear function to clear all flags
        // Using seekg function to move the pointer to the beginning of the file 
        units_file.clear();
        units_file.seekg(0);
    }

    // Private function to convert the the unit
    double NumberWithUnits::convertNumOfUnit(const NumberWithUnits &unit, const string &newUnitType) {
        // If it is the same unit, returns the number of the unit
        if (unit.unitType == newUnitType) {
            return unit.numOfUnit;
        }
        // Check if unit is in map, count returns 0 if unit does not exists
        if (unitsMap.count(newUnitType) == 0) {
            throw invalid_argument("There is no unit named " + newUnitType);
        }
        // If theres no entry to the units given combine (no 0 as number of unit exists), then throw exepction
        if (unitsMap[unit.unitType][newUnitType] == 0) {
            throw invalid_argument{"Units do not match - ["+unit.unitType+"] cannot be converted to ["+newUnitType+"]"};
        }
        // Return the mult of the numbers of the units
        return unit.numOfUnit * unitsMap[unit.unitType][newUnitType];
    }

    NumberWithUnits NumberWithUnits::operator+(const NumberWithUnits &unit) const{
        double newNumber = convertNumOfUnit(unit,this->unitType) + this->numOfUnit;
        return NumberWithUnits{newNumber, this->unitType};    
    }

    NumberWithUnits& NumberWithUnits::operator+=(const NumberWithUnits &unit) {
        *this = *this + unit;
        return *this;
    }

    NumberWithUnits NumberWithUnits::operator+() const { // Unary plus
        return NumberWithUnits{+this->numOfUnit, this->unitType}; 
    }

    NumberWithUnits NumberWithUnits::operator-(const NumberWithUnits &unit) const {
        double newNumber = this->numOfUnit - convertNumOfUnit(unit,this->unitType);
        return NumberWithUnits{newNumber, this->unitType};  
    }

    NumberWithUnits& NumberWithUnits::operator-=(const NumberWithUnits &unit) {
        *this = *this - unit;
        return *this;
    }

    NumberWithUnits NumberWithUnits::operator-() const { // Unary minus
        return NumberWithUnits{-this->numOfUnit, this->unitType}; 
    }

    // Check tolerance which means if double returns very small difference or not
    int NumberWithUnits::compareWithTolerance(double number1, double number2) {
        double diff = number1 - number2;
        if (diff > TOLERANCE) {
            return 1;
        }
        if (abs(diff) > TOLERANCE) {
            return -1;
        }
        return 0;
    }

    bool NumberWithUnits::operator>(const NumberWithUnits &unit) const {
        double newNumber = convertNumOfUnit(unit,this->unitType);
        return compareWithTolerance(this->numOfUnit, newNumber) > 0;  
    }

    bool NumberWithUnits::operator>=(const NumberWithUnits &unit) const {
        double newNumber = convertNumOfUnit(unit,this->unitType);
        return compareWithTolerance(this->numOfUnit, newNumber) >= 0;  
    }

    bool NumberWithUnits::operator<(const NumberWithUnits &unit) const {
        double newNumber = convertNumOfUnit(unit,this->unitType);
        return compareWithTolerance(this->numOfUnit, newNumber) < 0;  
    }

    bool NumberWithUnits::operator<=(const NumberWithUnits &unit) const {
        double newNumber = convertNumOfUnit(unit,this->unitType);
        return compareWithTolerance(this->numOfUnit, newNumber) <= 0;  
    }

    bool NumberWithUnits::operator==(const NumberWithUnits &unit) const {
        double newNumber = convertNumOfUnit(unit,this->unitType);
        return compareWithTolerance(this->numOfUnit, newNumber) == 0;  
    }

    bool NumberWithUnits::operator!=(const NumberWithUnits &unit) const {
        return !(*this == unit);
    }

    NumberWithUnits& NumberWithUnits::operator++() { // prefix (+)
        ++(this->numOfUnit);
        return *this;
    }

    NumberWithUnits NumberWithUnits::operator++(int flag) { // postfix (+)
        NumberWithUnits copy = *this;
        (this->numOfUnit)++;
        return copy;
    }

    NumberWithUnits& NumberWithUnits::operator--() { // prefix (-)
        --(this->numOfUnit);
        return *this;    
    }

    NumberWithUnits NumberWithUnits::operator--(int flag) { // postfix -+)
        NumberWithUnits copy = *this;
        (this->numOfUnit)--;
        return copy;
    }

    NumberWithUnits NumberWithUnits::operator*(double number) const {
        return NumberWithUnits{this->numOfUnit * number, this->unitType};
    }

    NumberWithUnits operator*(double number, const NumberWithUnits &unit) {
        return NumberWithUnits{number * unit.numOfUnit, unit.unitType};        
    }

    ostream& operator<<(ostream &os, const NumberWithUnits &unit) { // Output stream
        os << unit.numOfUnit << '[' << unit.unitType << ']';
        return os;
    }

    istream& operator>>(istream &is, NumberWithUnits &unit) { // Input stream
        // Temp number and unit to not effect on original ones
        double tempNumber = 0;
        string buffer;
        // Read from input stream to the number and to the buffer that stores the rest
        is >> tempNumber >> buffer;
        // Loop while buffer is not empty and not reached the ']' char
        while (!buffer.empty() && buffer.at(buffer.length()-1) != ']') {
            // String that holds the next string from the input stream
            string nextStr;
            is >> nextStr;
            // Adding it to the string buffer
            buffer += nextStr;
        }

        // Removes from the buffer the chars '[' and ']' to get only the unit
        buffer.erase(remove(buffer.begin(), buffer.end(), '['), buffer.end());
        buffer.erase(remove(buffer.begin(), buffer.end(), ']'), buffer.end());

        // Check if unit is in map, count returns 0 if unit does not exists
        if (unitsMap.count(buffer) == 0) {
            throw invalid_argument("There is no unit named " + unit.unitType);
        }
        // Put the temp unit and number (which they store only the number and only the unit) into the recived ones
        unit.unitType = buffer;
        unit.numOfUnit = tempNumber;
        return is;
    }  
}