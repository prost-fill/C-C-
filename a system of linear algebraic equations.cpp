#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;

const int numPerem = 4;

double norma1(double matrix[numPerem][numPerem]) { //Первая норма матрицы
 double norma[numPerem];
 double max = 0;
 for (int j = 0; j < numPerem; j++) {
  norma[j] = 0;
  for (int i = 0; i < numPerem; i++)
   norma[j] += fabs(matrix[i][j]);
  if (max < norma[j])
   max = norma[j];
 }
 return max;
}

double norma2(double matrix[numPerem][numPerem]) { //Вторая норма матрицы
 double norma[numPerem];
 double max = 0;
 for (int i = 0; i < numPerem; i++) {
  norma[i] = 0;
  for (int j = 0; j < numPerem; j++)
   norma[i] += fabs(matrix[i][j]);
  if (max < norma[i])
   max = norma[i];
 }
 return max;
}

double norma3(double matrix[numPerem][numPerem]) { //Третья норма матрицы
 double norma = 0;
 for (int i = 0; i < numPerem; i++)
  for (int j = 0; j < numPerem; j++)
   norma += matrix[i][j] * matrix[i][j];
 return sqrt(norma);
}

//Приведение матрицы к виду, удобному для итераций
void comfortability(double matrix[numPerem][numPerem], double rightPart[numPerem], double const speedCoef) {
 double matrixT[numPerem][numPerem]; //Транспонированная матрица
 for (int i = 0; i < numPerem; i++)
  for (int j = 0; j < numPerem; j++)
   matrixT[i][j] = matrix[j][i];

 double C[numPerem][numPerem];
 for (int i = 0; i < numPerem; i++) //Произведение транспонированной и обычной матриц
  for (int j = 0; j < numPerem; j++) {
   C[i][j] = 0;
   for (int p = 0; p < numPerem; p++)
    C[i][j] += matrixT[i][p] * matrix[p][j];
  }

 double norms[3] = { norma1(C), norma2(C), norma3(C) };
 double minNorma = norms[0]; //Выбор минимальной нормы
 for (int i = 1; i < 3; i++)
  if (minNorma > norms[i])
   minNorma = norms[i];

 double G[numPerem][numPerem];
 for (int i = 0; i < numPerem; i++) { //Деление матрицы на норму и умножение на ускоряющий коэффициент
  for (int j = 0; j < numPerem; j++) {
   G[i][j] = speedCoef * (-C[i][j]) / minNorma;
  }
  G[i][i] += 1; //Сложение с единичной матрицей
 }

 double F[numPerem];
 for (int i = 0; i < numPerem; i++) {
  F[i] = 0;
  for (int j = 0; j < numPerem; j++) 
   //Умножение транспонированной матрицы на правую часть, на ускоряющий коэффициент и деление на норму
   F[i] += speedCoef * matrixT[i][j] / minNorma * rightPart[j];
 }

 for (int i = 0; i < numPerem; i++) { //Изменение исходной системы на удобную для итераций
  for (int j = 0; j < numPerem; j++)
   matrix[i][j] = G[i][j];
  rightPart[i] = F[i];
 }
}


int iteration_method(double const matrix[numPerem][numPerem], double const rightPart[numPerem], double XChangable[numPerem], double const epsilon, double const speedCoef = 1) {
 int iterations = 0;
 double B[numPerem];
 double matrixCount[numPerem][numPerem];
 for (int i = 0; i < numPerem; i++) { //Копирование системы
  XChangable[i] = 0;
  B[i] = rightPart[i];
  for (int j = 0; j < numPerem; j++)
   matrixCount[i][j] = matrix[i][j];
 }

 comfortability(matrixCount, B, speedCoef); //Приведение к виду, удобному для итераций
 double accuracy = 1;
 while (accuracy > epsilon) {
  double XStep[numPerem];

  for (int i = 0; i < numPerem; i++) {
   XStep[i] = XChangable[i]; //Сохранение X предыдущего шага
   XChangable[i] = 0;
  }

  for (int i = 0; i < numPerem; i++) { //Подсчёт X этого шага
   for (int j = 0; j < numPerem; j++) {
    XChangable[i] += matrixCount[i][j] * XStep[j];
   }
   XChangable[i] += B[i];
  }

  double max_X = 0;
  for (int i = 0; i < numPerem; i++) { //Вычисление максимальной невязки
   double sum = 0;
   for (int j = 0; j < numPerem; j++) {
    sum += matrix[i][j] * XChangable[j];
   }
   sum -= rightPart[i];
   if (max_X < fabs(sum))
    max_X = fabs(sum);
  }
  accuracy = max_X; 

  iterations++;
 }
 return iterations;
}


int main() {
 setlocale(LC_ALL, "ru");
 const double epsilon = 0.001;
 
 const double matrix[numPerem][numPerem] = {
  {4.5, 4.21, -5.21, 1.23},
  {2.0, 1.87, 0.15, -2.11},
  {0.12, 0.73, 0.31, 0.83},
  {-1.42, 3.41, 1.73, 0.69}
 };
 const double B[numPerem] = { 4.70, 1.84, 2.98, 4.31 };
 double X[numPerem];

 cout << "Данная матрица: " << endl;
 for (int i = 0; i < numPerem; i++) {
  for (int j = 0; j < numPerem; j++)
   cout << setw(5) << matrix[i][j] << ' ';
  cout << " | " << B[i] << endl;
 }
 cout << endl;

 cout << "Количество итераций при значениях ускоряющего коэффициента" << endl;
 for (int alpha = 1; alpha <= 20; alpha++)
  cout << setw(3) << alpha / 10. << ": " << iteration_method(matrix, B, X, epsilon, alpha / 10.) << endl;
 cout << endl;
 
 cout << "Результат:" << endl;
 for (int i = 0; i < numPerem; i++)
  cout << 'X' << i + 1 << " = " << X[i] << endl;
 cout << endl;

 cout << "Невязка:" << endl;
 for (int i = 0; i < numPerem; i++) {
  double result = 0;
  for (int j = 0; j < numPerem; j++)
   result += matrix[i][j] * X[j];
  cout << "|B" << i + 1 << " - AX" << i + 1 << "| = " << result - B[i] << endl;
 }
 cout << endl;

 system("pause");
 return 0;
}