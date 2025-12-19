// ==================== БИБЛИОТЕЧНАЯ СИСТЕМА - ПОЛНАЯ ВЕРСИЯ ====================
// Авторы: 
// 1. Лиана - класс Person (регистрация пользователей)
// 2. Зорин - классы Literature, BookCopy, LiteratureManager (управление книгами)
// 3. Сенюшкин - класс LibrarianSystem (функции библиотекаря)
// 4. Сечко - классы LoanRecord, LoanManager (учет выдачи и штрафов)
// 5. Байрамукова - класс User (будет добавлен позже)

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

using namespace std;

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================
void clearScreen() { system("cls"); }
void waitEnter() {
    cout << "\nНажмите Enter...";
    cin.ignore();
    cin.get();
}

// ==================== КЛАСС PERSON (Лиана) ====================
class Person {
private:
    string lastName;
    string birthDate;
    string phone;
    string address;

    bool isValidDate(const string& date) {
        if (date.length() != 10) return false;
        if (date[2] != '.' || date[5] != '.') return false;

        for (int i = 0; i < 10; i++) {
            if (i == 2 || i == 5) continue;
            if (date[i] < '0' || date[i] > '9') return false;
        }

        int day = stoi(date.substr(0, 2));
        int month = stoi(date.substr(3, 2));
        int year = stoi(date.substr(6, 4));

        if (day < 1 || day > 31) return false;
        if (month < 1 || month > 12) return false;
        if (year < 1900 || year > 2100) return false;

        return true;
    }

    bool isValidPhone(const string& phone) {
        int digitCount = 0;
        for (char c : phone) {
            if (c >= '0' && c <= '9') digitCount++;
        }
        return digitCount >= 10;
    }

    bool isPhoneUnique(const string& phone) {
        ifstream file("persons.txt");
        if (!file.is_open()) return true;

        string line;
        while (getline(file, line)) {
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);
            size_t pos3 = line.find(',', pos2 + 1);

            if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos) {
                string filePhone = line.substr(pos2 + 1, pos3 - pos2 - 1);
                if (filePhone == phone) {
                    file.close();
                    return false;
                }
            }
        }

        file.close();
        return true;
    }

public:
    Person() {}

    Person(string ln, string bd, string ph, string addr)
        : lastName(ln), birthDate(bd), phone(ph), address(addr) {
    }

    void printInfo() {
        cout << "\n=== КАРТОЧКА ПОЛЬЗОВАТЕЛЯ ===\n";
        cout << "Фамилия: " << lastName << "\n";
        cout << "Дата рождения: " << birthDate << "\n";
        cout << "Телефон: " << phone << "\n";
        cout << "Адрес: " << address << "\n";
    }

    void saveToFile() {
        ofstream file("persons.txt", ios::app);
        if (file.is_open()) {
            file << lastName << "," << birthDate << "," << phone << "," << address << "\n";
            file.close();
        }
    }

    static void registerUser() {
        Person p;

        cout << "\n=== РЕГИСТРАЦИЯ ПОЛЬЗОВАТЕЛЯ ===\n";
        cout << "Введите 0 для отмены\n";

        cout << "Фамилия: ";
        getline(cin, p.lastName);
        if (p.lastName == "0") {
            cout << "Регистрация отменена\n";
            return;
        }

        while (true) {
            cout << "Дата рождения (ДД.ММ.ГГГГ): ";
            getline(cin, p.birthDate);
            if (p.birthDate == "0") {
                cout << "Регистрация отменена\n";
                return;
            }
            if (p.isValidDate(p.birthDate)) break;
            cout << "Неверный формат! Пример: 15.05.1990\n";
        }

        while (true) {
            cout << "Телефон (10+ цифр): ";
            getline(cin, p.phone);
            if (p.phone == "0") {
                cout << "Регистрация отменена\n";
                return;
            }
            if (p.isValidPhone(p.phone)) {
                if (p.isPhoneUnique(p.phone)) break;
                cout << "Этот телефон уже зарегистрирован!\n";
            }
            else {
                cout << "Телефон должен содержать минимум 10 цифр!\n";
            }
        }

        cout << "Адрес: ";
        getline(cin, p.address);
        if (p.address == "0") {
            cout << "Регистрация отменена\n";
            return;
        }

        p.saveToFile();
        cout << "Пользователь успешно добавлен!\n";
        p.printInfo();

        waitEnter();
    }

    static void showAllUsers() {
        clearScreen();
        ifstream file("persons.txt");

        if (!file.is_open()) {
            cout << "Нет зарегистрированных пользователей.\n";
            waitEnter();
            return;
        }

        cout << "\n=== ВСЕ ПОЛЬЗОВАТЕЛЯ ===\n";

        string line;
        int count = 0;

        while (getline(file, line)) {
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);
            size_t pos3 = line.find(',', pos2 + 1);

            if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos) {
                string ln = line.substr(0, pos1);
                string bd = line.substr(pos1 + 1, pos2 - pos1 - 1);
                string ph = line.substr(pos2 + 1, pos3 - pos2 - 1);
                string addr = line.substr(pos3 + 1);

                Person p(ln, bd, ph, addr);
                p.printInfo();
                count++;
            }
        }

        file.close();
        cout << "\nВсего пользователей: " << count << "\n";
        waitEnter();
    }
};

