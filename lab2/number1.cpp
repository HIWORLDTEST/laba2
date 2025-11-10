#include <iostream>
#include <string>
#include "utils.h"
using namespace std;
// <a><b></b><c><\c></a><a></a>
template <typename T>
struct StackNode
{
    T value;
    StackNode *next;
};

template <typename T>
struct Stack
{
    StackNode<T> *head;

    Stack() : head(nullptr) {}

    ~Stack()
    {
        while (head)
        {
            pop();
        }
    }

    bool empty() const
    {
        return head == nullptr;
    }

    void push(const T &val)
    {
        StackNode<T> *node = new StackNode<T>;
        node->value = val;
        node->next = head;
        head = node;
    }

    const T &top() const
    {
        return head->value;
    }

    void pop()
    {
        if (!head)
            return;
        StackNode<T> *tmp = head;
        head = head->next;
        delete tmp;
    }
};

bool isValidXML(const string &s)
{
    int n = (int)s.size();

    if (n == 0)
        return true;
    if (s[0] != '<' || s[n - 1] != '>')
        return false;

    Stack<string> tags;
    Stack<char> brackets;

    int i = 0;

    while (i < n)
    {
        if (s[i] != '<')
            return false;
        brackets.push('<');

        int j = i + 1;
        bool closing = false;

        if (j < n && s[j] == '/')
        {
            closing = true;
            ++j;
        }

        int nameStart = j;

        while (j < n && s[j] >= 'a' && s[j] <= 'z')
        {
            ++j;
        }

        if (nameStart == j) // пустое имя
            return false;
        if (j >= n || s[j] != '>')
            return false;

        if (brackets.empty() || brackets.top() != '<')
            return false;
        brackets.pop();

        string tag = my_substr(s, nameStart, j - nameStart); // получаем имя

        if (closing)
        {
            if (tags.empty() || tags.top() != tag)
            {
                return false;
            }
            tags.pop();
        }
        else
        {
            tags.push(tag);
        }

        i = j + 1;
    }

    return tags.empty() && brackets.empty();
}

int main()
{
    string input;
    if (!getline(cin, input))
    {
        return 0;
    }

    const string symbols = "abcdefghijklmnopqrstuvwxyz<>/";
    string best;
    bool found = false;

    for (size_t i = 0; i < input.size(); ++i)
    {
        char original = input[i];

        for (size_t j = 0; j < symbols.size(); ++j)
        {
            char c = symbols[j];

            if (c == original)
            {
                continue;
            }

            string candidate = input;
            candidate[i] = c;

            if (isValidXML(candidate))
            {
                if (!found || candidate < best)
                {
                    best = candidate;
                    found = true;
                }
            }
        }
    }

    if (found)
    {
        cout << best << '\n';
    }
    else
    {
        cout << "нет решения\n";
    }

    return 0;
}
