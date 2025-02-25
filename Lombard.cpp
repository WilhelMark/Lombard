#include <iostream>
#include <string>
#include <sqlite3.h>
#include <iomanip>

// Функция для подключения к базе данных и выполнения запроса
std::string getTariffDetails(const std::string& tariffName, std::string& periodRanges, std::string& additionalConditions) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    // Открытие базы данных
    rc = sqlite3_open("pawnshop.db", &db);
    if (rc) {
        return "Не удалось открыть базу данных.";
    }

    // SQL-запрос для получения данных о тарифе
    std::string sql = "SELECT period_ranges, additional_conditions FROM Tariffs WHERE name = ?";
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return "Ошибка подготовки запроса.";
    }

    // Привязка параметра (название тарифа)
    sqlite3_bind_text(stmt, 1, tariffName.c_str(), -1, SQLITE_STATIC);

    // Выполнение запроса
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        periodRanges = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* conditions = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        additionalConditions = conditions ? conditions : "Нет дополнительных условий";
    } else {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return "Тариф не найден.";
    }

    // Освобождение ресурсов и закрытие базы данных
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return "OK";
}

// Функция для расчета суммы займа
double calculateLoan(double weight, const std::string& metalType) {
    double pricePerGram;

    // Установка цены за грамм в зависимости от типа металла
    if (metalType == "gold") {
        pricePerGram = 60.0; // Цена за грамм золота
    } else if (metalType == "silver") {
        pricePerGram = 1.0; // Цена за грамм серебра
    } else {
        throw std::invalid_argument("Некорректный тип металла.");
    }

    return weight * pricePerGram;
}

int main() {
    try {
        std::cout << "Добро пожаловать в программу для расчетов ломбарда!" << std::endl;

        // Ввод данных от пользователя
        double weight;
        std::cout << "Введите вес металла (в граммах): ";
        std::cin >> weight;

        std::string metalType;
        std::cout << "Введите тип металла ('gold' или 'silver'): ";
        std::cin >> metalType;

        std::string tariffName;
        std::cout << "Введите название тарифа: ";
        std::cin.ignore(); // Очистка буфера ввода
        std::getline(std::cin, tariffName);

        // Получение информации о тарифе из базы данных
        std::string periodRanges, additionalConditions;
        std::string result = getTariffDetails(tariffName, periodRanges, additionalConditions);

        if (result != "OK") {
            std::cerr << result << std::endl;
            return 1;
        }

        // Расчет суммы займа
        double loanAmount = calculateLoan(weight, metalType);

        // Вывод результатов
        std::cout << "\nРезультаты расчета:" << std::endl;
        std::cout << "Сумма займа: " << std::fixed << std::setprecision(2) << loanAmount << " руб." << std::endl;
        std::cout << "Детали тарифа:" << std::endl;
        std::cout << "  Диапазоны периодов: " << periodRanges << std::endl;
        std::cout << "  Дополнительные условия: " << additionalConditions << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
