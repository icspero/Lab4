#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

// структуры данных для студента и результата сессии
struct SessionResult {
    int semester;
    string subject; 
    int mark; // оценка
};

struct Student {
    string fullName; 
    string group;
    vector<SessionResult> sessionResults; // результаты сессий
};

// функция для поиска студентов, получивших стипендию за семестр
void findScholarshipStudents(const vector<Student>& students, const string& group, int semester, vector<Student>& result) {
    for (const auto& student : students) {
        if (student.group == group) {
            bool scholarship = true;
            for (const auto& result : student.sessionResults) {
                if (result.semester == semester && (result.mark < 4)) {
                    scholarship = false;
                    break;
                }
            }
            if (scholarship) {
                result.push_back(student);
            }
        }
    }
}

// функция без многопоточности
void processWithoutThreads(const vector<Student>& students, const string& group, int semester) {
    vector<Student> scholarshipStudents;
    auto start = chrono::high_resolution_clock::now();
    findScholarshipStudents(students, group, semester, scholarshipStudents);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    
    cout << "Время обработки без многопоточности: " << duration.count() << " секунд." << endl;
    cout << "Количество студентов со стипендией: " << scholarshipStudents.size() << endl;
    for (const auto& student : scholarshipStudents) {
        cout << student.fullName << endl;
    }
}

// функция для многопоточной обработки
void processWithThreads(const vector<Student>& students, const string& group, int semester) {
    vector<Student> scholarshipStudents;
    size_t numThreads = 4; // количество потоков
    vector<thread> threads;
    vector<vector<Student>> threadResults(numThreads);
    
    auto start = chrono::high_resolution_clock::now();
    
    // разделим работу между потоками
    for (size_t i = 0; i < numThreads; ++i) {
        threads.push_back(thread([&, i]() {
            for (size_t j = i; j < students.size(); j += numThreads) {
                const auto& student = students[j];
                if (student.group == group) {
                    bool scholarship = true;
                    for (const auto& result : student.sessionResults) {
                        if (result.semester == semester && (result.mark < 4)) {
                            scholarship = false;
                            break;
                        }
                    }
                    if (scholarship) {
                        threadResults[i].push_back(student);
                    }
                }
            }
        }));
    }
    
    // ожидаем завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }
    
    // собираем результаты из всех потоков
    for (const auto& threadResult : threadResults) {
        for (const auto& student : threadResult) {
            scholarshipStudents.push_back(student);
        }
    }
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    
    cout << "Время обработки с многопоточностью: " << duration.count() << " секунд." << endl;
    cout << "Количество студентов со стипендией: " << scholarshipStudents.size() << endl;
    for (const auto& student : scholarshipStudents) {
        cout << student.fullName << endl;
    }
}

int main() {
    vector<Student> students(1000000, {"Тырышкин Клим", "Группа 1", {{1, "Математика", 3}, {1, "Физика", 4}, {1, "Программирование", 3}}});
    students.push_back({"Самойлов Сергей", "Группа 1", {{1, "Математика", 5}, {1, "Физика", 4}, {1, "Программирование", 5}}});
    students.push_back({"Мартынова Екатерина", "Группа 1", {{1, "Математика", 4}, {1, "Физика", 5}, {1, "Программирование", 5}}});
    students.push_back({"Пупкин Василий", "Группа 1", {{1, "Математика", 5}, {1, "Физика", 5}, {1, "Программирование", 5}}});

    string group = "Группа 1";
    int semester = 1;
    
    processWithoutThreads(students, group, semester);
    
    cout << endl;

    processWithThreads(students, group, semester);
    
    return 0;
}