#include <iostream>
#include <ctime>
#include "utils.h"
using namespace std;

template <typename T>
struct MyVector
{
    struct Node
    {
        T value;
        Node *next;
        Node(T v) : value(v), next(nullptr) {}
    };
    Node *head;
    int sz;
    MyVector() : head(nullptr), sz(0) {}
    ~MyVector() { clear(); }

    void push_back(T v)
    {
        Node *n = new Node(v);
        if (!head)
            head = n;
        else
        {
            Node *cur = head;
            while (cur->next)
                cur = cur->next;
            cur->next = n;
        }
        ++sz;
    }

    void clear()
    {
        Node *cur = head;
        while (cur)
        {
            Node *nx = cur->next;
            delete cur;
            cur = nx;
        }
        head = nullptr;
        sz = 0;
    }

    T &operator[](int idx)
    {
        Node *cur = head;
        for (int i = 0; i < idx; ++i)
            cur = cur->next;
        return cur->value;
    }

    int size() { return sz; }
};

struct ListNode
{
    int key;
    int value;
    ListNode *next;
    ListNode(int k, int v) : key(k), value(v), next(nullptr) {}
};

struct HashChaining
{
    struct Bucket
    {
        ListNode *head;
        Bucket() : head(nullptr) {}
    };

    MyVector<Bucket> buckets;
    int capacity;
    int size;

    void init(int cap)
    {
        capacity = nextPrime(cap);
        size = 0;
        for (int i = 0; i < capacity; ++i)
            buckets.push_back(Bucket());
    }

    void clear()
    {
        for (int i = 0; i < capacity; ++i)
        {
            ListNode *cur = buckets[i].head;
            while (cur)
            {
                ListNode *nx = cur->next;
                delete cur;
                cur = nx;
            }
            buckets[i].head = nullptr;
        }
        size = 0;
    }

    int indexOf(int key)
    {
        return key % capacity;
    }

