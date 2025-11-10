#include <iostream>
#include <string>
#include <fstream>
#include "utils.h"
using namespace std;
//./number2 --file test.txt --query SET_AT 99
struct HashSet
{
    struct Node
    {
        int key;
        Node *next;
        Node(int k) : key(k), next(nullptr) {}
    };

    Node *head;

    HashSet() : head(nullptr) {}

    ~HashSet()
    {
        Node *current = head;
        while (current)
        {
            Node *tmp = current;
            current = current->next;
            delete tmp;
        }
    }

    bool contains(int x) const
    {
        Node *current = head;
        while (current)
        {
            if (current->key == x)
                return true;
            current = current->next;
        }
        return false;
    }

    void add(int x)
    {
        if (contains(x))
            return;
        Node *newNode = new Node(x);
        newNode->next = head;
        head = newNode;
    }

    void removeKey(int x)
    {
        Node *current = head;
        Node *prev = nullptr;
        while (current)
        {
            if (current->key == x)
            {
                if (prev)
                    prev->next = current->next;
                else
                    head = current->next;
                delete current;
                return;
            }
            prev = current;
            current = current->next;
        }
    }
};

void loadFromFile(HashSet &s, const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
        return;
    int value;
    while (file >> value)
    {
        s.add(value);
    }
    file.close();
}

void saveToFile(const HashSet &s, const string &filename)
{
    ofstream file(filename, ios::trunc);
    if (!file.is_open())
    {
        cerr << "Ошибка: не удалось открыть файл для записи " << filename << endl;
        return;
    }
    HashSet::Node *current = s.head;
    while (current)
    {
        file << current->key << "\n";
        current = current->next;
    }
    file.close();
}

void printUsage()
{
    cerr << "Использование:\n"
         << "./number2 --file <путь> --query <SETADD|SETDEL|SET_AT> [значение]\n";
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        printUsage();
        return 1;
    }

    string filename, query;
    int value = 0;
    bool haveFile = false, haveQuery = false, haveValue = false;

    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg == "--file" && i + 1 < argc)
        {
            filename = argv[++i];
            haveFile = true;
        }
        else if (arg == "--query" && i + 1 < argc)
        {
            query = argv[++i];
            haveQuery = true;
        }
        else
        {
            int temp;
            if (parseInt(arg, temp))
            {
                value = temp;
                haveValue = true;
            }
        }
    }

    if (!haveFile || !haveQuery)
    {
        printUsage();
        return 1;
    }

    HashSet mySet;
    loadFromFile(mySet, filename);

    if (query == "SETADD")
    {
        if (!haveValue)
        {
            cerr << "Ошибка: не указано значение для SETADD\n";
            return 1;
        }
        mySet.add(value);
        saveToFile(mySet, filename);
    }
    else if (query == "SETDEL")
    {
        if (!haveValue)
        {
            cerr << "Ошибка: не указано значение для SETDEL\n";
            return 1;
        }
        mySet.removeKey(value);
        saveToFile(mySet, filename);
    }
    else if (query == "SET_AT")
    {
        if (!haveValue)
        {
            cerr << "Ошибка: не указано значение для SET_AT\n";
            return 1;
        }
        cout << (mySet.contains(value) ? "YES\n" : "NO\n");
    }
    else
    {
        cerr << "Неизвестная операция: " << query << endl;
        return 1;
    }

    return 0;
}
