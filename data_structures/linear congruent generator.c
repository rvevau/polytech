#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <locale.h>
long long a = 184467440737095525;
long long c = 45989;
double n = (double)100000;

unsigned long long mypow(unsigned long long a, unsigned long long b) {
	unsigned long long result = 1;
	for (int i = 1; i <= b; i++) {
		result *= a;
	}
	return result;
}

unsigned long long my_fact(int x) {
	int fact = 1;
	while (x != 1) {
		fact *= x;
		x--;
	}
	return fact;
}

unsigned long long generator(unsigned long long x0) {
	unsigned long long xn = a * x0 + c;
	return xn;
}


unsigned long long period(unsigned long long x0) {
	unsigned long long counter = 2;
	unsigned long long first_x = generator(x0);
	unsigned long long x = generator(first_x);
	while (x != first_x) {
		x = generator(x);
		if (x != first_x) counter++;
		else break;
	}
	return counter;
}


int power() {
	unsigned long long temp;
	for (int s = 0; ; s++) {
		temp = mypow(a - 1, s);
		if (temp == 0) {
			return s;
		}
	}
}


void hihi_table(int k, double values[7]) {
	int v = k - 1;
	switch (v) {
	case 1:
		values[0] = 0.00016;
		values[1] = 0.00393;
		values[2] = 0.1015;
		values[3] = 0.4549;
		values[4] = 1.323;
		values[5] = 3.841;
		values[6] = 6.635;
		break;

	case 2:
		values[0] = 0.02010;
		values[1] = 0.1026;
		values[2] = 0.5754;
		values[3] = 1.386;
		values[4] = 2.773;
		values[5] = 5.991;
		values[6] = 9.210;
		break;

	case 3:
		values[0] = 0.1148;
		values[1] = 0.3518;
		values[2] = 1.213;
		values[3] = 2.366;
		values[4] = 4.108;
		values[5] = 7.815;
		values[6] = 11.34;
		break;

	case 4:
		values[0] = 0.2971;
		values[1] = 0.7107;
		values[2] = 1.923;
		values[3] = 3.357;
		values[4] = 5.385;
		values[5] = 9.488;
		values[6] = 13.28;
		break;

	case 5:
		values[0] = 0.5543;
		values[1] = 1.1455;
		values[2] = 2.675;
		values[3] = 4.351;
		values[4] = 6.626;
		values[5] = 11.07;
		values[6] = 15.09;
		break;

	case 6:
		values[0] = 0.8721;
		values[1] = 1.635;
		values[2] = 3.455;
		values[3] = 5.348;
		values[4] = 7.841;
		values[5] = 12.59;
		values[6] = 16.81;
		break;

	case 7:
		values[0] = 1.239;
		values[1] = 2.167;
		values[2] = 4.255;
		values[3] = 6.346;
		values[4] = 9.037;
		values[5] = 14.07;
		values[6] = 18.48;
		break;

	case 8:
		values[0] = 1.646;
		values[1] = 2.733;
		values[2] = 5.071;
		values[3] = 7.344;
		values[4] = 10.22;
		values[5] = 15.51;
		values[6] = 20.09;
		break;

	case 9:
		values[0] = 2.088;
		values[1] = 3.325;
		values[2] = 5.899;
		values[3] = 8.343;
		values[4] = 11.93;
		values[5] = 16.92;
		values[6] = 21.67;
		break;

	case 10:
		values[0] = 2.558;
		values[1] = 3.940;
		values[2] = 6.737;
		values[3] = 9.342;
		values[4] = 12.55;
		values[5] = 18.31;
		values[6] = 23.21;
		break;

	case 11:
		values[0] = 3.053;
		values[1] = 4.575;
		values[2] = 7.584;
		values[3] = 10.34;
		values[4] = 13.70;
		values[5] = 19.68;
		values[6] = 24.72;
		break;

	case 12:
		values[0] = 3.571;
		values[1] = 5.226;
		values[2] = 8.438;
		values[3] = 11.34;
		values[4] = 14.85;
		values[5] = 21.03;
		values[6] = 26.22;
		break;

	case 15:
		values[0] = 5.229;
		values[1] = 7.261;
		values[2] = 11.04;
		values[3] = 14.34;
		values[4] = 18.25;
		values[5] = 25.00;
		values[6] = 30.58;
		break;

	case 20:
		values[0] = 8.260;
		values[1] = 10.85;
		values[2] = 15.45;
		values[3] = 19.34;
		values[4] = 23.83;
		values[5] = 31.41;
		values[6] = 37.57;
		break;

	case 30:
		values[0] = 14.95;
		values[1] = 18.49;
		values[2] = 24.48;
		values[3] = 29.34;
		values[4] = 34.80;
		values[5] = 43.77;
		values[6] = 50.89;
		break;

	default:
		double x0 = -2.33;
		double x1 = -1.64;
		double x2 = -0.674;
		double x3 = 0.00;
		double x4 = 0.674;
		double x5 = 1.64;
		double x6 = 2.33;
		values[0] = v + x0 * sqrt(2 * v) + ((double)2 / 3) * x0 * x0 - ((double)2 / 3) + ((double)1 / sqrt(v));
		values[1] = v + x1 * sqrt(2 * v) + ((double)2 / 3) * x1 * x1 - ((double)2 / 3) + ((double)1 / sqrt(v));
		values[2] = v + x2 * sqrt(2 * v) + ((double)2 / 3) * x2 * x2 - ((double)2 / 3) + ((double)1 / sqrt(v));
		values[3] = v + x3 * sqrt(2 * v) + ((double)2 / 3) * x3 * x3 - ((double)2 / 3) + ((double)1 / sqrt(v));
		values[4] = v + x4 * sqrt(2 * v) + ((double)2 / 3) * x4 * x4 - ((double)2 / 3) + ((double)1 / sqrt(v));
		values[5] = v + x5 * sqrt(2 * v) + ((double)2 / 3) * x5 * x5 - ((double)2 / 3) + ((double)1 / sqrt(v));
		values[6] = v + x6 * sqrt(2 * v) + ((double)2 / 3) * x6 * x6 - ((double)2 / 3) + ((double)1 / sqrt(v));

	}

}