    ListNode *find(int key)
    {
        int idx = indexOf(key);
        ListNode *cur = buckets[idx].head;
        while (cur)
        {
            if (cur->key == key)
                return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    void insert(int key, int value)
    {
        if (find(key))
            return;
        int idx = indexOf(key);
        ListNode *n = new ListNode(key, value);
        n->next = buckets[idx].head;
        buckets[idx].head = n;
        ++size;
    }

    bool remove(int key)
    {
        int idx = indexOf(key);
        ListNode *cur = buckets[idx].head;
        ListNode *prev = nullptr;
        while (cur)
        {
            if (cur->key == key)
            {
                if (prev)
                    prev->next = cur->next;
                else
                    buckets[idx].head = cur->next;
                delete cur;
                --size;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }
};

struct HashOpenAddress
{
    struct Cell
    {
        int key;
        int value;
        unsigned char state; // 0 = пусто, 1 = занято, 2 = удалено
        Cell() : key(0), value(0), state(0) {}
    };

    MyVector<Cell> cells;
    int capacity;
    int size;

    void init(int cap)
    {
        capacity = nextPrime(cap);
        size = 0;
        for (int i = 0; i < capacity; ++i)
            cells.push_back(Cell());
    }

    void clear()
    {
        for (int i = 0; i < capacity; ++i)
        {
            cells[i].key = 0;
            cells[i].value = 0;
            cells[i].state = 0;
        }
        size = 0;
    }

    Cell *find(int key)
    {
        int start = key % capacity;
        for (int k = 0; k < capacity; ++k)
        {
            int i = (start + k) % capacity;
            if (cells[i].state == 0)
                return nullptr;
            if (cells[i].state == 1 && cells[i].key == key)
                return &cells[i];
        }
        return nullptr;
    }

    void insert(int key, int value)
    {
        if (find(key))
            return;

        int start = key % capacity;
        int firstDel = -1;
        for (int k = 0; k < capacity; ++k)
        {
            int i = (start + k) % capacity;

            if (cells[i].state == 1)
            {
                if (cells[i].key == key)
                    return;
            }
            else if (cells[i].state == 2)
            {
                if (firstDel == -1)
                    firstDel = i;
            }
            else
            {
                if (firstDel != -1)
                    i = firstDel;
                cells[i].key = key;
                cells[i].value = value;
                cells[i].state = 1;
                ++size;
                return;
            }
        }
    }

    bool remove(int key)
    {
        int start = key % capacity;
        for (int k = 0; k < capacity; ++k)
        {
            int i = (start + k) % capacity;
            if (cells[i].state == 0)
                return false;
            if (cells[i].state == 1 && cells[i].key == key)
            {
                cells[i].state = 2;
                --size;
                return true;
            }
        }
        return false;
    }
};

void benchmarkSearches(MyVector<int> &data, int *queries, int M)
{
    int capChain = nextPrime(data.size() * 2 + 1);
    int capOpen = nextPrime(data.size() * 3 + 1);

    HashChaining Hc;
    Hc.init(capChain);
    HashOpenAddress Ho;
    Ho.init(capOpen);

    for (int i = 0; i < data.size(); ++i)
    {
        Hc.insert(data[i], data[i]);
        Ho.insert(data[i], data[i]);
    }

    clock_t t1 = clock();
    int hitsC = 0;
    for (int i = 0; i < M; ++i)
        if (Hc.find(queries[i]))
            ++hitsC;
    clock_t t2 = clock();

    clock_t t3 = clock();
    int hitsO = 0;
    for (int i = 0; i < M; ++i)
        if (Ho.find(queries[i]))
            ++hitsO;
    clock_t t4 = clock();

    double timeC = double(t2 - t1) / CLOCKS_PER_SEC;
    double timeO = double(t4 - t3) / CLOCKS_PER_SEC;

    cout << "\n=== Эмпирический анализ поиска (" << M << " запросов) ===\n";
    cout << "Таблица (цепочки):   найдено " << hitsC << ", время " << timeC << " c\n";
    cout << "Таблица (откр.адр.): найдено " << hitsO << ", время " << timeO << " c\n";

    // демонстрация удаления
    cout << "\n=== Проверка удаления ===\n";
    int delKey;
    cout << "Введите ключ для удаления: ";
    cin >> delKey;

    bool r1 = Hc.remove(delKey);
    bool r2 = Ho.remove(delKey);

    cout << "Цепочки: " << (r1 ? "удалён" : "не найден") << endl;
    cout << "Откр.адр.: " << (r2 ? "удалён" : "не найден") << endl;

    cout << "Повторный поиск ключа " << delKey << "...\n";
    cout << "Цепочки: " << (Hc.find(delKey) ? "найден" : "отсутствует") << endl;
    cout << "Откр.адр.: " << (Ho.find(delKey) ? "найден" : "отсутствует") << endl;

    Hc.clear();
    Ho.clear();
}

bool isomorphic(const char *a, const char *b)
{
    int n1 = mystrlen(a), n2 = mystrlen(b);
    if (n1 != n2)
        return false;

    HashChaining mapAB, mapBA;
    mapAB.init(257);
    mapBA.init(257);

    for (int i = 0; i < n1; ++i)
    {
        unsigned char x = (unsigned char)a[i];
        unsigned char y = (unsigned char)b[i];

        ListNode *nAB = mapAB.find(x);
        ListNode *nBA = mapBA.find(y);

        if (!nAB && !nBA)
        {
            mapAB.insert(x, y);
            mapBA.insert(y, x);
        }
        else
        {
            if (!nAB || !nBA || nAB->value != y || nBA->value != x)
                return false;
        }
    }
    return true;
}

int main()
{
    cout << "Выберите режим:\n";
    cout << "1) Эмпирический анализ хеш-таблиц (поиск + удаление)\n";
    cout << "2) Проверка изоморфности двух строк\n";
    cout << "Ваш выбор: ";
    int mode;
    if (!(cin >> mode))
        return 0;

    if (mode == 1)
    {
        int N;
        cout << "Введите N (число элементов): ";
        cin >> N;
        if (N <= 0 || N > 100000)
        {
            cout << "Некорректный N (1..100000).\n";
            return 0;
        }

        MyVector<int> data;
        cout << "Введите " << N << " целых элементов:\n";
        for (int i = 0; i < N; ++i)
        {
            int x;
            cin >> x;
            data.push_back(x);
        }

        int M;
        cout << "Введите M (число поисковых запросов): ";
        cin >> M;
        if (M <= 0 || M > 200000)
            return 0;

        int *q = new int[M];
        cout << "Введите " << M << " значений для поиска:\n";
        for (int i = 0; i < M; ++i)
            cin >> q[i];

        benchmarkSearches(data, q, M);

        delete[] q;
    }
    else if (mode == 2)
    {
        static char A[100005], B[100005];
        cout << "Введите строку A: ";
        cin >> A;
        cout << "Введите строку B: ";
        cin >> B;
        bool ok = isomorphic(A, B);
        cout << (ok ? "Изоморфны\n" : "Не изоморфны\n");
    }
    else
    {
        cout << "Неизвестный режим.\n";
    }
    return 0;
}
