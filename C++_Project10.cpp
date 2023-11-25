#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <limits>
#include <algorithm>

namespace MyAttendanceSystem {
    const int TOTAL_CLASSES = 100;
    const std::string STUDENT_FILE_NAME = "students.csv";

    class Student {
    private:
        std::string name;
        int rollNumber;
        int attendance;
        int totalClassesAttended;
        static int totalStudents;
        static double globalThreshold; // New member for global threshold percentage

    public:
        Student(const std::string& n, int roll) : name(n), rollNumber(roll), attendance(0), totalClassesAttended(0) {
            totalStudents++;
        }

        std::string getName() const { return name; }
        int getRollNumber() const { return rollNumber; }
        int getAttendance() const { return attendance; }
        int getTotalClassesAttended() const { return totalClassesAttended; }

        void markAttendance() {
            if (attendance < TOTAL_CLASSES) {
                attendance++;
                totalClassesAttended++;
            } else {
                std::cout << "Attendance cannot be marked. Maximum classes reached.\n";
            }
        }

        void setTotalClassesAttended(int total) {
            totalClassesAttended = total;
        }

        static void setGlobalThreshold(double threshold) {
            if (threshold >= 0 && threshold <= 100) {
                globalThreshold = threshold;
            } else {
                std::cerr << "Invalid threshold percentage. Using default (75%)\n";
                globalThreshold = 75.0;
            }
        }

        double getGlobalThreshold() const {
            return globalThreshold;
        }

        double getAttendancePercentage() const {
            if (totalClassesAttended == 0) {
                return 0;  // Avoid division by zero
            }
            return (static_cast<double>(attendance)*1000 / totalClassesAttended) * 10000;
        }

        void setAttendance(int newAttendance) {
            attendance = newAttendance;
        }

        void displayInfo() const {
            std::cout << "Name: " << name << "\nRoll Number: " << rollNumber << "\nAttendance: " << attendance << "\n";
        }

        void saveToCSV(std::ofstream& file) const {
            file << name << "," << rollNumber << "," << attendance << "," << totalClassesAttended << "\n";
        }

        bool isPassing() const {
            return getAttendancePercentage() >= globalThreshold;
        }

        static int getTotalStudents() {
            return totalStudents;
        }

        // Friend function to access private members of Student
        friend void friendFunction(const Student& student);
    };

    int Student::totalStudents = 0;
    double Student::globalThreshold = 75.0; // Initializing global threshold

    // Abstract class derived from Student
    class AbstractStudent : public Student {
    public:
        AbstractStudent(const std::string& n, int roll) : Student(n, roll) {}

        // Pure virtual function making this class abstract
        virtual void additionalInfo() const = 0;
    };

    class HighSchoolStudent : public Student {
    private:
        std::string gradeLevel;

    public:
        HighSchoolStudent(const std::string& n, int roll, const std::string& grade) : Student(n, roll), gradeLevel(grade) {}

        void displayInfo() const {
            Student::displayInfo();
            std::cout << "Grade Level: " << gradeLevel << "\n";
        }
    };

