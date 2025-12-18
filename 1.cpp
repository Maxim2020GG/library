#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <cctype>
#include <iomanip>
#include <algorithm>

class Literature {
protected:
    std::string id;
    std::string title;
    std::string author;
    std::string publisher;
    int year;

public:
    Literature(const std::string& id, const std::string& title,
        const std::string& author, const std::string& publisher, int year)
        : id(id), title(title), author(author), publisher(publisher), year(year) {
    }

    std::string getId() const { return id; }
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    std::string getPublisher() const { return publisher; }
    int getYear() const { return year; }

    virtual std::string getInfo() const {
        return "ID: " + id + ", \"" + title + "\" by " + author +
            " (" + publisher + ", " + std::to_string(year) + ")";
    }

    // Метод для сравнения книг (без учета ID)
    bool isSameBook(const std::string& title, const std::string& author,
        const std::string& publisher, int year) const {
        return (this->title == title &&
            this->author == author &&
            this->publisher == publisher &&
            this->year == year);
    }

    // Метод для сравнения только по названию и автору
    bool isSameTitleAuthor(const std::string& title, const std::string& author) const {
        return (this->title == title && this->author == author);
    }

    virtual ~Literature() = default;
};

class LiteratureManager {
private:
    std::vector<Literature*> literatureList;
    const std::string filePath = "C:\\ghjtrn\\literature.txt";

public:
    // Проверить, существует ли уже такая книга
    bool bookExists(const std::string& title, const std::string& author,
        const std::string& publisher, int year) const {
        for (const auto& lit : literatureList) {
            if (lit->isSameBook(title, author, publisher, year)) {
                return true;
            }
        }
        return false;
    }

    // Проверить, существует ли книга с таким же названием и автором
    bool similarBookExists(const std::string& title, const std::string& author) const {
        for (const auto& lit : literatureList) {
            if (lit->isSameTitleAuthor(title, author)) {
                return true;
            }
        }
        return false;
    }

    // Добавить новую литературу с проверкой на дубликаты
    bool addLiterature(const std::string& id, const std::string& title,
        const std::string& author, const std::string& publisher, int year) {
        // Проверяем, не существует ли уже такая книга
        if (bookExists(title, author, publisher, year)) {
            std::cout << "Ошибка: Такая книга уже существует в базе!" << std::endl;
            std::cout << "Нельзя добавить дубликат: \"" << title << "\" by " << author
                << " (" << publisher << ", " << year << ")" << std::endl;
            return false;
        }

        // Дополнительная проверка: книга с таким же названием и автором
        if (similarBookExists(title, author)) {
            std::cout << "Внимание: Книга с таким же названием и автором уже существует!" << std::endl;
            std::cout << "Вы уверены, что хотите добавить? Это может быть другое издание." << std::endl;

            // Показываем существующие книги с таким же названием и автором
            std::cout << "Существующие книги:" << std::endl;
            for (const auto& lit : literatureList) {
                if (lit->isSameTitleAuthor(title, author)) {
                    std::cout << "  - " << lit->getInfo() << std::endl;
                }
            }

            std::cout << "\nПродолжить добавление? (y/n): ";
            char choice;
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (tolower(choice) != 'y') {
                std::cout << "Добавление отменено." << std::endl;
                return false;
            }
        }

        Literature* lit = new Literature(id, title, author, publisher, year);
        literatureList.push_back(lit);
        saveToFile();
        std::cout << "Успешно добавлено: " << lit->getInfo() << std::endl;
        return true;
    }

