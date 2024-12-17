#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <algorithm>
#include <sstream>
#include <numeric> // Для accumulate
using namespace std;

struct pokazania {
    string resyrs;   // Тип ресурса
    string date;     // Дата
    double znach = 0.0; // Значение
};

// Удаление лишних пробелов
string trim(string s) {
    s.erase(0, s.find_first_not_of(" \t"));
    s.erase(s.find_last_not_of(" \t") + 1);
    return s;
}

// Удаление кавычек
string removeQuotes(const string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

// Проверка строки на корректность
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

// Чтение данных из файла
vector<pokazania> readDataFromFile(const string& filename) {
    ifstream ist(filename);
    if (!ist) {
        cerr << "Ошибка: Не удалось открыть файл: " << filename << "\n";
        return {};
    }

    vector<pokazania> data;
    string line;

    regex format1(R"((\d{4}\.\d{2}\.\d{2})\s+([\d.]+)\s+(.+))");
    regex format2(R"((.+)\s+(\d{4}\.\d{2}\.\d{2})\s+([\d.]+))");
    regex format3(R"(([\d.]+)\s+(.+)\s+(\d{4}\.\d{2}\.\d{2}))");

    while (getline(ist, line)) {
        trim(line);
        smatch match;
        pokazania obj;

        if (regex_match(line, match, format1)) {
            obj.date = match[1];
            obj.znach = stod(match[2]);
            obj.resyrs = removeQuotes(match[3]);
        } else if (regex_match(line, match, format2)) {
            obj.resyrs = removeQuotes(match[1]);
            obj.date = match[2];
            obj.znach = stod(match[3]);
        } else if (regex_match(line, match, format3)) {
            obj.znach = stod(match[1]);
            obj.resyrs = removeQuotes(match[2]);
            obj.date = match[3];
        } else {
            cerr << "Ошибка: Строка не соответствует ни одному формату: " << line << "\n";
            continue;
        }

        if (validateLine(obj.resyrs, obj.date, obj.znach)) {
            data.push_back(obj);
        }
    }
    return data;
}

// Вывод данных
void printData(const vector<pokazania>& data) {
    if (data.empty()) {
        cout << "Данные отсутствуют или некорректны.\n";
        return;
    }

    for (const auto& entry : data) {
        cout << "Тип ресурса: " << entry.resyrs << "\n";
        cout << "Дата: " << entry.date << "\n";
        cout << "Значение: " << entry.znach << "\n\n";
    }
}

// Подсчёт суммарного потребления
void totalConsumption(const vector<pokazania>& data) {
    string resource, start_date, end_date;
    cout << "Введите тип ресурса: ";
    cin.ignore(); // Очистка буфера
    getline(cin, resource);
    cout << "Введите начальную дату (YYYY.MM.DD): ";
    cin >> start_date;
    cout << "Введите конечную дату (YYYY.MM.DD): ";
    cin >> end_date;

    double total = accumulate(data.begin(), data.end(), 0.0, 
        [&](double sum, const pokazania& entry) {
            if (entry.resyrs == resource && entry.date >= start_date && entry.date <= end_date) {
                return sum + entry.znach;
            }
            return sum;
        });

    cout << "Суммарное потребление ресурса \"" << resource << "\" с "
         << start_date << " по " << end_date << " составляет: " << total << endl;
}

// Сортировка по дате
void sortByDate(vector<pokazania>& data) {
    stable_sort(data.begin(), data.end(), [](const pokazania& a, const pokazania& b) {
        return a.date < b.date;
    });

    cout << "Данные отсортированы по дате:\n";
    printData(data);
}

// Главная функция
int main() {
    setlocale(LC_ALL, "Russian");
    string filename = "1.txt";
    vector<pokazania> data = readDataFromFile(filename);
    printData(data);

    totalConsumption(data);
    sortByDate(data);

    return 0;
}
