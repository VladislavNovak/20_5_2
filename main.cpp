#include <windows.h>
#include <iostream>
#include <ctime>
#include <cstdlib> // rand, srand
#include <string>
#include <vector>
#include <algorithm> // find_if
#include <sstream> // stringstream

using std::string;
using std::vector;

string getTrimmedString(std::string str, std::string const &whiteSpaces = " \r\n\t\v\f") {
    auto start = str.find_first_not_of(whiteSpaces);
    str.erase(0, start);
    auto end = str.find_last_not_of(whiteSpaces);
    str.erase(end + 1);

    return str;
}

int getRandomIntFromRange(int from, int to) {
    return (from + std::rand() % (to - from + 1)); // NOLINT(cert-msc50-cpp)
}

// Разбивает строку на подстроки и возвращает вектор
vector<string> getSplitStringOnRecords(string const &str, const char delim = ',', bool isEmptyDenied = true) {
    vector<string> records;
    std::stringstream ss(str);

    string rawRecord;
    // Делим строки на токены по delim
    while (std::getline(ss, rawRecord, delim)) {
        string record = getTrimmedString(rawRecord);
        // Не позволяет добавлять пустой элемент
        if (record.empty() && isEmptyDenied) continue;

        records.push_back(record);
    }

    return records;
}

std::string getUserString(const string &propose) {
    while (true) {
        string userInput;
        printf("%s: ", propose.c_str());
        std::getline(std::cin, userInput);

        userInput = getTrimmedString(userInput);
        if (userInput.empty()) {
            std::cout << "Строка не может быть пустой. Попробуйте снова!" << std::endl;
            continue;
        }

        return getSplitStringOnRecords(userInput, ' ')[0];
    }
}

bool isNumeric(std::string const &str) {
    auto it = std::find_if(
            str.begin(),
            str.end(),
            [](char const &c) { return !std::isdigit(c); });

    return !str.empty() && it == str.end();
}

// Если from != to, тогда ввести цифры возможно лишь в диапазоне от from до to
int getUserNumeric(const string &msg = "Введите цифры", int from = 0, int to = 0) {
    string warning = "Попробуйте снова. Это должно быть целое число";
    bool isRange = (from != to);

    while (true) {
        string userInput = getUserString(msg);

        if (!isNumeric(userInput)) {
            printf("%s\n", warning.c_str());
            continue;
        }

        int num = std::stoi(userInput);

        if (isRange && (num < from || num > to)) {
            printf("%s в диапазоне (%i - %i)\n", warning.c_str(), from, to);
            continue;
        }

        return num;
    }
}

vector<string> getRecords(int numberOfRecords) {
    vector<string> records;

    for (int i = 0; i < numberOfRecords; ++i) {
        records.push_back(std::to_string(getRandomIntFromRange(0, 1)));
    }

    return records;
}

void setComplexData(vector<int> const &sizes, vector<vector<string>> &complexData) {
    const int LINES_COUNT = sizes[0];
    const int CELLS_COUNT = sizes[1];

    for (int i = 0; i < LINES_COUNT; ++i) {
        complexData.push_back(getRecords(CELLS_COUNT));
    }
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    std::srand(static_cast<unsigned int>(std::time(nullptr))); // NOLINT(cert-msc51-cpp)

    vector<vector<string>> picture;

    setComplexData({ getUserNumeric("Высота картины"), getUserNumeric("Ширина картины") }, picture);

    for (auto const &line : picture) {
        for (auto const &cell : line) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }

}