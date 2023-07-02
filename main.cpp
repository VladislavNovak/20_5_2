#include <windows.h>
#include <iostream>
#include <ctime>
#include <cstdlib> // rand, srand
#include <string>
#include <vector>
#include <algorithm> // find_if
#include <sstream> // stringstream
#include <fstream>

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

void outputComplexData(std::ostream &out, vector<vector<string>> const &complexData, const string &format = " ") {
    for (auto &line : complexData) {
        for (int i = 0; i < line.size(); ++i) {
             out << line[i] << (i != line.size() - 1 ? format : "");
        }
        out << std::endl;
    }
}

void writeComplexDataToFile(const char *path,
                            vector<vector<string>> const &complexData,
                            bool isAppendMode = true,
                            const string &format = ",") {
    std::ofstream file(path, (isAppendMode ? std::ios::app : std::ios::out));

    // Печатаем в поток file
    outputComplexData(file, complexData, format);

    file.close();
}

bool readFileToVector(const char* pathName, vector<string> &data) {
    bool isFileReadSuccessfully = false;
    std::ifstream in(pathName);

    if (in.is_open() && !in.bad()) {
        string textLine;

        while (std::getline(in, textLine)) {
            data.push_back(textLine);
        }

        in.clear();
        in.seekg(0, std::ios_base::beg);
        isFileReadSuccessfully = true;
    }

    in.close();

    return isFileReadSuccessfully;
}

bool readFileToComplexData(const char* pathName, vector<vector<string>> &complexData, const char &delim = ',') {
    vector<string> lines;
    bool isFileReadSuccessfully = readFileToVector(pathName, lines);

    if (isFileReadSuccessfully && !lines.empty()) {
        for (const auto &line : lines) {
            complexData.push_back(getSplitStringOnRecords(line, delim));
        }
    }

    return isFileReadSuccessfully;
}

void printWarning(const char* currentPath) {
    printf("%s не обнаружен.\n"
           "Он должен находиться в директории с исполняемым файлом!\n", currentPath);
}

void displayComplexDataToScreen(vector<vector<string>> const &complexData, const string &format = " ") {
     system("cls");
     std::cout << "---------Читаем данные из файла-------------" << std::endl;
    // Печатаем в поток std::cout
    outputComplexData(std::cout, complexData, format);
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    std::srand(static_cast<unsigned int>(std::time(nullptr))); // NOLINT(cert-msc51-cpp)

    const char* path = R"(..\test.txt)";
    // Данные для сохранения в файл
    vector<vector<string>> picture;
    // Данные, извлечённые из файла
    vector<vector<string>> pictureFromFile;

    // Пользователь задает размеры картины (матрица), затем каждый элемент получаем случайной генерацией цифр
    setComplexData({ getUserNumeric("Высота картины"), getUserNumeric("Ширина картины") }, picture);
    // Записываем картину (матрица) в файл, который каждый раз создается заново (isAppendMode = false)
    writeComplexDataToFile(path, picture, false);

    // Извлекаем данные из файла
    bool isFileReadSuccessfully = readFileToComplexData(path, pictureFromFile);
    // Проверяем удачное ли было прочтение из файла
    if (!isFileReadSuccessfully) {
        printWarning(path);
        return 1;
    }

    if (pictureFromFile.empty()) {
        std::cout << "Проверьте файлы на корректность данных" << std::endl;
        return 1;
    }

    // Распечатываем результат на экран
    displayComplexDataToScreen(pictureFromFile, "  ");
}