// ==================== КЛАССЫ LITERATURE (Зорин) ====================

struct BookData {
    string title;
    string author;
    string publisher;
    int year;

    BookData(const string& t, const string& a, const string& p, int y)
        : title(t), author(a), publisher(p), year(y) {
    }
};

class BookCopy {
private:
    string copyId;
    string location;
    string condition;
    bool available;

public:
    BookCopy(const string& id, const string& loc, const string& cond, bool avail = true)
        : copyId(id), location(loc), condition(cond), available(avail) {
    }

    string getInfo() const {
        return "Копия " + copyId + " (" + condition + ", " + location +
            ", " + (available ? "доступна" : "выдана") + ")";
    }

    string getCopyId() const { return copyId; }
    string getLocation() const { return location; }
    string getCondition() const { return condition; }
    bool isAvailable() const { return available; }

    void setAvailable(bool avail) { available = avail; }
};

class Literature {
protected:
    string id;
    string title;
    string author;
    string publisher;
    int year;
    int pages;
    vector<BookCopy> copies;
    int issueNumber;
    string issueDate;

public:
    Literature(const string& id, const string& title,
        const string& author, const string& publisher, int year,
        int pages = 0, int issueNumber = 0, const string& issueDate = "")
        : id(id), title(title), author(author), publisher(publisher), year(year),
        pages(pages), issueNumber(issueNumber), issueDate(issueDate) {
    }

    string getId() const { return id; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getPublisher() const { return publisher; }
    int getYear() const { return year; }
    int getPages() const { return pages; }
    const vector<BookCopy>& getCopies() const { return copies; }
    int getIssueNumber() const { return issueNumber; }
    string getIssueDate() const { return issueDate; }

    void setPages(int p) { pages = p; }
    void setIssueNumber(int num) { issueNumber = num; }
    void setIssueDate(const string& date) { issueDate = date; }

    void addCopy(const BookCopy& copy) {
        copies.push_back(copy);
    }

    void removeCopy(const string& copyId) {
        for (auto it = copies.begin(); it != copies.end(); ++it) {
            if (it->getCopyId() == copyId) {
                copies.erase(it);
                break;
            }
        }
    }

    string getInfo() const {
        string info = "ID: " + id + ", \"" + title + "\" автор: " + author +
            " (" + publisher + ", " + to_string(year) + ")";

        if (pages > 0) info += ", стр: " + to_string(pages);
        if (issueNumber > 0) info += ", выпуск: " + to_string(issueNumber);
        if (!issueDate.empty()) info += ", дата выпуска: " + issueDate;
        if (!copies.empty()) info += ", копий: " + to_string(copies.size());

        return info;
    }

    string getFullInfo() const {
        string info = "=== ПОДРОБНАЯ ИНФОРМАЦИЯ ===\n";
        info += "ID: " + id + "\n";
        info += "Название: " + title + "\n";
        info += "Автор: " + author + "\n";
        info += "Издательство: " + publisher + "\n";
        info += "Год: " + to_string(year) + "\n";
        info += "Страниц: " + to_string(pages) + "\n";
        info += "Номер выпуска: " + to_string(issueNumber) + "\n";
        info += "Дата выпуска: " + issueDate + "\n";

        if (!copies.empty()) {
            info += "\nФИЗИЧЕСКИЕ КОПИИ (" + to_string(copies.size()) + "):\n";
            for (size_t i = 0; i < copies.size(); ++i) {
                info += "  " + to_string(i + 1) + ". " + copies[i].getInfo() + "\n";
            }
        }
        else {
            info += "\nФИЗИЧЕСКИЕ КОПИИ: нет\n";
        }

        return info;
    }

    bool isSameBook(const string& t, const string& a,
        const string& p, int y) const {
        return title == t && author == a && publisher == p && year == y;
    }

    bool isSameTitleAuthor(const string& t, const string& a) const {
        return title == t && author == a;
    }

    bool hasAvailableCopy() const {
        for (const auto& copy : copies) {
            if (copy.isAvailable()) return true;
        }
        return false;
    }

    string getAvailableCopyId() const {
        for (const auto& copy : copies) {
            if (copy.isAvailable()) return copy.getCopyId();
        }
        return "";
    }

    BookCopy* findCopyById(const string& copyId) {
        for (auto& copy : copies) {
            if (copy.getCopyId() == copyId) {
                return &copy;
            }
        }
        return nullptr;
    }

    const BookCopy* findCopyById(const string& copyId) const {
        for (const auto& copy : copies) {
            if (copy.getCopyId() == copyId) {
                return &copy;
            }
        }
        return nullptr;
    }

    virtual ~Literature() = default;
};

class LiteratureManager {
private:
    vector<Literature*> literatureList;
    string filePath = "literature.txt";