void hihi_kvadrat(int k) {
	unsigned long long otrezok = (pow(2, 64)) / k;
	unsigned long long xn;
	double values[7];
	double Vs[1000];
	double np;
	unsigned long long nums[100000];
	np = n / (double)k;
	srand(time(NULL));
	unsigned long long x0 = rand();
	for (int x = 0; x < 1000; x++) {
		double V = 0;
		x0 += x * 100;
		nums[0] = generator(x0);
		for (int i = 1; i < n; i++) {
			nums[i] = generator(nums[i - 1]);
		}
		int* otrezki = (int*)malloc(k * sizeof(int));
		for (int i = 0; i < k; i++) otrezki[i] = 0;
		for (int i = 0; i < n; i++) {
			for (unsigned long long j = 0; j < k; j++) {
				if (otrezok * j <= nums[i] && nums[i] < ((otrezok) * (j+1))) {
					otrezki[j]++;
					break;
				}
			}
		}
		for (int i = 0; i < k; i++) {
			V += (pow((otrezki[i] - np), 2)) / np;

		}
		Vs[x] = V;
		//for (int i = 0; i < k; i++) {
			//printf("%d\n", otrezki[i]);
		//}
		
		free(otrezki);
		//printf("\n");
	}

	hihi_table(k, values);

	short counters[8];
	for (int i = 0; i < 8; i++) {
		counters[i] = 0;
	}
	for (int i = 0; i < 1000; i++) {
		if (Vs[i] <= values[0]) counters[0]++;
		else if (Vs[i] >= values[6]) counters[7]++;
		else {
			for (int j = 0; j < 7; j++) {
				if (values[j] <= Vs[i] && Vs[i] <= values[j + 1]) {
					counters[j + 1]++;
					break;
				}
			}

		}
	}
	short good, bad;
	good = counters[1] + counters[2] + counters[3] + counters[4] + counters[5] + counters[6];
	bad = counters[0] + counters[7];
	printf("Результаты теста Хи-квадрат:\n\
%hi значений(-я) < 1%%\n\
%hi значений(-я) от 1%% до 5%%\n\
%hi значений(-я) от 5%% до 25%%\n\
%hi значений(-я) от 25%% до 50%%\n\
%hi значений(-я) от 50%% до 75%%\n\
%hi значений(-я) от 75%% до 95%%\n\
%hi значений(-я) от 95%% до 99%%\n\
%hi значений(-я) > 99%%\n\
Хороших чисел: %hi\n\
Подозрительных чисел: %hi", counters[0], counters[1], counters[2], counters[3], counters[4], counters[5], counters[6], counters[7], good, bad);

}