    // Загрузить все из файла
    void loadFromFile() {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cout << "Файл не найден, создаем новый." << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::istringstream iss(line);
            std::string id, title, author, publisher;
            int year;

            std::getline(iss, id, '|');
            std::getline(iss, title, '|');
            std::getline(iss, author, '|');
            std::getline(iss, publisher, '|');
            iss >> year;

            Literature* lit = new Literature(id, title, author, publisher, year);
            literatureList.push_back(lit);
        }
        file.close();
        std::cout << "Загружено " << literatureList.size() << " записей." << std::endl;
    }

    // Сохранить в файл
    void saveToFile() {
        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cout << "Ошибка создания файла!" << std::endl;
            return;
        }

        for (const auto& lit : literatureList) {
            file << lit->getId() << "|" << lit->getTitle() << "|"
                << lit->getAuthor() << "|" << lit->getPublisher() << "|"
                << lit->getYear() << "\n";
        }
        file.close();
    }

    // Показать все
    void showAll() {
        std::cout << "\n=== СПИСОК ЛИТЕРАТУРЫ ===\n";
        if (literatureList.empty()) {
            std::cout << "Список пуст." << std::endl;
            return;
        }
        for (size_t i = 0; i < literatureList.size(); ++i) {
            std::cout << i + 1 << ". " << literatureList[i]->getInfo() << std::endl;
        }
    }

    // Получить следующий доступный ID
    std::string getNextId() {
        if (literatureList.empty()) {
            return "[001]";
        }

        // Находим максимальный ID
        int maxId = 0;
        for (const auto& lit : literatureList) {
            std::string idStr = lit->getId();
            // Удаляем скобки и преобразуем в число
            if (idStr.length() >= 3) {
                idStr = idStr.substr(1, idStr.length() - 2); // Убираем []
                try {
                    int idNum = std::stoi(idStr);
                    if (idNum > maxId) maxId = idNum;
                }
                catch (...) {
                    continue;
                }
            }
        }

        // Форматируем следующий ID с использованием stringstream
        int nextId = maxId + 1;
        std::stringstream ss;
        ss << "[" << std::setw(3) << std::setfill('0') << nextId << "]";
        return ss.str();
    }

    ~LiteratureManager() {
        for (auto& lit : literatureList) {
            delete lit;
        }
    }
};

// Функция для очистки ввода
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Функция для нормализации строк (приведение к нижнему регистру)
std::string toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Функция для сравнения строк без учета регистра
bool compareIgnoreCase(const std::string& str1, const std::string& str2) {
    return toLowerCase(str1) == toLowerCase(str2);
}

// Функция для ручного добавления книг
void addBooksManually(LiteratureManager& manager) {
    char choice;

    std::cout << "\n=== ДОБАВЛЕНИЕ НОВЫХ КНИГ ===\n";

    do {
        std::cout << "\nДобавить новую книгу? (y/n): ";
        std::cin >> choice;
        clearInputBuffer();

        if (tolower(choice) == 'y') {
            std::string title, author, publisher;
            int year;

            // Генерируем автоматический ID
            std::string id = manager.getNextId();

            std::cout << "ID книги (автоматически): " << id << std::endl;

            // Ввод названия
            std::cout << "Введите название книги: ";
            std::getline(std::cin, title);

            // Ввод автора
            std::cout << "Введите автора: ";
            std::getline(std::cin, author);

            // Ввод издательства
            std::cout << "Введите издательство: ";
            std::getline(std::cin, publisher);

            // Ввод года
            while (true) {
                std::cout << "Введите год издания: ";
                std::cin >> year;

                if (std::cin.fail()) {
                    std::cout << "Ошибка! Введите корректный год.\n";
                    clearInputBuffer();
                }
                else if (year < 0 || year > 2100) {
                    std::cout << "Некорректный год! Введите год между 0 и 2100.\n";
                    clearInputBuffer();
                }
                else {
                    clearInputBuffer();
                    break;
                }
            }

            // Добавляем книгу с проверкой на дубликаты
            bool added = manager.addLiterature(id, title, author, publisher, year);

            if (added) {
                std::cout << "Книга успешно добавлена!\n";
            }
        }

    } while (tolower(choice) == 'y');

    std::cout << "\nЗавершение добавления книг.\n";
}

int main() {
    setlocale(LC_ALL, "Russian");
    LiteratureManager manager;
    manager.loadFromFile();

    // Добавление тестовых книг с проверкой
    std::cout << "\n=== ДОБАВЛЕНИЕ ТЕСТОВЫХ КНИГ ===\n";
    manager.addLiterature("[001]", "Война и мир", "Л.Н. Толстой", "Эксмо", 1869);
    manager.addLiterature("[002]", "1984", "Джордж Оруэлл", "АСТ", 1949);

    // Попытка добавить дубликат (должна быть отклонена)
    std::cout << "\n=== ПРОВЕРКА ДУБЛИКАТОВ ===\n";
    manager.addLiterature("[003]", "Война и мир", "Л.Н. Толстой", "Эксмо", 1869);

    // Попытка добавить книгу с тем же названием и автором, но другим издательством
    manager.addLiterature("[004]", "Война и мир", "Л.Н. Толстой", "АСТ", 1869);

    manager.showAll();

    // Добавление книг вручную
    addBooksManually(manager);

    // Показываем обновленный список
    manager.showAll();

    std::cout << "\nНажмите Enter для выхода...";
    std::cin.get();
    return 0;
}