#pragma once
#include <string>
#include <climits>
using std::string;

int mystrlen(const char *s)
{
    int n = 0;
    while (s[n] != '\0')
        ++n;
    return n;
}

bool isPrime(int x)
{
    if (x < 2)
        return false;
    if (x % 2 == 0)
        return x == 2;
    for (int d = 3; d * d <= x; d += 2)
        if (x % d == 0)
            return false;
    return true;
}

int nextPrime(int x)
{
    if (x <= 2)
        return 2;
    if (x % 2 == 0)
        ++x;
    while (!isPrime(x))
        x += 2;
    return x;
}

string my_substr(const string &s, size_t pos, size_t len)
{
    string result;
    if (pos >= s.size())
        return result;
    if (pos + len > s.size())
        len = s.size() - pos;
    for (size_t i = 0; i < len; ++i)
        result += s[pos + i];
    return result;
}

bool parseInt(const string &s, int &result)
{
    if (s.empty())
        return false;

    int sign = 1;
    size_t i = 0;
    long long val = 0;

    if (s[0] == '-')
    {
        sign = -1;
        i = 1;
    }
    else if (s[0] == '+')
    {
        i = 1;
    }

    for (; i < s.size(); ++i)
    {
        char c = s[i];
        if (c < '0' || c > '9')
            return false;
        val = val * 10 + (c - '0');
        if (val > INT_MAX)
            return false;
    }

    result = static_cast<int>(val * sign);
    return true;
}

int absInt(int x)
{
    return x < 0 ? -x : x;
}