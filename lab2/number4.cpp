#include <iostream>
using namespace std;

template <typename T>
struct ArrayList
{
    T *data;
    int size;
    int capacity;

    ArrayList(int cap = 2) : size(0), capacity(cap)
    {
        data = new T[capacity];
    }

    ~ArrayList()
    {
        delete[] data;
    }

    void append(T v)
    {
        if (size >= capacity)
        {
            // увеличиваем размер массива в 2 раза
            capacity *= 2;
            T *newData = new T[capacity];
            for (int i = 0; i < size; ++i)
                newData[i] = data[i];
            delete[] data;
            data = newData;
        }
        data[size++] = v;
    }

    void removeLast()
    {
        if (size > 0)
            --size;
    }

    void print() const
    {
        cout << "{";
        for (int i = 0; i < size; ++i)
        {
            if (i > 0)
                cout << ", ";
            cout << data[i];
        }
        cout << "}";
    }
};

template <typename T>
void generateSubsets(const T input[], int n, int index, ArrayList<T> &subset)
{
    if (index == n)
    {
        subset.print();
        cout << "\n";
        return;
    }

    // не включаем элемент
    generateSubsets(input, n, index + 1, subset);

    // включаем элемент
    subset.append(input[index]);
    generateSubsets(input, n, index + 1, subset);
    subset.removeLast();
}

int main()
{
    int n;
    cout << "Введите количество элементов множества: ";
    cin >> n;
    if (n <= 0)
    {
        cout << "Ошибка: некорректное количество.\n";
        return 0;
    }

    cout << "Выберите тип элементов (1 — числа, 2 — строки): ";
    int type;
    cin >> type;

    if (type == 1)
    {
        int *arr = new int[n];
        cout << "Введите элементы множества: ";
        for (int i = 0; i < n; ++i)
            cin >> arr[i];

        ArrayList<int> subset;
        cout << "\nВсе различные подмножества:\n";
        generateSubsets(arr, n, 0, subset);

        delete[] arr;
    }
    else if (type == 2)
    {
        string *arr = new string[n];
        cout << "Введите элементы множества (строки): ";
        for (int i = 0; i < n; ++i)
            cin >> arr[i];

        ArrayList<string> subset;
        cout << "\nВсе различные подмножества:\n";
        generateSubsets(arr, n, 0, subset);

        delete[] arr;
    }
    else
    {
        cout << "Ошибка: неверный тип.\n";
    }

    return 0;
}
