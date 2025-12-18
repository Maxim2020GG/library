#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <cctype>
#include <iomanip>
#include <algorithm>
#include <locale>
#include <windows.h>
#include <ctime>

// Простая структура для хранения данных книги
struct BookData {
    std::string title;
    std::string author;
    std::string publisher;
    int year;

    BookData(const std::string& t, const std::string& a, const std::string& p, int y)
        : title(t), author(a), publisher(p), year(y) {
    }
};

class BookCopy {
private:
    std::string copyId;
    std::string location;
    std::string condition;
    bool available;

public:
    BookCopy(const std::string& id, const std::string& loc, const std::string& cond, bool avail = true)
        : copyId(id), location(loc), condition(cond), available(avail) {
    }

    std::string getInfo() const {
        return "Копия " + copyId + " (" + condition + ", " + location +
            ", " + (available ? "доступна" : "выдана") + ")";
    }

    std::string getCopyId() const { return copyId; }
    std::string getLocation() const { return location; }
    std::string getCondition() const { return condition; }
    bool isAvailable() const { return available; }

    void setAvailable(bool avail) { available = avail; }
};

class Literature {
protected:
    std::string id, title, author, publisher;
    int year;
    int pages;
    std::vector<BookCopy> copies;
    int issueNumber;
    std::string issueDate;

public:
    Literature(const std::string& id, const std::string& title,
        const std::string& author, const std::string& publisher, int year,
        int pages = 0, int issueNumber = 0, const std::string& issueDate = "")
        : id(id), title(title), author(author), publisher(publisher), year(year),
        pages(pages), issueNumber(issueNumber), issueDate(issueDate) {
    }

    std::string getId() const { return id; }
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    std::string getPublisher() const { return publisher; }
    int getYear() const { return year; }
    int getPages() const { return pages; }
    std::vector<BookCopy> getCopies() const { return copies; }
    int getIssueNumber() const { return issueNumber; }
    std::string getIssueDate() const { return issueDate; }

    void setPages(int p) { pages = p; }
    void setIssueNumber(int num) { issueNumber = num; }
    void setIssueDate(const std::string& date) { issueDate = date; }

    void addCopy(const BookCopy& copy) {
        copies.push_back(copy);
    }

    void removeCopy(const std::string& copyId) {
        copies.erase(std::remove_if(copies.begin(), copies.end(),
            [&copyId](const BookCopy& copy) { return copy.getCopyId() == copyId; }), copies.end());
    }

    std::string getInfo() const {
        std::string info = "ID: " + id + ", \"" + title + "\" by " + author +
            " (" + publisher + ", " + std::to_string(year) + ")";

        if (pages > 0) info += ", стр: " + std::to_string(pages);
        if (issueNumber > 0) info += ", вып: " + std::to_string(issueNumber);
        if (!issueDate.empty()) info += ", дата: " + issueDate;
        if (!copies.empty()) info += ", копий: " + std::to_string(copies.size());

        return info;
    }

    std::string getFullInfo() const {
        std::string info = "=== ПОДРОБНАЯ ИНФОРМАЦИЯ ===\n";
        info += "ID: " + id + "\n";
        info += "Название: " + title + "\n";
        info += "Автор: " + author + "\n";
        info += "Издательство: " + publisher + "\n";
        info += "Год: " + std::to_string(year) + "\n";
        info += "Страниц: " + std::to_string(pages) + "\n";
        info += "Номер выпуска: " + std::to_string(issueNumber) + "\n";
        info += "Дата выпуска: " + issueDate + "\n";

        if (!copies.empty()) {
            info += "\nФИЗИЧЕСКИЕ КОПИИ (" + std::to_string(copies.size()) + "):\n";
            for (size_t i = 0; i < copies.size(); ++i) {
                info += "  " + std::to_string(i + 1) + ". " + copies[i].getInfo() + "\n";
            }
        }
        else {
            info += "\nФИЗИЧЕСКИЕ КОПИИ: нет\n";
        }

        return info;
    }

    bool isSameBook(const std::string& t, const std::string& a,
        const std::string& p, int y) const {
        return title == t && author == a && publisher == p && year == y;
    }

    bool isSameTitleAuthor(const std::string& t, const std::string& a) const {
        return title == t && author == a;
    }

    virtual ~Literature() = default;
};

class LiteratureManager {
private:
    std::vector<Literature*> literatureList;
    std::string filePath = "D:\\VS\\Library\\literature.txt";

