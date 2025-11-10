#include <iostream>
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

    // const версия
    const T &operator[](int idx) const
    {
        Node *cur = head;
        for (int i = 0; i < idx; ++i)
            cur = cur->next;
        return cur->value;
    }

    int size() { return sz; }
};

struct Node
{
    int key, val;
    Node *prev;
    Node *next;
    Node(int k, int v) : key(k), val(v), prev(nullptr), next(nullptr) {}
};

struct LRUList
{
    Node *head = nullptr;
    Node *tail = nullptr;

    void push_front(Node *n)
    {
        n->prev = nullptr;
        n->next = head;
        if (head)
            head->prev = n;
        head = n;
        if (!tail)
            tail = n;
    }

    void remove(Node *n)
    {
        if (n->prev)
            n->prev->next = n->next;
        else
            head = n->next;
        if (n->next)
            n->next->prev = n->prev;
        else
            tail = n->prev;
        n->prev = n->next = nullptr;
    }

    void move_to_front(Node *n)
    {
        if (head == n)
            return;
        remove(n);
        push_front(n);
    }

    Node *pop_back()
    {
        Node *r = tail;
        if (!r)
            return nullptr;
        remove(r);
        return r;
    }
};

struct PtrHashMap
{
    struct Cell
    {
        int key;
        Node *val;
        unsigned char state;
        Cell() : key(0), val(nullptr), state(0) {}
    };

    MyVector<Cell> cells;
    int cap;
    int size;

    void init(int expect)
    {
        cap = nextPrime(expect);
        size = 0;
        for (int i = 0; i < cap; ++i)
            cells.push_back(Cell());
    }

    void clear()
    {
        cells.clear();
        cap = 0;
        size = 0;
    }

    int findIndex(int key) const
    {
        int start = key % cap;
        for (int k = 0; k < cap; ++k)
        {
            int i = (start + k) % cap;
            if (cells[i].state == 0)
                return -1;
            if (cells[i].state == 1 && cells[i].key == key)
                return i;
        }
        return -1;
    }

    Node *get(int key) const
    {
        int i = findIndex(key);
        return (i == -1) ? nullptr : cells[i].val;
    }

    void put(int key, Node *val)
    {
        int start = key % cap;
        int firstDel = -1;
        for (int k = 0; k < cap; ++k)
        {
            int i = (start + k) % cap;
            if (cells[i].state == 1)
            {
                if (cells[i].key == key)
                {
                    cells[i].val = val;
                    return;
                }
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
                cells[i].val = val;
                cells[i].state = 1;
                ++size;
                return;
            }
        }
    }

    void erase(int key)
    {
        int i = findIndex(key);
        if (i != -1)
        {
            cells[i].state = 2;
            cells[i].val = nullptr;
        }
    }
};

struct LRUCache
{
    int capacity;
    int curSize;
    LRUList order;
    PtrHashMap map;

    void init(int cap)
    {
        capacity = (cap < 0) ? 0 : cap;
        curSize = 0;
        int mapCap = (capacity > 0) ? (capacity * 4 + 7) : 11;
        map.init(mapCap);
    }

    void clear()
    {
        Node *c = order.head;
        while (c)
        {
            Node *nx = c->next;
            delete c;
            c = nx;
        }
        order.head = order.tail = nullptr;
        map.clear();
        capacity = 0;
        curSize = 0;
    }

    int get(int key)
    {
        if (capacity <= 0)
            return -1;
        Node *n = map.get(key);
        if (!n)
            return -1;
        order.move_to_front(n);
        return n->val;
    }

    void set(int key, int value)
    {
        if (capacity <= 0)
            return;
        Node *n = map.get(key);
        if (n)
        {
            n->val = value;
            order.move_to_front(n);
            return;
        }

        Node *m = new Node(key, value);
        order.push_front(m);
        map.put(key, m);
        ++curSize;

        if (curSize > capacity)
        {
            Node *old = order.pop_back();
            map.erase(old->key);
            delete old;
            --curSize;
        }
    }
};

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int cap, Q;
    if (!(cin >> cap >> Q))
        return 0;

    LRUCache cache;
    cache.init(cap);

    bool firstOut = true;

    for (int i = 0; i < Q; ++i)
    {
        char cmd[8];
        cin >> cmd;
        if (cmd[0] == 'S')
        {
            int x, y;
            cin >> x >> y;
            cache.set(x, y);
        }
        else if (cmd[0] == 'G')
        {
            int x;
            cin >> x;
            int res = cache.get(x);
            if (!firstOut)
                cout << ' ';
            cout << res;
            firstOut = false;
        }
    }

    if (!firstOut)
        cout << "\n";

    cache.clear();
    return 0;
}
