#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <string>

const double PI = acos(-1); //Число пи

using namespace std;

//Функция из задания
double task_function(double x) {
 while (x > PI / 2)
  x -= PI;
 while (x < -PI / 2)
  x += PI;

 if (x >= -PI / 2 && x <= 0)
  return -x;
 if (x > 0 && x <= PI / 2)
  return x;
 return 0;
}

//Тригонометрический многочлен
double fourier_interpolation(double const& x, double const& alpha0, double const* alpha, double const* beta, double const& omega, int const& numIter) {
 double result = alpha0;
 for (int l = 0; l < numIter; l++)
  result += alpha[l] * cos((l + 1) * omega * x) + beta[l] * sin((l + 1) * omega * x);
 return result;
}

//Вывод таблицы для двух значений
void output_table(double const* row1, string row1Name, double const* row2, string row2Name, int rowLen) {
 for (int p = 0; p < ceil(rowLen / 8.); p++) {
  cout << setw(6) << row1Name << '|';
  for (int j = 0; j < 8 && j + p * 8 < rowLen; j++) {
   cout << setw(8) << row1[j + p * 8] << '|';
  }
  cout << endl;

  cout << setw(6) << row2Name << '|';
  for (int j = 0; j < 8 && j + p * 8 < rowLen; j++) {
   cout << setw(8) << row2[j + p * 8] << '|';
  }
  cout << endl;
 }
}

//Форматирование вывода double в файл .csv
string format_double(double num) {
 string strNum = to_string(num);
 for (int i = 0; i < strNum.length(); i++)
  if (strNum[i] == '.') {
   strNum[i] = ',';
   break;
  }
 return strNum;
}

//Вывод таблицы для построения графиков
void table_for_graphic(double const& Xleft, double const& Xright, int const& numOfIntervals, int const& numOfPolynoms,
 double const* alpha0, double** alpha, double** beta, double const& omega, int const* numOfIters) {

 double step = (Xright - Xleft) / numOfIntervals; //Шаг для x
 cout << setw(8) << 'x' << '|' << setw(8) << "y(x)" << '|';
 for (int j = 0; j < numOfPolynoms; j++) {
  cout << "n=" << setw(6) << numOfIters[j] << '|';
 }
 cout << endl;
 double x = Xleft; //Аргумент функции
 for (int i = 0; i <= numOfIntervals; i++) {
  cout << setw(8) << x << '|'
   << setw(8) << task_function(x) << '|';
  for (int j = 0; j < numOfPolynoms; j++)
   cout << setw(8) << fourier_interpolation(x, alpha0[j], alpha[j], beta[j], omega, numOfIters[j]) << '|';
  x += step;
  cout << endl;
 }

 ofstream fout;
 fout.open("lab3.csv");
 if (fout.is_open()) {
  fout << 'x' << ';' << "y(x)" << ';';
  for (int j = 0; j < numOfPolynoms; j++) {
   fout << "n=" << numOfIters[j] << ';';
  }
  fout << '\n';
  double x = Xleft;
  for (int i = 0; i <= numOfIntervals; i++) {
   fout << format_double(x) << ';'
    << format_double(task_function(x)) << ';';
   for (int j = 0; j < numOfPolynoms; j++)
    fout << format_double(fourier_interpolation(x, alpha0[j], alpha[j], beta[j], omega, numOfIters[j])) << ';';
   x += step;
   fout << '\n';
  }
  fout.close();
 }
}

int main() {
 setlocale(LC_ALL, "ru");
 cout.setf(ios::fixed);
 cout.precision(3);

 const int numOfPolynoms = 3; //Количество многочленов в программе
 const int N[numOfPolynoms] = { 4, 8, 16 }; //Количество коэффициентов многочлена при alpha и beta
 const double Xleft = -PI / 2; //Левая граница интервала
 const double Xright = PI / 2; //Правая граница интервала
 const double T = Xright - Xleft; //Период
 const double omega = 2 * PI / T; //Циклическая частота

 //Создание массива для аргументов функции
 double** X = new double* [numOfPolynoms];
 for (int i = 0; i < numOfPolynoms; i++)
  X[i] = new double[N[i] * 2];

 //Создание массива для значений функции
 double** Y = new double* [numOfPolynoms];
 for (int i = 0; i < numOfPolynoms; i++)
  Y[i] = new double[N[i] * 2];

 //Заполняем массивы X и Y
 for (int i = 0; i < numOfPolynoms; i++) {
  double step = T / (N[i] * 2);
  for (int j = 0; j < N[i] * 2; j++) {
   X[i][j] = Xleft + (j + 1) * step;
   Y[i][j] = task_function(X[i][j]);
  }
 }

 //Вывод таблиц для X и Y
 for (int i = 0; i < numOfPolynoms; i++) {
  int n = N[i];
  cout << "n=" << n << endl;
  output_table(X[i], "Xk", Y[i], "Yk", n << 1);
  cout << endl;
 }

 //Создание массивов для коэффициентов Фурье
 double alpha0[numOfPolynoms]; //Создание массива для альфа при l = 0
 //Создание массивов для остальных коэффициентов
 double** alpha = new double* [numOfPolynoms];
 for (int i = 0; i < numOfPolynoms; i++)
  alpha[i] = new double[N[i]];

 double** beta = new double* [numOfPolynoms];
 for (int i = 0; i < numOfPolynoms; i++)
  beta[i] = new double[N[i]];


 //Заполнение коэффициентов нулями
 for (int i = 0; i < numOfPolynoms; i++) {
  alpha0[i] = 0;
  for (int j = 0; j < N[i]; j++) {
   alpha[i][j] = 0;
   beta[i][j] = 0;
  }
 }

 //Нахождение коэффициентов ряда Фурье
 for (int i = 0; i < numOfPolynoms; i++) {
  for (int k = -N[i] + 1; k <= N[i]; k++) {
   alpha0[i] += Y[i][k + N[i] - 1];
  }
  for (int l = 0; l < N[i]; l++) {
   for (int k = -N[i] + 1; k <= N[i]; k++) {
    alpha[i][l] += Y[i][k + N[i] - 1] * cos((l + 1) * omega * k * T / (2 * N[i]));
    beta[i][l] += Y[i][k + N[i] - 1] * sin((l + 1) * omega * k * T / (2 * N[i]));
   }
   alpha[i][l] /= N[i];
   beta[i][l] /= N[i];
  }
  alpha0[i] /= 2 * N[i];
  alpha[i][N[i] - 1] /= 2;
  beta[i][N[i] - 1] /= 2;
 }

 //Вывод таблиц с коэффициентами
 for (int i = 0; i < numOfPolynoms; i++) {
  cout << "n=" << N[i] << "\talpha0=" << alpha0[i] << endl;
  output_table(alpha[i], "alpha", beta[i], "beta", N[i]);
  cout << endl;
 }

 table_for_graphic(Xleft, Xright, 50, numOfPolynoms, alpha0, alpha, beta, omega, N);

 for (int i = 0; i < numOfPolynoms; i++)
  delete[] beta[i];
 delete[] beta;
 for (int i = 0; i < numOfPolynoms; i++)
  delete[] alpha[i];
 delete[] alpha;

 for (int i = 0; i < numOfPolynoms; i++)
  delete[] Y[i];
 delete[] Y;
 for (int i = 0; i < numOfPolynoms; i++)
  delete[] X[i];
 delete[] X;

 system("pause");
 return 0;
}