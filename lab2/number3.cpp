#include <iostream>
using namespace std;
// 4 5 10 1 3 7
struct Node
{
    int value;
    Node *next;
    Node(int v) : value(v), next(nullptr) {}
};

struct LinkedList
{
    Node *head;
    int sum;

    LinkedList() : head(nullptr), sum(0) {}

    void append(int v)
    {
        Node *n = new Node(v);
        if (!head)
        {
            head = n;
        }
        else
        {
            Node *cur = head;
            while (cur->next)
                cur = cur->next;
            cur->next = n;
        }
        sum += v;
    }

    void removeLast()
    {
        if (!head)
            return;
        if (!head->next)
        {
            sum -= head->value;
            delete head;
            head = nullptr;
            return;
        }
        Node *cur = head;
        while (cur->next->next)
            cur = cur->next;
        sum -= cur->next->value;
        delete cur->next;
        cur->next = nullptr;
    }

    void print() const
    {
        cout << "{";
        Node *cur = head;
        int first = 1;
        while (cur)
        {
            if (!first)
                cout << ", ";
            cout << cur->value;
            first = 0;
            cur = cur->next;
        }
        cout << "}";
    }

    ~LinkedList()
    {
        while (head)
        {
            Node *tmp = head;
            head = head->next;
            delete tmp;
        }
    }
};

bool dfs(Node *current, LinkedList *buckets, int m, int K)
{
    if (!current)
        return true;

    int x = current->value;

    for (int i = 0; i < m; ++i)
    {
        if (buckets[i].sum + x <= K)
        {
            buckets[i].append(x);

            if (dfs(current->next, buckets, m, K))
                return true;

            buckets[i].removeLast();

            if (buckets[i].sum == 0)
                break;
        }
    }

    return false;
}

void sortList(LinkedList &list)
{
    if (!list.head || !list.head->next)
        return;

    bool swapped;
    do
    {
        swapped = false;
        Node *cur = list.head;
        while (cur->next)
        {
            if (cur->value < cur->next->value)
            {
                int t = cur->value;
                cur->value = cur->next->value;
                cur->next->value = t;
                swapped = true;
            }
            cur = cur->next;
        }
    } while (swapped);
}

int main()
{
    int n;
    cout << "Введите количество элементов множества: ";
    if (!(cin >> n) || n <= 0)
    {
        cout << "Ошибка ввода.\n";
        return 0;
    }

    LinkedList list;
    cout << "Введите элементы множества: ";
    for (int i = 0; i < n; ++i)
    {
        int v;
        cin >> v;
        if (v <= 0)
        {
            cout << "Ошибка: только натуральные числа.\n";
            return 0;
        }
        list.append(v);
    }

    int K;
    cout << "Введите целевую сумму K: ";
    cin >> K;
    if (K <= 0)
    {
        cout << "Ошибка: K должно быть > 0.\n";
        return 0;
    }

    cout << "Исходное множество S = ";
    list.print();
    cout << "\n";

    if (list.sum % K != 0)
    {
        cout << "Ошибка: сумма элементов " << list.sum << " не кратна " << K << ".\n";
        return 0;
    }

    int m = list.sum / K;
    if (m > n)
    {
        cout << "Ошибка: слишком много подмножеств.\n";
        return 0;
    }

    Node *cur = list.head;
    while (cur)
    {
        if (cur->value > K)
        {
            cout << "Ошибка: элемент " << cur->value << " больше K=" << K << ".\n";
            return 0;
        }
        cur = cur->next;
    }

    sortList(list);

    LinkedList *buckets = new LinkedList[m];

    bool ok = dfs(list.head, buckets, m, K);

    if (!ok)
    {
        cout << "Разбиение невозможно.\n";
    }
    else
    {
        cout << "Разбиение найдено (" << m << " подмножеств):\n";
        for (int i = 0; i < m; ++i)
        {
            cout << i + 1 << ") ";
            buckets[i].print();
            cout << "  (сумма = " << buckets[i].sum << ")\n";
        }
    }

    delete[] buckets;
    return 0;
}
