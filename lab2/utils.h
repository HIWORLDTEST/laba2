

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

// Мультипликативный хеш Кнута (32‑бит)
unsigned int hash32(unsigned int x)
{
    return x * 2654435761u;
}
