#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syscall.h>

#define BUFFER_SIZE 1024


void print_help() {
	printf("Использование: digset [команда] [имя файла] [число]\n");
	printf("Команды:\n");
	printf("  help                       Показать сообщение с командами\n");
	printf("  add [имя файла] [число]    Добавить число в файл\n");
	printf("  find [имя файла] [число]   Найти число в файле\n");
	printf("  del [имя файла] [число]    Удалить число из файла\n");
}


void add_number(const char *filename, const char *number) {
	int fd = open(filename);
	if (fd == -1) {
		create(filename, 1024);
		fd = open(filename);
	}

	char buffer[BUFFER_SIZE];
	int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
	if (bytes_read == -1) {
        	printf("Ошибка чтения файла: %s\n", filename);
        	close(fd);
        	return;
	}

	buffer[bytes_read] = '\0';

	int current_length = strlen(buffer);
	strlcat(buffer, number, sizeof(buffer));
 	strlcat(buffer, "\n", sizeof(buffer));

	close(fd);

	fd = open(filename);
	if (fd == -1) {
        	printf("Ошибка открытия файла для записи");
        	return;
	}
	// SORTING //
	int int_number = atoi(number);
	int numbers[100];
	char* temp;
	int count = 0;
	char* token = strtok_r(buffer, "\n", &temp);
	while (token != NULL) {
		numbers[count] = atoi(token);
		count++;
		token = strtok_r(temp, "\n", &temp);
	}
	int i;
	for (i = 1; i < count; i++) {
        	int newElement = numbers[i];
        	int location = i - 1;
        	while(location >= 0 && numbers[location] > newElement) {
        		numbers[location+1] = numbers[location];
        		location = location - 1;
		}
		numbers[location+1] = newElement;
	}

	for (i = 1; i < count; i++) {
		if (numbers[i] == numbers[i - 1]) {
			printf("Число %d уже существует в файле\n", numbers[i]);
			return;
		}
	}

	i = 0;
	buffer[0] = '\0';
	char temp2[100];
	char* str_num;
	for (i; i < count; i++) {
		snprintf(temp2, sizeof(temp2), "%d", numbers[i]);
		strlcat(buffer, temp2, sizeof(buffer));
		strlcat(buffer, "\n", sizeof(buffer));
	}

	if (write(fd, buffer, strlen(buffer)) == -1) {
        	printf("Ошибка записи обновленного буфера в файл");
	}
	else {
		printf("Число %s добавлено в файл.\n", number);
	}

	close(fd);
}

void find_number(const char *filename, int number) {
	char buffer[BUFFER_SIZE];
	int fd = open(filename);
	if (fd == -1) {
		printf("Файла с данным именем не существует\n");
		return;
	}
	int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
	buffer[bytes_read] = '\0';
	int number2;
	char* temp;
	char* token = strtok_r(buffer, "\n", &temp);
        while (token != NULL) {
                number2 = atoi(token);
		if (number2 == number) {
			printf("Число %d найдено в файле\n", number2);
			return;
		}
                token = strtok_r(temp, "\n", &temp);
        }
	printf("Число %d не найдено в файле\n", number);
	return;

}



//функция для удаления числа из файла
void delete_number(const char *filename, int number) {
	int fd = open(filename);
	if (fd == -1) {
		create(filename, 1024);
		fd = open(filename);
	}

	char buffer[BUFFER_SIZE];
	int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
	if (bytes_read == -1) {
        	printf("Ошибка чтения файла: %s\n", filename);
        	close(fd);
        	return;
	}

	buffer[bytes_read] = '\0';

	int current_length = strlen(buffer);

	close(fd);

	fd = open(filename);
	if (fd == -1) {
        	printf("Ошибка открытия файла для записи");
        	return;
	}
	// SORTING //
	int int_number;
	int numbers[100];
	char* temp;
	int count = 0;
	int flag = 0;
	char* token = strtok_r(buffer, "\n", &temp);
	while (token != NULL) {
		int_number = atoi(token);
		if (int_number != number) {
			numbers[count] = int_number;
			count++;
		}
		else flag = 1;
		token = strtok_r(temp, "\n", &temp);
	}
	int i;
	buffer[0] = '\0';
	char temp2[100];
	for (i = 0; i < count; i++) {
		//printf("int i = %d, num = %d\n", i, numbers[i]);
		snprintf(temp2, sizeof(temp2), "%d", numbers[i]);
		strlcat(buffer, temp2, sizeof(buffer));
		strlcat(buffer, "\n", sizeof(buffer));
	}


	remove(filename);
	create(filename, 1024);
	fd = open(filename);


	//buffer[strlen(buffer)] = '\0';
	if (write(fd, buffer, strlen(buffer)) == -1) {
        	printf("Ошибка записи обновленного буфера в файл");
	}
	else {
		if (flag == 0) printf("Число %d не найдено в файле.\n", number);
		else printf("Удаление завершено\n");
	}

	close(fd);
}


// Главная функция
int main(int argc, char *argv[]) {
    if (argc < 3) {
	print_help();
        return 1;
    }

    const char *command = argv[1];
    const char *filename = argv[2];
    char *number = argv[3];


    if (strcmp(command, "add") == 0 && argc == 4) {
        add_number(filename, number);
    } else if (strcmp(command, "find") == 0 && argc == 4) {
        find_number(filename, atoi(number));
    } else if (strcmp(command, "del") == 0 && argc == 4) {
        delete_number(filename, atoi(number));
	}
     else {
	print_help();
        return 1;
    }
}
