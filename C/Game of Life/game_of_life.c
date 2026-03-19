#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>

#define STR 25
#define COL 80

// 1 - Функция ввода матрцицы
void input(int matr[STR][COL], int file_number);

// 2 - Функция итерации по матрице и отбора исследуемой точки
void update(int matr1[STR][COL], int matr2[STR][COL]);

// 3 - Функция подсчета соседей
int neighbors(int matr1[STR][COL], int i, int j);

// 4 - Функция принятия решения о следующем поколении
int decision(int neighbors, int condition);

// 5 - Функция для переприсваивания матриц
void replace(int matr2[STR][COL], int matr1[STR][COL]);

// 6 - Функция проверяющая статичность поля
int check(int matr1[STR][COL], int matr2[STR][COL]);

// 7 - Функция смены скорости
int speed(char control_button, int *flag, int *time_mili_sec);

// 8 - Функция на случай пустого поля
int count(int matr[STR][COL]);

// 9 - Функция отображения меню
int show_menu();

// 10 - Функция ручного ввода поля (без пробелов)
void manual_input(int matr[STR][COL]);

// Дополнительные функции для рефакторинга
int is_valid_line(char *line, int *len);
void fill_matrix_row(int matr[STR][COL], int row, char *line, int input_len);
void print_manual_header();
int process_file_line(char *line, int row, int matr[STR][COL]);
void fill_remaining_rows(int matr[STR][COL], int start_row);

// MAIN
// ---------------------------------------
int main()
{
    int matr1[STR][COL] = {0};
    int matr2[STR][COL] = {0};
    int time_mili_sec = 500;
    int stop = 0;

    int choice = show_menu();

    if (choice == 0)
    {
        return 0;
    }
    else if (choice >= 1 && choice <= 5)
    {
        input(matr1, choice);
    }
    else if (choice == 6)
    {
        manual_input(matr1);
    }

    initscr();
    freopen("/dev/tty", "r", stdin);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, true);
    curs_set(0);
    typeahead(-1);

    while (stop != 1)
    {
        char control_button = getch();

        if (control_button != ERR)
        {
            if (count(matr1) == 0)
            {
                stop = 1;
                break;
            }

            speed(control_button, &stop, &time_mili_sec);

            flushinp();
        }

        napms(time_mili_sec);

        clear();
        update(matr1, matr2);

        if (check(matr1, matr2) == STR * COL)
        {
            stop = 1;
        }
        replace(matr2, matr1);
    }

    endwin();
    return 0;
}
// ---------------------------------------

int show_menu()
{
    int choice = 0;

    printf("====================================\n");
    printf("         ИГРА 'ЖИЗНЬ'\n");
    printf("====================================\n\n");
    printf("Выберите начальную конфигурацию:\n\n");
    printf("1 - Конфигурация из файла 1\n");
    printf("2 - Конфигурация из файла 2\n");
    printf("3 - Конфигурация из файла 3\n");
    printf("4 - Конфигурация из файла 4\n");
    printf("5 - Конфигурация из файла 5\n");
    printf("6 - Ввести поле вручную (без пробелов)\n");
    printf("0 - Выход\n\n");
    printf("Ваш выбор: ");

    scanf("%d", &choice);

    while (getchar() != '\n')
        ;

    return choice;
}

void print_manual_header()
{
    printf("\n====================================\n");
    printf("     РУЧНОЙ ВВОД ПОЛЯ\n");
    printf("====================================\n");
    printf("Введите 25 строк из 0 и 1 (без пробелов)\n");
    printf("Правила ввода:\n");
    printf("- Если введено меньше 80 символов, оставшиеся заполняются 0\n");
    printf("- Если введено больше 80 символов, берутся первые 80\n");
    printf("- Разрешены только символы 0 и 1\n\n");
}

int is_valid_line(char *line, int *len)
{
    *len = 0;
    while (line[*len] != '\n' && line[*len] != '\0')
    {
        (*len)++;
    }

    int check_len = (*len < COL) ? *len : COL;
    for (int j = 0; j < check_len; j++)
    {
        if (line[j] != '0' && line[j] != '1')
        {
            printf("Ошибка: символ '%c' не является 0 или 1 (позиция %d)\n", line[j], j + 1);
            printf("Попробуйте еще раз.\n");
            return 0;
        }
    }
    return 1;
}

void fill_matrix_row(int matr[STR][COL], int row, char *line, int input_len)
{
    for (int j = 0; j < COL; j++)
    {
        matr[row][j] = (j < input_len) ? (line[j] - '0') : 0;
    }

    if (input_len < COL)
        printf("  (Введено %d символов, остальные %d заполнены 0)\n", input_len, COL - input_len);
    else if (input_len > COL)
        printf("  (Введено %d символов, использованы первые %d)\n", input_len, COL);
    else
        printf("  (Введено ровно 80 символов)\n");
}