    bool createDirectoryIfNotExists(const std::string& path) {
        size_t pos = path.find_last_of("\\/");
        if (pos != std::string::npos) {
            std::string dir = path.substr(0, pos);
            if (CreateDirectoryA(dir.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
                return true;
            return false;
        }
        return true;
    }

    std::string getCurrentDate() {
        time_t now = time(0);
        struct tm ltm;
        localtime_s(&ltm, &now);
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << ltm.tm_mday << "."
            << std::setw(2) << std::setfill('0') << (1 + ltm.tm_mon) << "."
            << (1900 + ltm.tm_year);
        return ss.str();
    }

public:
    LiteratureManager() {
        createDirectoryIfNotExists(filePath);
    }

    bool bookExists(const std::string& title, const std::string& author,
        const std::string& publisher, int year) const {
        for (const auto& lit : literatureList)
            if (lit->isSameBook(title, author, publisher, year))
                return true;
        return false;
    }

    bool similarBookExists(const std::string& title, const std::string& author) const {
        for (const auto& lit : literatureList)
            if (lit->isSameTitleAuthor(title, author))
                return true;
        return false;
    }

    bool addLiterature(const std::string& id, const std::string& title,
        const std::string& author, const std::string& publisher, int year,
        int pages = 0, int issueNumber = 0, const std::string& issueDate = "") {

        if (bookExists(title, author, publisher, year)) {
            std::cout << "Ошибка: Такая книга уже существует!\n";
            return false;
        }

        if (similarBookExists(title, author)) {
            std::cout << "Внимание: Похожая книга уже есть!\n";
            for (const auto& lit : literatureList)
                if (lit->isSameTitleAuthor(title, author))
                    std::cout << "  - " << lit->getInfo() << std::endl;

            std::cout << "\nПродолжить? (y/n): ";
            char choice;
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (std::tolower(choice) != 'y') return false;
        }

        Literature* lit = new Literature(id, title, author, publisher, year, pages, issueNumber, issueDate);
        literatureList.push_back(lit);

        saveToFile();

        std::cout << "Добавлено: " << lit->getInfo() << std::endl;
        return true;
    }

    Literature* findBookById(const std::string& id) {
        for (auto& lit : literatureList) {
            if (lit->getId() == id) return lit;
        }
        return nullptr;
    }

    std::vector<Literature*> findBooksByTitleAuthor(const std::string& title, const std::string& author) {
        std::vector<Literature*> result;
        for (auto& lit : literatureList) {
            if (lit->isSameTitleAuthor(title, author)) {
                result.push_back(lit);
            }
        }
        return result;
    }

    void loadFromFile() {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cout << "Файл не найден. Будет создан новый.\n";
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::vector<std::string> parts;
            std::stringstream ss(line);
            std::string part;

            while (std::getline(ss, part, '|')) {
                parts.push_back(part);
            }

            if (parts.size() >= 8) {
                std::string id = parts[0];
                std::string title = parts[1];
                std::string author = parts[2];
                std::string publisher = parts[3];
                int year = atoi(parts[4].c_str());
                int pages = atoi(parts[5].c_str());
                int issueNumber = atoi(parts[6].c_str());
                std::string issueDate = parts[7];

                Literature* lit = new Literature(id, title, author, publisher, year, pages, issueNumber, issueDate);

                if (parts.size() > 8) {
                    std::string copiesData = parts[8];
                    std::stringstream copiesStream(copiesData);
                    std::string copyInfo;
                    while (std::getline(copiesStream, copyInfo, ';')) {
                        if (!copyInfo.empty()) {
                            std::vector<std::string> copyParts;
                            std::stringstream copySs(copyInfo);
                            std::string copyPart;
                            while (std::getline(copySs, copyPart, ',')) {
                                copyParts.push_back(copyPart);
                            }
                            if (copyParts.size() >= 4) {
                                bool available = (copyParts[3] == "1" || copyParts[3] == "true");
                                lit->addCopy(BookCopy(copyParts[0], copyParts[1], copyParts[2], available));
                            }
                        }
                    }
                }

                literatureList.push_back(lit);
            }
        }
        file.close();
        std::cout << "Загружено " << literatureList.size() << " записей.\n";
    }

    void saveToFile() {
        createDirectoryIfNotExists(filePath);
        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cout << "Ошибка сохранения файла!\n";
            return;
        }

        for (const auto& lit : literatureList) {
            file << lit->getId() << "|"
                << lit->getTitle() << "|"
                << lit->getAuthor() << "|"
                << lit->getPublisher() << "|"
                << lit->getYear() << "|"
                << lit->getPages() << "|"
                << lit->getIssueNumber() << "|"
                << lit->getIssueDate();

            auto copies = lit->getCopies();
            if (!copies.empty()) {
                file << "|";
                for (size_t i = 0; i < copies.size(); ++i) {
                    file << copies[i].getCopyId() << ","
                        << copies[i].getLocation() << ","
                        << copies[i].getCondition() << ","
                        << (copies[i].isAvailable() ? "1" : "0");
                    if (i < copies.size() - 1) file << ";";
                }
            }
            file << "\n";
        }
        file.close();
        std::cout << "Сохранено " << literatureList.size() << " книг.\n";
    }

