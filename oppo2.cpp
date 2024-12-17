#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <algorithm>
#include <sstream>
using namespace std;

struct pokazania {
    string resyrs; // Тип ресурса
    string date;   // Дата
    double znach = 0.0; // Значение
};

// Удаление лишних пробелов
string trim(string s) {
    s.erase(0, s.find_first_not_of(" \t"));
    s.erase(s.find_last_not_of(" \t") + 1);
    return s;
}

// Функция для проверки строки 
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

string removeQuotes(const string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
        return s.substr(1, s.size() - 2);
    }
    return s;
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
            obj.resyrs = removeQuotes(match[3]);
        }
        else if (regex_match(line, match, format2)) {
            obj.resyrs = removeQuotes(match[1]);
            obj.date = match[2];
            obj.znach = stod(match[3]);
        }
        else if (regex_match(line, match, format3)) {
            obj.znach = stod(match[1]);
            obj.resyrs = removeQuotes(match[2]);
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

// Функция для преобразования строки даты в числовое значение для сравнения
int dateToNumber(const string& date) {
    istringstream ss(date);
    int year, month, day;
    char dot;
    ss >> year >> dot >> month >> dot >> day;
    return year * 10000 + month * 100 + day; // Преобразуем в формат YYYYMMDD
}

// Функция для подсчета суммарного потребления ресурса за период времени
void totalConsumption(const vector<pokazania>& data) {
    string resource, start_date, end_date;
    cout << "Введите тип ресурса: ";
    getline(cin, resource);
    cout << "Введите начальную дату (YYYY.MM.DD): ";
    cin >> start_date;
    cout << "Введите конечную дату (YYYY.MM.DD): ";
    cin >> end_date;

    double total = 0.0;
    int start = dateToNumber(start_date);
    int end = dateToNumber(end_date);

    for (const auto& entry : data) {
        int current_date = dateToNumber(entry.date);
        if (entry.resyrs == resource && current_date >= start && current_date <= end) {
            total += entry.znach;
        }
    }

    cout << "Суммарное потребление ресурса \"" << resource << "\" с "
        << start_date << " по " << end_date << " составляет: " << total << endl;
}

// Функция для сортировки данных по дате
void sortByDate(vector<pokazania>& data) {
    sort(data.begin(), data.end(), [](const pokazania& a, const pokazania& b) {
        return dateToNumber(a.date) < dateToNumber(b.date);
        });

    cout << "Данные отсортированы по дате:\n";
    printData(data);
}


// Точка входа в программу
int main() {
    setlocale(LC_ALL, "Russian");
    string filename = "1.txt"; // Имя файла
    vector<pokazania> data = readDataFromFile(filename);
    printData(data);
    cin.ignore(); // Очистка буфера перед вводом строки
    // Вызов функций
    totalConsumption(data); // Подсчёт суммарного потребления
    sortByDate(data);       // Сортировка по дате

    return 0;
}