void manual_input(int matr[STR][COL])
{
    print_manual_header();

    for (int i = 0; i < STR; i++)
    {
        char line[256];
        int valid = 0;
        int input_len = 0;

        while (!valid)
        {
            printf("Строка %2d: ", i + 1);
            fgets(line, sizeof(line), stdin);
            valid = is_valid_line(line, &input_len);
        }

        fill_matrix_row(matr, i, line, input_len);
    }

    printf("\nПоле успешно введено!\n");
}

int process_file_line(char *line, int row, int matr[STR][COL])
{
    int j = 0;
    while (line[j] == ' ' || line[j] == '\t' || line[j] == '\n')
        j++;

    if (line[j] == '\0' || line[j] == '\n')
        return 0;

    int input_len = 0;
    char digits[COL + 1];

    for (int k = 0; line[k] != '\n' && line[k] != '\0' && input_len < COL; k++)
    {
        if (line[k] == '0' || line[k] == '1')
            digits[input_len++] = line[k];
    }

    for (int col = 0; col < COL; col++)
        matr[row][col] = (col < input_len) ? (digits[col] - '0') : 0;

    return 1;
}

void fill_remaining_rows(int matr[STR][COL], int start_row)
{
    for (int i = start_row; i < STR; i++)
        for (int j = 0; j < COL; j++)
            matr[i][j] = 0;
}

void input(int matr[STR][COL], int file_number)
{
    char filename[20];
    sprintf(filename, "input%d.txt", file_number);

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Файл %s не найден. Поле будет заполнено нулями.\n", filename);
        fill_remaining_rows(matr, 0);
        return;
    }

    char line[256];
    int i = 0;

    while (i < STR && fgets(line, sizeof(line), file))
    {
        if (process_file_line(line, i, matr))
            i++;
    }

    fill_remaining_rows(matr, i);
    fclose(file);
    printf("Файл %s успешно загружен!\n", filename);
}

void update(int matr1[STR][COL], int matr2[STR][COL])
{
    for (int i = 0; i < STR; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            matr2[i][j] = decision(neighbors(matr1, i, j), matr1[i][j]);
            if (matr2[i][j] == 1)
                printw("0");
            else
                printw("-");
        }
        printw("\n");
    }
}

int neighbors(int matr1[STR][COL], int i, int j)
{
    int sum = 0;
    int i_minus = i - 1, j_minus = j - 1, i_plus = i + 1, j_plus = j + 1;

    if (i_minus < 0)
        i_minus = STR - 1;
    if (j_minus < 0)
        j_minus = COL - 1;
    if (i_plus > STR - 1)
        i_plus = i_plus % STR;
    if (j_plus > COL - 1)
        j_plus = j_plus % COL;

    sum += matr1[i_minus][j_minus];
    sum += matr1[i_minus][j];
    sum += matr1[i_minus][j_plus];
    sum += matr1[i][j_plus];
    sum += matr1[i_plus][j_plus];
    sum += matr1[i_plus][j];
    sum += matr1[i_plus][j_minus];
    sum += matr1[i][j_minus];

    return sum;
}

int decision(int neighbors, int condition)
{
    int next_gen = -1;
    if ((neighbors == 2 || neighbors == 3) && condition == 1)
    {
        next_gen = 1;
    }
    else if (neighbors == 3 && condition == 0)
    {
        next_gen = 1;
    }
    else
    {
        next_gen = 0;
    }
    return next_gen;
}

void replace(int matr2[STR][COL], int matr1[STR][COL])
{
    for (int i = 0; i < STR; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            matr1[i][j] = matr2[i][j];
        }
    }
}

int check(int matr1[STR][COL], int matr2[STR][COL])
{
    int ans = 0;
    for (int i = 0; i < STR; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            if (matr1[i][j] == matr2[i][j])
                ans++;
        }
    }
    return ans;
}

int speed(char control_button, int *flag, int *time_mili_sec)
{
    const int step = 200;
    const int min_delay = 100;
    const int max_delay = 2000;

    switch (control_button)
    {
    case 'a':
    case 'A':
        if (*time_mili_sec - step >= min_delay)
            *time_mili_sec -= step;
        break;
    case 'z':
    case 'Z':
        if (*time_mili_sec + step <= max_delay)
            *time_mili_sec += step;
        break;
    case ' ':
        *flag = 1;
        break;
    }

    return *time_mili_sec;
}

int count(int matr[STR][COL])
{
    int sum = 0;
    for (int i = 0; i < STR; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            sum += matr[i][j];
        }
    }
    return sum;
}