    void showAll() {
        std::cout << "\n=== СПИСОК КНИГ ===\n";
        if (literatureList.empty()) {
            std::cout << "Список пуст.\n";
            return;
        }
        for (size_t i = 0; i < literatureList.size(); ++i)
            std::cout << i + 1 << ". " << literatureList[i]->getInfo() << std::endl;
        std::cout << "===================\n";
    }

    std::string getNextId() {
        if (literatureList.empty()) return "[001]";

        int maxId = 0;
        for (const auto& lit : literatureList) {
            std::string idStr = lit->getId();
            if (idStr.length() >= 3 && idStr[0] == '[' && idStr.back() == ']') {
                idStr = idStr.substr(1, idStr.length() - 2);
                maxId = std::max(maxId, atoi(idStr.c_str()));
            }
        }

        int nextId = maxId + 1;
        std::stringstream ss;
        ss << "[" << std::setw(3) << std::setfill('0') << nextId << "]";
        return ss.str();
    }

    // Функция для добавления начальных 50 книг
    void addInitialBooks() {
        // Массив из 50 классических книг
        BookData books[] = {
            BookData("Евгений Онегин", "А. С. Пушкин", "Русская классика", 1833),
            BookData("Капитанская дочка", "А. С. Пушкин", "Русская классика", 1836),
            BookData("Герой нашего времени", "М. Ю. Лермонтов", "Русская классика", 1840),
            BookData("Демон", "М. Ю. Лермонтов", "Русская классика", 1841),
            BookData("Мертвые души", "Н. В. Гоголь", "Русская классика", 1842),
            BookData("Ревизор", "Н. В. Гоголь", "Русская классика", 1836),
            BookData("Шинель", "Н. В. Гоголь", "Русская классика", 1842),
            BookData("Гроза", "А. Н. Островский", "Русская классика", 1859),
            BookData("Обломов", "И. А. Гончаров", "Русская классика", 1859),
            BookData("Отцы и дети", "И. С. Тургенев", "Русская классика", 1862),
            BookData("Записки охотника", "И. С. Тургенев", "Русская классика", 1852),
            BookData("Преступление и наказание", "Ф. М. Достоевский", "Русская классика", 1866),
            BookData("Идиот", "Ф. М. Достоевский", "Русская классика", 1869),
            BookData("Братья Карамазовы", "Ф. М. Достоевский", "Русская классика", 1880),
            BookData("Война и мир", "Л. Н. Толстой", "Русская классика", 1869),
            BookData("Анна Каренина", "Л. Н. Толстой", "Русская классика", 1877),
            BookData("Воскресение", "Л. Н. Толстой", "Русская классика", 1899),
            BookData("История одного города", "М. Е. Салтыков-Щедрин", "Русская классика", 1870),
            BookData("Господа Головлевы", "М. Е. Салтыков-Щедрин", "Русская классика", 1880),
            BookData("Кому на Руси жить хорошо", "Н. А. Некрасов", "Русская классика", 1876),
            BookData("Левша", "Н. С. Лесков", "Русская классика", 1881),
            BookData("Очарованный странник", "Н. С. Лесков", "Русская классика", 1873),
            BookData("Вишневый сад", "А. П. Чехов", "Русская классика", 1904),
            BookData("Дядя Ваня", "А. П. Чехов", "Русская классика", 1896),
            BookData("Рассказы", "А. П. Чехов", "Русская классика", 1890),
            BookData("Гранатовый браслет", "А. И. Куприн", "Русская классика", 1911),
            BookData("Господин из Сан-Франциско", "И. А. Бунин", "Русская классика", 1915),
            BookData("Темные аллеи", "И. А. Бунин", "Русская классика", 1943),
            BookData("На дне", "М. Горький", "Русская классика", 1902),
            BookData("Детство", "М. Горький", "Русская классика", 1913),
            BookData("Мастер и Маргарита", "М. А. Булгаков", "Русская классика", 1940),
            BookData("Белая гвардия", "М. А. Булгаков", "Русская классика", 1924),
            BookData("Двенадцать", "А. А. Блок", "Русская классика", 1918),
            BookData("Стихотворения", "С. А. Есенин", "Русская классика", 1925),
            BookData("Облако в штанах", "В. В. Маяковский", "Русская классика", 1915),
            BookData("Котлован", "А. П. Платонов", "Русская классика", 1930),
            BookData("Чевенгур", "А. П. Платонов", "Русская классика", 1929),
            BookData("Тихий Дон", "М. А. Шолохов", "Русская классика", 1940),
            BookData("Судьба человека", "М. А. Шолохов", "Русская классика", 1957),
            BookData("Конармия", "И. Э. Бабель", "Русская классика", 1926),
            BookData("Мы", "Е. И. Замятин", "Русская классика", 1920),
            BookData("Петр Первый", "А. Н. Толстой", "Русская классика", 1945),
            BookData("Василий Теркин", "А. Т. Твардовский", "Русская классика", 1945),
            BookData("Доктор Живаго", "Б. Л. Пастернак", "Русская классика", 1957),
            BookData("Один день Ивана Денисовича", "А. И. Солженицын", "Русская классика", 1962),
            BookData("Архипелаг ГУЛАГ", "А. И. Солженицын", "Русская классика", 1973),
            BookData("Колымские рассказы", "В. Т. Шаламов", "Русская классика", 1966),
            BookData("И дольше века длится день", "Ч. Т. Айтматов", "Русская классика", 1980),
            BookData("Прощание с Матерой", "В. Г. Распутин", "Русская классика", 1976),
            BookData("Царь-рыба", "В. П. Астафьев", "Русская классика", 1976)
        };

        int numBooks = sizeof(books) / sizeof(books[0]);

        std::cout << "Добавление " << numBooks << " классических книг...\n";

        for (int i = 0; i < numBooks; ++i) {
            const BookData& book = books[i];

            // Создаем ID вида [001], [002], ...
            std::stringstream idStream;
            idStream << "[" << std::setw(3) << std::setfill('0') << (i + 1) << "]";
            std::string id = idStream.str();

            // Пропускаем, если книга уже существует
            if (bookExists(book.title, book.author, book.publisher, book.year)) {
                continue;
            }

            // Случайные значения для дополнительных полей
            int pages = 100 + (i * 13) % 500;
            int issueNumber = (i % 5) + 1;
            std::string issueDate = "01.01." + std::to_string(book.year);

            Literature* lit = new Literature(id, book.title, book.author, book.publisher,
                book.year, pages, issueNumber, issueDate);
            literatureList.push_back(lit);
        }

        saveToFile();
        std::cout << "Добавлено " << numBooks << " классических книг.\n";
    }