void monotonnost() {
	short d = 128;
	unsigned long long numbers[100000];
	srand(time(NULL));
	unsigned long long x0 = rand();
	double Vs[1000];
	for (int i = 0; i < 1000; i++) {
		Vs[i] = 0;
	}
	for (int x = 0; x < 1000; x++) {
		x0 += x * 100;
		numbers[0] = generator(x0);
		for (int i = 1; i < 100000; i++) {
			numbers[i] = generator(numbers[i - 1]);
		}
		int counters[7];
		for (int i = 0; i < 7; i++) {
			counters[i] = 0;
		}
		int r = 1;
		for (int j = 1; j < 100000; j++) {
			if (numbers[j - 1] < numbers[j]) {
				r++;
			}
			else {
				if (r >= 6) {
					counters[6]++;
					r = 1;
				}
				else {
					counters[r]++;
					r = 1;
				}
				j++;
			}
		}

		double summ = counters[1] + counters[2] + counters[3] + counters[4] + counters[5] + counters[6];
		double V = 0.0;

		for (int i = 1; i < 6; i++) {
			double j = double(i);
			double p1 = double(1);
			while (j != 0) {
				p1 /= j;
				j--;
			}
			double p2 = double(1);
			j = double(i + 1);
			while (j != 0) {
				p2 /= j;
				j--;
			}
			double p = p1 - p2;
			V += pow(counters[i] - summ * p, 2) / (summ * p);
		}
		double p = double(1);
		double t = double(6);
		while (t != 0) {
			p /= t;
			t--;
		}
		V += pow(counters[6] - summ * p, 2) / (summ * p);

		Vs[x] = V;

	}
	double values[7];
	hihi_table(6, values);
	short counters_new[8];
	for (int i = 0; i < 8; i++) {
		counters_new[i] = 0;
	}
	for (int i = 0; i < 1000; i++) {
		if (Vs[i] <= values[0]) counters_new[0]++;
		else if (Vs[i] >= values[6]) counters_new[7]++;
		else {
			for (int j = 0; j < 6; j++) {
				if (values[j] <= Vs[i] && Vs[i] <= values[j + 1]) {
					counters_new[j + 1]++;
					break;
				}
			}

		}
	}
	short good, bad;
	good = counters_new[1] + counters_new[2] + counters_new[3] + counters_new[4] + counters_new[5] + counters_new[6];
	bad = counters_new[0] + counters_new[7];
	printf("Результаты теста Хи-квадрат:\n\
%hi значений(-я) < 1%%\n\
%hi значений(-я) от 1%% до 5%%\n\
%hi значений(-я) от 5%% до 25%%\n\
%hi значений(-я) от 25%% до 50%%\n\
%hi значений(-я) от 50%% до 75%%\n\
%hi значений(-я) от 75%% до 95%%\n\
%hi значений(-я) от 95%% до 99%%\n\
%hi значений(-я) > 99%%\n\
Хороших чисел: %hi\n\
Подозрительных чисел: %hi", counters_new[0], counters_new[1], counters_new[2], counters_new[3], counters_new[4], counters_new[5], counters_new[6], counters_new[7], good, bad);

}


int main() {
	setlocale(LC_ALL, "ru");
	srand(time(NULL));
	unsigned long long x0 = rand();
	int vibor;
	printf("Выберите, что хотите получить:\n\
1) Период генератора\n\
2) Мощность генератора\n\
3) Критерий Хи-квадрат\n\
4) Критерий монотонности\n");
	printf("\n");
	scanf("%d", &vibor);

	printf("\n");
	if (vibor == 1) {
		unsigned long long razmer_perioda = period(x0);
		printf("%llu", razmer_perioda);
		printf("\n");
	}
	else if (vibor == 2) {
		printf("Мощность генератора = %d", power());
		printf("\n");
	}
	else if (vibor == 3) {
		int input;
		printf("Введите число k: ");
		scanf("%d", &input);
		printf("\n");
		hihi_kvadrat(input);
		printf("\n");
	}
	else if (vibor == 4) {
		monotonnost();
		printf("\n");
	}
	else printf("Номер введен некорректно!"); printf("\n");
	return 0;
}


