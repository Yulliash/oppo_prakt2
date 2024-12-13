#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <cctype>
using namespace std;

struct pokazania {
    string resyrs; // Тип ресурса
    string date;   // Дата
    double znach = 0.0; // Значение
};

// Удаление лишних пробелов
void trim(string& s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) { return !isspace(ch); }));
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !isspace(ch); }).base(), s.end());
}

// Функция для проверки строки (дополнительная валидация)
bool validateLine(const string& resyrs, const string& date, double znach) {
    regex date_regex(R"(\d{4}\.\d{2}\.\d{2})"); // Формат YYYY.MM.DD
    if (!regex_match(date, date_regex)) {
        cerr << "Ошибка: Некорректная дата: " << date << "\n";
        return false;
    }
    if (resyrs.empty()) {
        cerr << "Ошибка: Тип ресурса не указан.\n";
        return false;
    }
    if (znach < 0) {
        cerr << "Ошибка: Отрицательное значение: " << znach << "\n";
        return false;
    }
    return true;
}

// Функция для чтения данных из файла
vector<pokazania> readDataFromFile(const string& filename) {
    ifstream ist(filename);
    if (!ist) {
        cerr << "Ошибка: Не удалось открыть файл: " << filename << "\n";
        return {};
    }

    vector<pokazania> data;
    string line;

    // Регулярные выражения для разных форматов
    regex format1(R"((\d{4}\.\d{2}\.\d{2})\s+([\d.]+)\s+(.+))"); // Дата Значение "Ресурс"
    regex format2(R"((.+)\s+(\d{4}\.\d{2}\.\d{2})\s+([\d.]+))"); // "Ресурс" Дата Значение
    regex format3(R"(([\d.]+)\s+(.+)\s+(\d{4}\.\d{2}\.\d{2}))"); // Значение "Ресурс" Дата

    while (getline(ist, line)) {
        trim(line); // Убираем пробелы
        smatch match;
        pokazania obj;

        // Проверка строки на соответствие каждому формату
        if (regex_match(line, match, format1)) {
            obj.date = match[1];
            obj.znach = stod(match[2]);
            obj.resyrs = match[3];
        }
        else if (regex_match(line, match, format2)) {
            obj.resyrs = match[1];
            obj.date = match[2];
            obj.znach = stod(match[3]);
        }
        else if (regex_match(line, match, format3)) {
            obj.znach = stod(match[1]);
            obj.resyrs = match[2];
            obj.date = match[3];
        }
        else {
            cerr << "Ошибка: Строка не соответствует ни одному формату: " << line << "\n";
            continue;
        }

        // Проверка корректности данных
        if (validateLine(obj.resyrs, obj.date, obj.znach)) {
            data.push_back(obj);
        }
    }

    return data;
}

// Функция для вывода данных
void printData(const vector<pokazania>& data) {
    if (data.empty()) {
        cout << "Данные отсутствуют или некорректны.\n";
        return;
    }

    for (const auto& entry : data) {
        cout << "Показания счётчика:\n";
        cout << "Тип ресурса: " << entry.resyrs << "\n";
        cout << "Дата: " << entry.date << "\n";
        cout << "Значение: " << entry.znach << "\n\n";
    }
}





// Точка входа в программу
int main() {
    setlocale(0, ""); // Для поддержки русского языка в консоли

    string filename = "1.txt"; // Имя файла
    vector<pokazania> data = readDataFromFile(filename);
    printData(data);

    return 0;
}