    ~LiteratureManager() {
        for (auto& lit : literatureList) delete lit;
        literatureList.clear();
    }
};

void addBookWithDetails(LiteratureManager& manager) {
    std::cout << "\n=== ДОБАВЛЕНИЕ КНИГИ С ПОДРОБНОСТЯМИ ===\n";

    std::string id = manager.getNextId();
    std::string title, author, publisher, issueDate;
    int year, pages, issueNumber;

    std::cout << "ID: " << id << std::endl;
    std::cout << "Название: "; std::getline(std::cin, title);
    std::cout << "Автор: "; std::getline(std::cin, author);
    std::cout << "Издательство: "; std::getline(std::cin, publisher);

    while (true) {
        std::cout << "Год издания: ";
        std::cin >> year;
        if (std::cin.fail() || year < 0 || year > 2100) {
            std::cout << "Ошибка! Введите год 0-2100.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
    }

    std::cout << "Количество страниц (0 если неизвестно): ";
    std::cin >> pages;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Номер выпуска (0 если не применимо): ";
    std::cin >> issueNumber;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Дата выпуска (ДД.ММ.ГГГГ или Enter для текущей даты): ";
    std::getline(std::cin, issueDate);
    if (issueDate.empty()) {
        time_t now = time(0);
        struct tm ltm;
        localtime_s(&ltm, &now);
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << ltm.tm_mday << "."
            << std::setw(2) << std::setfill('0') << (1 + ltm.tm_mon) << "."
            << (1900 + ltm.tm_year);
        issueDate = ss.str();
        std::cout << "Установлена дата: " << issueDate << std::endl;
    }

    manager.addLiterature(id, title, author, publisher, year, pages, issueNumber, issueDate);
}

void editBookDetails(LiteratureManager& manager) {
    std::cout << "\n=== РЕДАКТИРОВАНИЕ КНИГИ ===\n";

    std::string searchTitle, searchAuthor;
    std::cout << "Введите название книги: ";
    std::getline(std::cin, searchTitle);
    std::cout << "Введите автора: ";
    std::getline(std::cin, searchAuthor);

    auto books = manager.findBooksByTitleAuthor(searchTitle, searchAuthor);

    if (books.empty()) {
        std::cout << "Книга не найдена!\n";
        return;
    }

    std::cout << "Найдено " << books.size() << " книг:\n";
    for (size_t i = 0; i < books.size(); ++i) {
        std::cout << i + 1 << ". " << books[i]->getInfo() << std::endl;
    }
    std::cout << "Выберите номер книги для редактирования: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choice < 1 || choice > books.size()) {
        std::cout << "Неверный выбор!\n";
        return;
    }

    Literature* book = books[choice - 1];

    std::cout << "\nРедактирование книги:\n";
    std::cout << "Текущие страницы: " << book->getPages() << std::endl;
    std::cout << "Новые страницы (текущее: " << book->getPages() << "): ";
    int pages;
    std::cin >> pages;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    book->setPages(pages);

    std::cout << "Текущий номер выпуска: " << book->getIssueNumber() << std::endl;
    std::cout << "Новый номер выпуска (текущее: " << book->getIssueNumber() << "): ";
    int issueNum;
    std::cin >> issueNum;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    book->setIssueNumber(issueNum);

    std::cout << "Текущая дата выпуска: " << book->getIssueDate() << std::endl;
    std::cout << "Новая дата выпуска (ДД.ММ.ГГГГ, Enter - оставить без изменений): ";
    std::string date;
    std::getline(std::cin, date);
    if (!date.empty()) book->setIssueDate(date);

    manager.saveToFile();
    std::cout << "Книга обновлена и сохранена!\n";
}

