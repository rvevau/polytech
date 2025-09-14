#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syscall.h>

#define BUFFER_SIZE 1024 // Размер буфера для чтения/записи файла
#define MAX_FILE_SIZE (1024 * 1024) // Максимальный размер файла: 1 МБ

// Функция для вывода справки
void print_help() {
    printf("Использование: digset [команда] [имя файла] [число]\n");
    printf("Команды:\n");
    printf("  help                       Показать сообщение с командами\n");
    printf("  add [имя файла] [число]    Добавить число в файл\n");
    printf("  find [имя файла] [число]   Найти число в файле\n");
    printf("  del [имя файла] [число]    Удалить число из файла\n");
}

// Функция для чтения чисел из файла
int read_numbers(const char *filename, int *numbers, int *count) {
    int fd = open(filename);
    if (fd == -1) {
        *count = 0; // Файл не существует
        return 0;
    }

    // Получаем размер файла
    int size = filesize(fd);
    if (size == -1) {
        printf("Ошибка получения размера файла.\n");
        close(fd);
        return -1;
    }

    // Чтение данных из файла
    int bytes_read = read(fd, numbers, size);
    if (bytes_read != size) {
        printf("Ошибка чтения файла.\n");
        close(fd);
        return -1;
    }

    close(fd);
    *count = size / sizeof(int); // Количество чисел
    return 0;
}

// Функция для записи чисел в файл
int write_numbers(const char *filename, int *numbers, int count) {
    int fd = open(filename);
    if (fd == -1) {
        // Если файл не существует, создаем его
        create(filename, BUFFER_SIZE);
        fd = open(filename);
        if (fd == -1) {
            printf("Ошибка создания файла.\n");
            return -1;
        }
    }

    // Запись данных в файл
    int bytes_written = write(fd, numbers, count * sizeof(int));
    if (bytes_written != count * sizeof(int)) {
        printf("Ошибка записи в файл.\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

// Функция для добавления числа в файл
void add_number(const char *filename, int number) {
    printf("Добавление числа %d в файл %s...\n", number, filename);

    int numbers[MAX_FILE_SIZE / sizeof(int)];
    int count;

    // Читаем числа из файла
    if (read_numbers(filename, numbers, &count) == -1) {
        printf("Ошибка чтения файла.\n");
        return;
    }

    // Проверяем, есть ли число уже в файле
    for (int i = 0; i < count; i++) {
        if (numbers[i] == number) {
            printf("Число %d уже есть в файле.\n", number);
            return;
        }
    }

    // Добавляем число в массив
    numbers[count++] = number;

    // Сортируем массив
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (numbers[i] > numbers[j]) {
                int temp = numbers[i];
                numbers[i] = numbers[j];
                numbers[j] = temp;
            }
        }
    }

    // Записываем числа обратно в файл
    if (write_numbers(filename, numbers, count) == -1) {
        printf("Ошибка записи в файл.\n");
        return;
    }

    printf("Число %d успешно добавлено.\n", number);
}

// Функция для поиска числа в файле
void find_number(const char *filename, int number) {
    printf("Поиск числа %d в файле %s...\n", number, filename);

    int numbers[MAX_FILE_SIZE / sizeof(int)];
    int count;

    // Читаем числа из файла
    if (read_numbers(filename, numbers, &count) == -1) {
        printf("Ошибка чтения файла.\n");
        return;
    }

    // Ищем число в массиве
    for (int i = 0; i < count; i++) {
        if (numbers[i] == number) {
            printf("Число %d найдено в файле.\n", number);
            return;
        }
    }

    printf("Число %d не найдено в файле.\n", number);
}

// Функция для удаления числа из файла
void delete_number(const char *filename, int number) {
    printf("Удаление числа %d из файла %s...\n", number, filename);

    int numbers[MAX_FILE_SIZE / sizeof(int)];
    int count;

    // Читаем числа из файла
    if (read_numbers(filename, numbers, &count) == -1) {
        printf("Ошибка чтения файла.\n");
        return;
    }

    // Ищем число в массиве
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (numbers[i] == number) {
            found = 1;
            // Сдвигаем оставшиеся элементы влево
            for (int j = i; j < count - 1; j++) {
                numbers[j] = numbers[j + 1];
            }
            count--;
            break;
        }
    }

    if (!found) {
        printf("Число %d не найдено в файле.\n", number);
        return;
    }

    // Записываем числа обратно в файл
    if (write_numbers(filename, numbers, count) == -1) {
        printf("Ошибка записи в файл.\n");
        return;
    }

    printf("Число %d успешно удалено.\n", number);
}

// Главная функция
int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_help();
        return 1;
    }

    const char *command = argv[1];
    const char *filename = argv[2];

    if (strcmp(command, "add") == 0 && argc == 4) {
        int number = atoi(argv[3]);
        add_number(filename, number);
    } else if (strcmp(command, "find") == 0 && argc == 4) {
        int number = atoi(argv[3]);
        find_number(filename, number);
    } else if (strcmp(command, "del") == 0 && argc == 4) {
        int number = atoi(argv[3]);
        delete_number(filename, number);
    } else if (strcmp(command, "help") == 0) {
        print_help();
    } else {
        print_help();
        return 1;
    }

    return 0;
}