    std::vector<Student> loadStudents(const std::string& fileName) {
        std::vector<Student> students;
        std::ifstream file(fileName);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::stringstream ss(line);
                std::string name;
                int roll, attendance, totalClassesAttended;
                std::getline(ss, name, ',');
                ss >> roll;
                ss.ignore();
                ss >> attendance;
                ss.ignore();
                ss >> totalClassesAttended;
                students.emplace_back(name, roll);
                students.back().setAttendance(attendance);
                students.back().setTotalClassesAttended(totalClassesAttended);
            }
            file.close();
        }
        return students;
    }

    void saveStudents(const std::vector<Student>& students, const std::string& fileName) {
        std::ofstream file(fileName);
        if (file.is_open()) {
            for (const auto& student : students) {
                file << student.getName() << ","
                    << student.getRollNumber() << ","
                    << student.getAttendance() << "\n";
            }
            file.close();
            std::cout << "Data saved successfully.\n";
        } else {
            std::cerr << "Error: Unable to open the file for saving data.\n";
        }
    }

    void displayAllStudents(const std::vector<Student>& students) {
        // Lambda function to compare students based on attendance percentage
        auto compareByAttendance = [](const Student& a, const Student& b) {
            return a.getAttendancePercentage() > b.getAttendancePercentage();
        };

        std::vector<Student> sortedStudents = students;
        std::sort(sortedStudents.begin(), sortedStudents.end(), compareByAttendance);

        for (const auto& student : sortedStudents) {
            student.displayInfo();
            std::cout << "Attendance Percentage: " << student.getAttendancePercentage() << "%\n";
            if (student.isPassing()) {
                std::cout << "Result: Passing\n";
            } else {
                std::cout << "Result: Failing\n";
            }
            std::cout << "------------------------\n";
        }
    }

    void searchStudentByName(const std::vector<Student>& students, const std::string& searchName) {
        bool found = false;
        for (const auto& student : students) {
            if (student.getName() == searchName) {
                student.displayInfo();
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "Student with name '" << searchName << "' not found.\n";
        }
    }

    void displayAttendance(const std::vector<Student>& students) {
        for (const auto& student : students) {
            std::cout << "Name: " << student.getName() << "\n Classes Attended: "
                      << student.getAttendancePercentage() << "%\n\n";
        }
    }

    // Friend function to display all passing students
    void friendFunction(const std::vector<Student>& students) {
        std::cout << "Students who are passing:\n";
        for (const auto& student : students) {
            if (student.isPassing()) {
                std::cout << "Name: " << student.getName() << "\n";
                std::cout << "------------------------\n";
            }
        }
    }
}
    int main(int argc, char* argv[]) {
        using namespace MyAttendanceSystem;

        std::string fileName = STUDENT_FILE_NAME;

        if (argc > 1) {
            fileName = argv[1];
        }

        std::vector<Student> students = loadStudents(fileName);

        int choice;
        do {
            std::cout << "|-------------------------|\n";
            std::cout << "|       Menu Options      |\n";
            std::cout << "|-------------------------|\n";
            std::cout << "| 1. Add Student          |\n";
            std::cout << "| 2. Mark Attendance      |\n";
            std::cout << "| 3. Display Information  |\n";
            std::cout << "| 4. Display All Students |\n";
            std::cout << "| 5. Search by Name       |\n";
            std::cout << "| 6. Classes Attended     |\n";
            std::cout << "| 7. Set Threshold        |\n";
            std::cout << "| 8. Save and Exit        |\n";
            std::cout << "|-------------------------|\n";
            std::cout << "Enter your choice: ";
            std::cin >> choice;

            switch (choice) {
                case 1: {
                    std::string name;
                    int roll;
                    std::cout << "Enter name: ";
                    std::cin.ignore();
                    std::getline(std::cin, name);

                    try {
                        std::cout << "Enter roll number: ";
                        std::cin >> roll;

                        if (std::cin.fail()) {
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            throw std::invalid_argument("Invalid roll number. Please enter a valid integer.");
                        }

                        // Check if the roll number already exists
                        bool duplicateRollNumber = std::any_of(students.begin(), students.end(),
                            [roll](const Student& student) { return student.getRollNumber() == roll; });

                        if (duplicateRollNumber) {
                            throw std::invalid_argument("Student with the same roll number already exists.");
                        }

                        students.emplace_back(name, roll);
                    } catch (const std::exception& e) {
                        std::cerr << "Error: " << e.what() << "\n";
                    }
                    break;
                }

                case 2: {
                    int roll;
                    std::cout << "Enter roll number: ";
                    std::cin >> roll;
                    while (roll != 0) {
                        for (auto& student : students) {
                            if (student.getRollNumber() == roll) {
                                student.markAttendance();
                                std::cout << "Attendance marked for " << student.getName() << "\n";
                                break;
                            }
                        }
                        std::cout << "Enter roll number (0 to exit): ";
                        std::cin >> roll;
                    }
                    break;
                }
                case 3: {
                    int roll;
                    std::cout << "Enter roll number: ";
                    std::cin >> roll;
                    for (const auto& student : students) {
                        if (student.getRollNumber() == roll) {
                            student.displayInfo();
                            break;
                        }
                    }
                    break;
                }
                case 4:
                    displayAllStudents(students);
                    break;
                case 5: {
                    std::string searchName;
                    std::cout << "Enter name to search: ";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::getline(std::cin, searchName);
                    searchStudentByName(students, searchName);
                    break;
                }
                case 6:
                    displayAttendance(students);
                    break;
                case 7: {
                    double threshold;
                    std::cout << "Enter new threshold percentage for the class: ";
                    std::cin >> threshold;
                    Student::setGlobalThreshold(threshold);
                    std::cout << "Threshold updated for the entire class.\n";
                    break;
                }
                case 8:
                    saveStudents(students, fileName);
                    std::cout << "Data saved. Exiting.\n";
                    break;
                default:
                    std::cout << "Invalid choice. Try again.\n";
                    break;
            }
        } while (choice != 8);

        friendFunction(students);

        return 0;
    }