    bool createDirectoryIfNotExists(const string& path) {
        size_t pos = path.find_last_of("\\/");
        if (pos != string::npos) {
            string dir = path.substr(0, pos);
            if (CreateDirectoryA(dir.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
                return true;
            return false;
        }
        return true;
    }

public:
    LiteratureManager() {
        createDirectoryIfNotExists(filePath);
    }

    bool bookExists(const string& title, const string& author,
        const string& publisher, int year) const {
        for (const auto& lit : literatureList)
            if (lit->isSameBook(title, author, publisher, year))
                return true;
        return false;
    }

    bool similarBookExists(const string& title, const string& author) const {
        for (const auto& lit : literatureList)
            if (lit->isSameTitleAuthor(title, author))
                return true;
        return false;
    }

    bool addLiterature(const string& id, const string& title,
        const string& author, const string& publisher, int year,
        int pages = 0, int issueNumber = 0, const string& issueDate = "") {

        if (bookExists(title, author, publisher, year)) {
            cout << "Ошибка: Такая книга уже существует!\n";
            return false;
        }

        Literature* lit = new Literature(id, title, author, publisher, year,
            pages, issueNumber, issueDate);
        literatureList.push_back(lit);

        string copyId = id + "-1";
        lit->addCopy(BookCopy(copyId, "Полка А", "новая", true));

        saveToFile();

        cout << "Добавлено: " << lit->getInfo() << endl;
        return true;
    }

    Literature* findBookById(const string& id) {
        for (auto& lit : literatureList) {
            if (lit->getId() == id) return lit;
        }
        return nullptr;
    }

    vector<Literature*> findBooksByTitleAuthor(const string& title, const string& author) {
        vector<Literature*> result;
        for (auto& lit : literatureList) {
            if (lit->isSameTitleAuthor(title, author)) {
                result.push_back(lit);
            }
        }
        return result;
    }

    void loadFromFile() {
        ifstream file(filePath);
        if (!file.is_open()) {
            cout << "Файл не найден. Будет создан новый.\n";
            addInitialBooks();
            return;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            vector<string> parts;
            stringstream ss(line);
            string part;

            while (getline(ss, part, '|')) {
                parts.push_back(part);
            }

            if (parts.size() >= 8) {
                string id = parts[0];
                string title = parts[1];
                string author = parts[2];
                string publisher = parts[3];
                int year = stoi(parts[4]);
                int pages = stoi(parts[5]);
                int issueNumber = stoi(parts[6]);
                string issueDate = parts[7];

                Literature* lit = new Literature(id, title, author, publisher,
                    year, pages, issueNumber, issueDate);

                if (parts.size() > 8) {
                    string copiesData = parts[8];
                    stringstream copiesStream(copiesData);
                    string copyInfo;

                    while (getline(copiesStream, copyInfo, ';')) {
                        if (!copyInfo.empty()) {
                            vector<string> copyParts;
                            stringstream copySs(copyInfo);
                            string copyPart;

                            while (getline(copySs, copyPart, ',')) {
                                copyParts.push_back(copyPart);
                            }

                            if (copyParts.size() >= 4) {
                                bool available = (copyParts[3] == "1");
                                lit->addCopy(BookCopy(copyParts[0], copyParts[1],
                                    copyParts[2], available));
                            }
                        }
                    }
                }

                literatureList.push_back(lit);
            }
        }
        file.close();
        cout << "Загружено " << literatureList.size() << " записей.\n";
    }

    void saveToFile() {
        createDirectoryIfNotExists(filePath);
        ofstream file(filePath);
        if (!file.is_open()) {
            cout << "Ошибка сохранения файла!\n";
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
    }

    void showAll() {
        clearScreen();
        cout << "\n=== КАТАЛОГ КНИГ ===\n";
        if (literatureList.empty()) {
            cout << "Список пуст.\n";
            return;
        }
        for (size_t i = 0; i < literatureList.size(); ++i)
            cout << i + 1 << ". " << literatureList[i]->getInfo() << endl;
    }

    string getNextId() {
        if (literatureList.empty()) return "[001]";

        int maxId = 0;
        for (const auto& lit : literatureList) {
            string idStr = lit->getId();
            if (idStr.length() >= 3 && idStr[0] == '[' && idStr.back() == ']') {
                idStr = idStr.substr(1, idStr.length() - 2);
                int currentId = stoi(idStr);
                maxId = (maxId > currentId) ? maxId : currentId;
            }
        }

        int nextId = maxId + 1;
        stringstream ss;
        ss << "[" << setw(3) << setfill('0') << nextId << "]";
        return ss.str();
    }

    void addInitialBooks() {
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

        cout << "Добавление " << numBooks << " классических книг...\n";

        for (int i = 0; i < numBooks; ++i) {
            const BookData& book = books[i];

            stringstream idStream;
            idStream << "[" << setw(3) << setfill('0') << (i + 1) << "]";
            string id = idStream.str();

            if (bookExists(book.title, book.author, book.publisher, book.year)) {
                continue;
            }

            int pages = 100 + (i * 13) % 500;
            int issueNumber = (i % 5) + 1;
            string issueDate = "01.01." + to_string(book.year);

            Literature* lit = new Literature(id, book.title, book.author,
                book.publisher, book.year,
                pages, issueNumber, issueDate);

            string copyId1 = id + "-1";
            string copyId2 = id + "-2";
            lit->addCopy(BookCopy(copyId1, "Полка А", "новая", true));
            if (i % 2 == 0) {
                lit->addCopy(BookCopy(copyId2, "Полка Б", "хорошая", true));
            }

            literatureList.push_back(lit);
        }

        saveToFile();
        cout << "Добавлено " << numBooks << " классических книг.\n";
    }

    void showBookInfoById() {
        clearScreen();
        cout << "=== ПОИСК КНИГИ ПО ID ===\n";
        string bookId;
        cout << "Введите ID книги (например [001]): ";
        getline(cin, bookId);

        Literature* book = findBookById(bookId);
        if (book) {
            cout << "\n" << book->getFullInfo() << endl;
        }
        else {
            cout << "\nКнига не найдена!\n";
        }
    }

    void removeBookById() {
        clearScreen();
        cout << "=== УДАЛЕНИЕ КНИГИ ===\n";
        string bookId;
        cout << "Введите ID книги для удаления: ";
        getline(cin, bookId);

        Literature* book = findBookById(bookId);
        if (!book) {
            cout << "\nКнига не найдена!\n";
            waitEnter();
            return;
        }

        cout << "\nНайдена книга: " << book->getInfo() << endl;
        cout << "Удалить? (y/n): ";
        char choice;
        cin >> choice;
        cin.ignore();

        if (tolower(choice) == 'y') {
            for (auto it = literatureList.begin(); it != literatureList.end(); ++it) {
                if ((*it)->getId() == bookId) {
                    delete* it;
                    literatureList.erase(it);
                    break;
                }
            }
            saveToFile();
            cout << "\nКнига удалена!\n";
        }
        else {
            cout << "\nУдаление отменено\n";
        }
    }

    void addNewBookSimple() {
        clearScreen();
        cout << "=== ДОБАВЛЕНИЕ НОВОЙ КНИГИ ===\n";

        string id = getNextId();
        string title, author;

        cout << "ID новой книги: " << id << "\n";
        cout << "Название: ";
        getline(cin, title);
        cout << "Автор: ";
        getline(cin, author);

        if (addLiterature(id, title, author, "Новое издательство", 2024)) {
            cout << "Книга успешно добавлена!\n";
        }
    }

    void searchBooks() {
        clearScreen();
        cout << "=== ПОИСК КНИГ ===\n";
        string title, author;

        cout << "Название (можно часть): ";
        getline(cin, title);
        cout << "Автор (можно часть, оставьте пустым если не важно): ";
        getline(cin, author);

        vector<Literature*> results;

        if (title.empty() && author.empty()) {
            showAll();
            return;
        }

        for (auto& book : literatureList) {
            bool titleMatch = title.empty() ||
                book->getTitle().find(title) != string::npos;
            bool authorMatch = author.empty() ||
                book->getAuthor().find(author) != string::npos;

            if (titleMatch && authorMatch) {
                results.push_back(book);
            }
        }

        cout << "\n=== РЕЗУЛЬТАТЫ ПОИСКА ===\n";
        if (results.empty()) {
            cout << "Книги не найдены\n";
        }
        else {
            for (size_t i = 0; i < results.size(); i++) {
                cout << i + 1 << ". " << results[i]->getInfo() << "\n";
            }
        }
    }

    ~LiteratureManager() {
        for (auto& lit : literatureList) delete lit;
        literatureList.clear();
    }
};

// ==================== КЛАСС LOANRECORD (Сечко) ====================

class LoanRecord {
private:
    string loanId;
    string userId;
    string copyId;
    string borrowDate;
    string dueDate;
    string returnDate;
    double fine;

    int dateToDays(const string& date) const {
        if (date.length() != 10) return 0;

        int day = stoi(date.substr(0, 2));
        int month = stoi(date.substr(3, 2));
        int year = stoi(date.substr(6, 4));

        return day + (month * 30) + ((year - 2000) * 365);
    }

public:
    LoanRecord(string id, string uid, string cid, string borrow, string due)
        : loanId(id), userId(uid), copyId(cid),
        borrowDate(borrow), dueDate(due), fine(0) {
        returnDate = "";
    }

    bool isOverdue(string currentDate) {
        if (!returnDate.empty()) {
            return dateToDays(returnDate) > dateToDays(dueDate);
        }
        return dateToDays(currentDate) > dateToDays(dueDate);
    }

    double calcFine(string currentDate, double dailyFineRate = 10.0) {
        if (!isOverdue(currentDate)) {
            fine = 0;
            return 0;
        }

        int daysOverdue;
        if (!returnDate.empty()) {
            daysOverdue = dateToDays(returnDate) - dateToDays(dueDate);
        }
        else {
            daysOverdue = dateToDays(currentDate) - dateToDays(dueDate);
        }

        if (daysOverdue < 0) daysOverdue = 0;

        fine = daysOverdue * dailyFineRate;
        return fine;
    }

    string getLoanId() const { return loanId; }
    string getUserId() const { return userId; }
    string getCopyId() const { return copyId; }
    string getBorrowDate() const { return borrowDate; }
    string getDueDate() const { return dueDate; }
    string getReturnDate() const { return returnDate; }
    double getFine() const { return fine; }

    void setReturnDate(string date, string currentDate) {
        returnDate = date;
        calcFine(currentDate);
    }

    string getInfo() const {
        string info = "Выдача #" + loanId + ": " + userId + " -> " + copyId;
        info += " (выдано: " + borrowDate + ", вернуть до: " + dueDate;
        if (!returnDate.empty()) {
            info += ", возвращено: " + returnDate;
        }
        if (fine > 0) {
            char buffer[20];
            sprintf_s(buffer, "%.2f", fine);
            info += ", штраф: " + string(buffer) + " руб.";
        }
        info += ")";
        return info;
    }
};

class LoanManager {
private:
    vector<LoanRecord> loans;
    string loanFile = "loans.txt";

    string getCurrentDate() {
        time_t now = time(0);
        struct tm ltm;
        localtime_s(&ltm, &now);
        stringstream ss;
        ss << setw(2) << setfill('0') << ltm.tm_mday << "."
            << setw(2) << setfill('0') << (1 + ltm.tm_mon) << "."
            << (1900 + ltm.tm_year);
        return ss.str();
    }

    string getDueDate(int days = 14) {
        time_t now = time(0);
        struct tm ltm;
        localtime_s(&ltm, &now);
        ltm.tm_mday += days;
        mktime(&ltm);
        stringstream ss;
        ss << setw(2) << setfill('0') << ltm.tm_mday << "."
            << setw(2) << setfill('0') << (1 + ltm.tm_mon) << "."
            << (1900 + ltm.tm_year);
        return ss.str();
    }

    string generateLoanId() {
        int maxId = 0;
        for (const auto& loan : loans) {
            string idStr = loan.getLoanId();
            if (idStr.length() > 2 && idStr.substr(0, 2) == "LR") {
                try {
                    int idNum = stoi(idStr.substr(2));
                    if (idNum > maxId) maxId = idNum;
                }
                catch (...) {
                }
            }
        }
        return "LR" + to_string(maxId + 1);
    }

public:
    void loadFromFile() {
        ifstream file(loanFile);
        if (!file.is_open()) {
            cout << "Файл выдачи не найден. Будет создан новый.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            vector<string> parts;
            stringstream ss(line);
            string part;

            while (getline(ss, part, '|')) {
                parts.push_back(part);
            }

            if (parts.size() >= 6) {
                LoanRecord lr(parts[0], parts[1], parts[2], parts[3], parts[4]);
                if (parts[5] != "none" && !parts[5].empty()) {
                    lr.setReturnDate(parts[5], getCurrentDate());
                }
                loans.push_back(lr);
            }
        }

        file.close();
        cout << "Загружено выдач: " << loans.size() << "\n";
    }

    void saveToFile() {
        ofstream file(loanFile);
        for (const auto& loan : loans) {
            file << loan.getLoanId() << "|"
                << loan.getUserId() << "|"
                << loan.getCopyId() << "|"
                << loan.getBorrowDate() << "|"
                << loan.getDueDate() << "|"
                << (loan.getReturnDate().empty() ? "none" : loan.getReturnDate()) << "|"
                << loan.getFine() << "\n";
        }
        file.close();
    }

    string createLoan(string userId, string copyId) {
        string loanId = generateLoanId();
        string borrowDate = getCurrentDate();
        string dueDate = getDueDate(14);

        LoanRecord lr(loanId, userId, copyId, borrowDate, dueDate);
        loans.push_back(lr);
        saveToFile();

        return loanId;
    }

    bool returnBook(string loanId) {
        for (auto& loan : loans) {
            if (loan.getLoanId() == loanId && loan.getReturnDate().empty()) {
                loan.setReturnDate(getCurrentDate(), getCurrentDate());
                saveToFile();
                return true;
            }
        }
        return false;
    }

    LoanRecord* findActiveLoan(string userId, string copyId) {
        for (auto& loan : loans) {
            if (loan.getUserId() == userId &&
                loan.getCopyId() == copyId &&
                loan.getReturnDate().empty()) {
                return &loan;
            }
        }
        return nullptr;
    }

    LoanRecord* findLoanById(string loanId) {
        for (auto& loan : loans) {
            if (loan.getLoanId() == loanId) {
                return &loan;
            }
        }
        return nullptr;
    }

    void showAllLoans() {
        clearScreen();
        cout << "=== ВСЕ ВЫДАЧИ КНИГ ===\n";
        if (loans.empty()) {
            cout << "Нет записей о выдаче книг\n";
            return;
        }

        for (size_t i = 0; i < loans.size(); i++) {
            cout << i + 1 << ". " << loans[i].getInfo() << "\n";
        }
    }

    void showActiveLoans() {
        clearScreen();
        cout << "=== АКТИВНЫЕ ВЫДАЧИ (КНИГИ НА РУКАХ) ===\n";

        int count = 0;
        for (const auto& loan : loans) {
            if (loan.getReturnDate().empty()) {
                cout << ++count << ". " << loan.getInfo() << "\n";
            }
        }

        if (count == 0) {
            cout << "Нет активных выдач\n";
        }
    }

    void showOverdueLoans() {
        clearScreen();
        cout << "=== ПРОСРОЧЕННЫЕ ВЫДАЧИ ===\n";

        string currentDate = getCurrentDate();
        int count = 0;

        for (auto& loan : loans) {
            if (loan.isOverdue(currentDate)) {
                double fine = loan.getFine();
                if (fine == 0) {
                    loan.calcFine(currentDate);
                }
                cout << ++count << ". " << loan.getInfo() << "\n";
            }
        }

        if (count == 0) {
            cout << "Нет просроченных выдач\n";
        }
    }

    double calculateTotalFines() {
        string currentDate = getCurrentDate();
        double total = 0;

        for (auto& loan : loans) {
            if (loan.isOverdue(currentDate)) {
                total += loan.calcFine(currentDate);
            }
        }

        return total;
    }

    void showUserHistory(string userId) {
        clearScreen();
        cout << "=== ИСТОРИЯ ВЫДАЧ ДЛЯ: " << userId << " ===\n";

        int count = 0;
        for (const auto& loan : loans) {
            if (loan.getUserId() == userId) {
                cout << ++count << ". " << loan.getInfo() << "\n";
            }
        }

        if (count == 0) {
            cout << "Нет истории выдачи для этого пользователя\n";
        }
    }
};

// ==================== КЛАСС LIBRARIAN (Сенюшкин) ====================

class LibrarianSystem {
private:
    LiteratureManager* litManager;
    LoanManager* loanManager;

    const string LIB_PHONE = "89167778899";
    const string LIB_EMAIL = "librarian@library.ru";

    bool authenticate() {
        string inputPhone, inputEmail;

        cout << "\n=== ВХОД БИБЛИОТЕКАРЯ ===\n";
        cout << "Телефон: ";
        getline(cin, inputPhone);
        cout << "Email: ";
        getline(cin, inputEmail);

        if (inputPhone == LIB_PHONE && inputEmail == LIB_EMAIL) {
            cout << "Доступ разрешен\n";
            return true;
        }
        else {
            cout << "Доступ запрещен\n";
            return false;
        }
    }

    void bookManagementMenu() {
        while (true) {
            cout << "\n=== УПРАВЛЕНИЕ КНИГАМИ ===\n";
            cout << "1. Показать все книги\n";
            cout << "2. Добавить новую книгу\n";
            cout << "3. Удалить книгу\n";
            cout << "4. Поиск книги по ID\n";
            cout << "5. Поиск книг по названию/автору\n";
            cout << "6. Назад\n";
            cout << "Выбор: ";

            string choice;
            getline(cin, choice);

            if (choice == "1") {
                litManager->showAll();
            }
            else if (choice == "2") {
                litManager->addNewBookSimple();
            }
            else if (choice == "3") {
                litManager->removeBookById();
            }
            else if (choice == "4") {
                litManager->showBookInfoById();
            }
            else if (choice == "5") {
                litManager->searchBooks();
            }
            else if (choice == "6") {
                break;
            }

            waitEnter();
        }
    }

    void loanManagementMenu() {
        while (true) {
            cout << "\n=== ВЫДАЧА/ПРИЕМ КНИГ ===\n";
            cout << "1. Выдать книгу читателю\n";
            cout << "2. Принять книгу от читателя\n";
            cout << "3. Показать активные выдачи\n";
            cout << "4. Показать все выдачи\n";
            cout << "5. Показать просроченные выдачи\n";
            cout << "6. Проверить штрафы\n";
            cout << "7. Назад\n";
            cout << "Выбор: ";

            string choice;
            getline(cin, choice);

            if (choice == "1") {
                cout << "Фамилия читателя: ";
                string user;
                getline(cin, user);
                cout << "ID книги: ";
                string bookId;
                getline(cin, bookId);

                Literature* book = litManager->findBookById(bookId);
                if (book && book->hasAvailableCopy()) {
                    string copyId = book->getAvailableCopyId();
                    string loanId = loanManager->createLoan(user, copyId);
                    cout << "Книга выдана. ID выдачи: " << loanId << "\n";

                    BookCopy* copy = book->findCopyById(copyId);
                    if (copy) {
                        copy->setAvailable(false);
                        litManager->saveToFile();
                    }
                }
                else {
                    cout << "Книга не найдена или нет доступных копий\n";
                }
            }
            else if (choice == "2") {
                cout << "ID выдачи (LR...): ";
                string loanId;
                getline(cin, loanId);

                if (loanManager->returnBook(loanId)) {
                    cout << "Книга принята\n";
                    LoanRecord* loan = loanManager->findLoanById(loanId);
                    if (loan) {
                        // Здесь нужно обновить статус копии книги
                    }
                }
                else {
                    cout << "Выдача не найдена или книга уже возвращена\n";
                }
            }
            else if (choice == "3") {
                loanManager->showActiveLoans();
            }
            else if (choice == "4") {
                loanManager->showAllLoans();
            }
            else if (choice == "5") {
                loanManager->showOverdueLoans();
            }
            else if (choice == "6") {
                double totalFines = loanManager->calculateTotalFines();
                cout << "Общая сумма штрафов: " << totalFines << " руб.\n";
            }
            else if (choice == "7") {
                break;
            }

            waitEnter();
        }
    }

public:
    LibrarianSystem(LiteratureManager* lm, LoanManager* lom)
        : litManager(lm), loanManager(lom) {
    }

    void showMenu() {
        if (!authenticate()) {
            waitEnter();
            return;
        }

        while (true) {
            cout << "\n=== МЕНЮ БИБЛИОТЕКАРЯ ===\n";
            cout << "1. Управление книгами\n";
            cout << "2. Выдача/прием книг\n";
            cout << "3. Учет выдачи и штрафы\n";
            cout << "4. Выйти\n";
            cout << "Выбор: ";

            string choice;
            getline(cin, choice);

            if (choice == "1") {
                bookManagementMenu();
            }
            else if (choice == "2") {
                loanManagementMenu();
            }
            else if (choice == "3") {
                while (true) {
                    cout << "\n=== УЧЕТ ВЫДАЧИ И ШТРАФЫ ===\n";
                    cout << "1. Показать все выдачи\n";
                    cout << "2. Показать активные выдачи\n";
                    cout << "3. Показать просроченные выдачи\n";
                    cout << "4. Посчитать общие штрафы\n";
                    cout << "5. Назад\n";
                    cout << "Выбор: ";

                    string loanChoice;
                    getline(cin, loanChoice);

                    if (loanChoice == "1") {
                        loanManager->showAllLoans();
                    }
                    else if (loanChoice == "2") {
                        loanManager->showActiveLoans();
                    }
                    else if (loanChoice == "3") {
                        loanManager->showOverdueLoans();
                    }
                    else if (loanChoice == "4") {
                        double total = loanManager->calculateTotalFines();
                        cout << "Общая сумма штрафов: " << total << " руб.\n";
                    }
                    else if (loanChoice == "5") {
                        break;
                    }

                    waitEnter();
                }
            }
            else if (choice == "4") {
                break;
            }
        }
    }
};

// ==================== ГЛАВНАЯ ПРОГРАММА ====================

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    system("title Библиотечная система");

    cout << "=== БИБЛИОТЕЧНАЯ СИСТЕМА ===\n";
    cout << "Загрузка данных...\n";

    LiteratureManager litManager;
    LoanManager loanManager;
    LibrarianSystem librarian(&litManager, &loanManager);

    litManager.loadFromFile();
    loanManager.loadFromFile();

    while (true) {
        cout << "\n=== ГЛАВНОЕ МЕНЮ ===\n";
        cout << "1. Аккаунт (Лиана)\n";
        cout << "2. Библиотека\n";
        cout << "3. Выход\n";
        cout << "Выбор: ";

        string choice;
        getline(cin, choice);

        if (choice == "1") {
            while (true) {
                cout << "\n=== АККАУНТ ===\n";
                cout << "1. Регистрация пользователя\n";
                cout << "2. Показать всех пользователей\n";
                cout << "3. Назад\n";
                cout << "Выбор: ";

                string accChoice;
                getline(cin, accChoice);

                if (accChoice == "1") {
                    Person::registerUser();
                }
                else if (accChoice == "2") {
                    Person::showAllUsers();
                }
                else if (accChoice == "3") {
                    break;
                }
            }
        }
        else if (choice == "2") {
            while (true) {
                cout << "\n=== БИБЛИОТЕКА ===\n";
                cout << "1. Просмотр каталога книг\n";
                cout << "2. Поиск книги по ID\n";
                cout << "3. Поиск книг по названию/автору\n";
                cout << "4. Войти как библиотекарь\n";
                cout << "5. Назад\n";
                cout << "Выбор: ";

                string libChoice;
                getline(cin, libChoice);

                if (libChoice == "1") {
                    litManager.showAll();
                    waitEnter();
                }
                else if (libChoice == "2") {
                    litManager.showBookInfoById();
                    waitEnter();
                }
                else if (libChoice == "3") {
                    litManager.searchBooks();
                    waitEnter();
                }
                else if (libChoice == "4") {
                    librarian.showMenu();
                }
                else if (libChoice == "5") {
                    break;
                }
            }
        }
        else if (choice == "3") {
            cout << "Выход из программы...\n";
            break;
        }
    }

    return 0;
}