void manageBookCopies(LiteratureManager& manager) {
    std::cout << "\n=== УПРАВЛЕНИЕ ФИЗИЧЕСКИМИ КОПИЯМИ ===\n";

    std::string searchTitle, searchAuthor;
    std::cout << "Введите название книги: ";
    std::getline(std::cin, searchTitle);
    std::cout << "Введите автора: ";
    std::getline(std::cin, searchAuthor);

    auto books = manager.findBooksByTitleAuthor(searchTitle, searchAuthor);

    if (books.empty()) {
        std::cout << "Книга не найдена!\n";
        return;
    }

    std::cout << "Найдено " << books.size() << " книг:\n";
    for (size_t i = 0; i < books.size(); ++i) {
        std::cout << i + 1 << ". " << books[i]->getInfo() << std::endl;
    }
    std::cout << "Выберите номер книги: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choice < 1 || choice > books.size()) {
        std::cout << "Неверный выбор!\n";
        return;
    }

    Literature* book = books[choice - 1];

    std::cout << "\n" << book->getFullInfo() << std::endl;

    std::cout << "\n1. Добавить копию\n";
    std::cout << "2. Удалить копию\n";
    std::cout << "3. Назад\n";
    std::cout << "Выбор: ";

    int action;
    std::cin >> action;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (action == 1) {
        std::string copyId, location, condition;
        std::cout << "ID копии: ";
        std::getline(std::cin, copyId);
        std::cout << "Место хранения: ";
        std::getline(std::cin, location);
        std::cout << "Состояние (новый/хороший/удовлетворительный/ветхий): ";
        std::getline(std::cin, condition);

        book->addCopy(BookCopy(copyId, location, condition));
        manager.saveToFile();
        std::cout << "Копия добавлена!\n";
    }
    else if (action == 2) {
        auto copies = book->getCopies();
        if (copies.empty()) {
            std::cout << "Нет копий для удаления!\n";
            return;
        }

        std::cout << "Список копий:\n";
        for (size_t i = 0; i < copies.size(); ++i) {
            std::cout << i + 1 << ". " << copies[i].getInfo() << std::endl;
        }
        std::cout << "Выберите номер копии для удаления: ";
        int copyChoice;
        std::cin >> copyChoice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (copyChoice < 1 || copyChoice > copies.size()) {
            std::cout << "Неверный выбор!\n";
            return;
        }

        std::string copyId = copies[copyChoice - 1].getCopyId();
        book->removeCopy(copyId);
        manager.saveToFile();
        std::cout << "Копия удалена!\n";
    }
}

