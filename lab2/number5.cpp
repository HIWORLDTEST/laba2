#include <iostream>
#include "utils.h"
using namespace std;

// -------------------------
// Узел дерева Хаффмана
// -------------------------
struct HNode
{
    unsigned char ch; // символ (для листа)
    int freq;         // частота
    HNode *left;
    HNode *right;
    bool isLeaf;
};

// Создать лист
HNode *makeLeaf(unsigned char c, int f)
{
    HNode *n = new HNode;
    n->ch = c;
    n->freq = f;
    n->left = n->right = 0;
    n->isLeaf = true;
    return n;
}

// Создать внутренний узел
HNode *makeParent(HNode *a, HNode *b)
{
    HNode *n = new HNode;
    n->ch = 0;
    n->freq = a->freq + b->freq;
    n->left = a;  // по заданию: left -> 0
    n->right = b; // right -> 1
    n->isLeaf = false;
    return n;
}

// Освобождение дерева
void freeTree(HNode *t)
{
    if (!t)
        return;
    freeTree(t->left);
    freeTree(t->right);
    delete t;
}

// Длина C-строки: вынесена в utils.h (mystrlen)

// -------------------------
// Поиск двух минимальных по freq узлов в массиве nodes[0..n-1]
// -------------------------
void pickTwoMin(HNode **nodes, int n, int &i1, int &i2)
{
    i1 = -1;
    i2 = -1;
    for (int i = 0; i < n; ++i)
    {
        if (i1 == -1 || nodes[i]->freq < nodes[i1]->freq ||
            (nodes[i]->freq == nodes[i1]->freq && i > i1))
        {
            i2 = i1;
            i1 = i;
        }
        else if (i2 == -1 || nodes[i]->freq < nodes[i2]->freq ||
                 (nodes[i]->freq == nodes[i2]->freq && i > i2))
        {
            if (i != i1)
                i2 = i;
        }
    }
    // упорядочим, чтобы i1 > i2 — так удобнее «выкидывать» элементы
    if (i1 < i2)
    {
        int t = i1;
        i1 = i2;
        i2 = t;
    }
}

// -------------------------
// Построение дерева Хаффмана
// -------------------------
HNode *buildHuffman(const int freq[256], int &outUnique)
{
    HNode *nodes[256];
    int n = 0;
    for (int c = 0; c < 256; ++c)
    {
        if (freq[c] > 0)
        {
            nodes[n++] = makeLeaf((unsigned char)c, freq[c]);
        }
    }
    outUnique = n;
    if (n == 0)
        return 0;
    if (n == 1)
        return nodes[0]; // один символ — дерево из одного листа

    while (n > 1)
    {
        int i1, i2;
        pickTwoMin(nodes, n, i1, i2);
        HNode *parent = makeParent(nodes[i2], nodes[i1]);
        nodes[i1] = parent;
        nodes[i2] = nodes[n - 1];
        --n;
    }
    return nodes[0];
}

// -------------------------
// Таблица кодов и построение кодов (тот же код)
// -------------------------
void buildCodesDfs(HNode *t, char path[], int depth,
                   char codes[256][256], int len[256])
{
    if (!t)
        return;
    if (t->isLeaf)
    {
        if (depth == 0)
        {
            path[0] = '0';
            depth = 1;
        }
        for (int i = 0; i < depth; ++i)
            codes[t->ch][i] = path[i];
        codes[t->ch][depth] = '\0';
        len[t->ch] = depth;
        return;
    }
    path[depth] = '0';
    buildCodesDfs(t->left, path, depth + 1, codes, len);
    path[depth] = '1';
    buildCodesDfs(t->right, path, depth + 1, codes, len);
}

void buildCodes(HNode *root, char codes[256][256], int len[256])
{
    for (int i = 0; i < 256; ++i)
    {
        codes[i][0] = '\0';
        len[i] = 0;
    }
    char path[256];
    buildCodesDfs(root, path, 0, codes, len);
}

// -------------------------
// Кодирование
// -------------------------
int encode(const char *s, const char codes[256][256], char *encoded)
{
    int pos = 0;
    for (int i = 0; s[i]; ++i)
    {
        const char *z = codes[(unsigned char)s[i]];
        for (int j = 0; z[j]; ++j)
        {
            encoded[pos++] = z[j];
        }
    }
    encoded[pos] = '\0';
    return pos;
}

// -------------------------
// Декодирование
// -------------------------
void decode(const char *encoded, HNode *root, char *out)
{
    int pos = 0;
    if (!root)
    {
        out[0] = '\0';
        return;
    }

    if (root->isLeaf)
    {
        int L = mystrlen(encoded);
        for (int i = 0; i < L; ++i)
            out[pos++] = root->ch;
        out[pos] = '\0';
        return;
    }

    HNode *cur = root;
    for (int i = 0; encoded[i]; ++i)
    {
        if (encoded[i] == '0')
            cur = cur->left;
        else
            cur = cur->right;

        if (cur->isLeaf)
        {
            out[pos++] = cur->ch;
            cur = root;
        }
    }
    out[pos] = '\0';
}

// -------------------------
// Печать таблицы (символ -> код)
// -------------------------
void printTable(const int freq[256], const char codes[256][256])
{
    cout << "Таблица кодов (символ : код):\n";
    for (int c = 0; c < 256; ++c)
    {
        if (freq[c] > 0)
        {
            unsigned char uc = (unsigned char)c;
            if (uc >= 32 && uc < 127)
            {
                cout << "  " << uc << " : " << codes[c] << "\n";
            }
            else
            {
                cout << "  [#" << c << "] : " << codes[c] << "\n";
            }
        }
    }
}

// -------------------------
// MAIN
// -------------------------
int main()
{
    static char input[100005];
    cout << "Введите исходную строку (без пробелов): ";
    cin >> input;

    int freq[256];
    for (int i = 0; i < 256; ++i)
        freq[i] = 0;
    for (int i = 0; input[i]; ++i)
        ++freq[(unsigned char)input[i]];

    int uniq = 0;
    HNode *root = buildHuffman(freq, uniq);
    if (!root)
    {
        cout << "Пустая строка.\n";
        return 0;
    }

    static char codes[256][256];
    int codeLen[256];
    buildCodes(root, codes, codeLen);

    static char encoded[200005];
    int encLen = encode(input, codes, encoded);

    static char decoded[100005];
    decode(encoded, root, decoded);

    cout << "\nКод Хаффмана: " << encoded << "\n\n";
    printTable(freq, codes);
    cout << "\nВосстановленная строка: " << decoded << "\n";

    if (mystrlen(input) == mystrlen(decoded))
    {
        int ok = 1;
        for (int i = 0; input[i]; ++i)
            if (input[i] != decoded[i])
            {
                ok = 0;
                break;
            }
        cout << (ok ? "Проверка: OK\n" : "Проверка: НЕ СОВПАЛО\n");
    }

    freeTree(root);
    return 0;
}