void showBookDetails(LiteratureManager& manager) {
    std::cout << "\n=== ПОДРОБНАЯ ИНФОРМАЦИЯ О КНИГЕ ===\n";

    std::string searchTitle, searchAuthor;
    std::cout << "Введите название книги: ";
    std::getline(std::cin, searchTitle);
    std::cout << "Введите автора: ";
    std::getline(std::cin, searchAuthor);

    auto books = manager.findBooksByTitleAuthor(searchTitle, searchAuthor);

    if (books.empty()) {
        std::cout << "Книга не найдена!\n";
        return;
    }

    std::cout << "Найдено " << books.size() << " книг:\n";
    for (size_t i = 0; i < books.size(); ++i) {
        std::cout << i + 1 << ". " << books[i]->getInfo() << std::endl;
    }
    std::cout << "Выберите номер книги для просмотра: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choice < 1 || choice > books.size()) {
        std::cout << "Неверный выбор!\n";
        return;
    }

    Literature* book = books[choice - 1];
    std::cout << "\n" << book->getFullInfo() << std::endl;
}

void printMenu() {
    std::cout << "\n=== ГЛАВНОЕ МЕНЮ ===\n";
    std::cout << "1. Показать все книги\n";
    std::cout << "2. Добавить книгу (базовое)\n";
    std::cout << "3. Добавить книгу с подробностями\n";
    std::cout << "4. Редактировать детали книги\n";
    std::cout << "5. Управление физическими копиями\n";
    std::cout << "6. Показать подробную информацию\n";
    std::cout << "7. Добавить 50 классических книг\n";
    std::cout << "8. Сохранить\n";
    std::cout << "9. Выйти\n";
    std::cout << "Выбор: ";
}

void addBooksManually(LiteratureManager& manager) {
    char choice;
    std::cout << "\n=== ДОБАВЛЕНИЕ КНИГ ===\n";

    do {
        std::cout << "\nДобавить книгу? (y/n): ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (std::tolower(choice) == 'y') {
            std::string id = manager.getNextId();
            std::string title, author, publisher;
            int year;

            std::cout << "ID: " << id << std::endl;
            std::cout << "Название: "; std::getline(std::cin, title);
            std::cout << "Автор: "; std::getline(std::cin, author);
            std::cout << "Издательство: "; std::getline(std::cin, publisher);

            while (true) {
                std::cout << "Год: ";
                std::cin >> year;
                if (std::cin.fail() || year < 0 || year > 2100) {
                    std::cout << "Ошибка! Введите год 0-2100.\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                else {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                }
            }

            manager.addLiterature(id, title, author, publisher, year);
        }

    } while (std::tolower(choice) == 'y');
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    std::setlocale(LC_ALL, "Russian");

    std::cout << "=== СИСТЕМА УПРАВЛЕНИЯ БИБЛИОТЕКОЙ ===\n";
    LiteratureManager manager;
    manager.loadFromFile();

    int choice;
    bool running = true;

    while (running) {
        printMenu();
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 1: manager.showAll(); break;
        case 2: addBooksManually(manager); break;
        case 3: addBookWithDetails(manager); break;
        case 4: editBookDetails(manager); break;
        case 5: manageBookCopies(manager); break;
        case 6: showBookDetails(manager); break;
        case 7: manager.addInitialBooks(); break;
        case 8: manager.saveToFile(); break;
        case 9:
            running = false;
            manager.saveToFile();
            std::cout << "Выход...\n";
            break;
        default: std::cout << "Неверный выбор\n"; break;
        } // ← эта скобка закрывает switch
    } // ← эта скобка закрывает while (running)

    std::cout << "\nНажмите Enter...";
    std::cin.get();
    return 0;
} // ← эта скобка закрывает main()