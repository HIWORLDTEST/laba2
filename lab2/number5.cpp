#include <iostream>
using namespace std;

struct HNode // узел дерева
{
    unsigned char ch;
    int freq;
    HNode *left;
    HNode *right;
    bool isLeaf;
};

struct NodeList
{
    HNode *node;
    NodeList *next;
};

struct BitNode
{
    char bit;
    BitNode *next;
};

struct CodeTable
{
    unsigned char ch;
    BitNode *code;
    CodeTable *next;
};

HNode *makeLeaf(unsigned char c, int f)
{
    HNode *n = new HNode;
    n->ch = c;
    n->freq = f;
    n->left = n->right = nullptr;
    n->isLeaf = true;
    return n;
}

HNode *makeParent(HNode *a, HNode *b)
{
    HNode *n = new HNode;
    n->ch = 0;
    n->freq = a->freq + b->freq;
    n->left = a;
    n->right = b;
    n->isLeaf = false;
    return n;
}

void freeTree(HNode *t)
{
    if (!t)
        return;
    freeTree(t->left);
    freeTree(t->right);
    delete t;
}

void pushNode(NodeList *&head, HNode *node)
{
    NodeList *n = new NodeList;
    n->node = node;
    n->next = head;
    head = n;
}

void removeNode(NodeList *&head, NodeList *prev, NodeList *cur)
{
    if (!prev)
        head = cur->next;
    else
        prev->next = cur->next;
    delete cur;
}

void pickTwoMin(NodeList *head, HNode *&min1, HNode *&min2, NodeList *&prev1, NodeList *&prev2)
{
    min1 = min2 = nullptr;
    prev1 = prev2 = nullptr;
    NodeList *cur = head, *prev = nullptr;
    while (cur)
    {
        if (!min1 || cur->node->freq < min1->freq)
        {
            min2 = min1;
            prev2 = prev1;
            min1 = cur->node;
            prev1 = prev;
        }
        else if (!min2 || cur->node->freq < min2->freq)
        {
            min2 = cur->node;
            prev2 = prev;
        }
        prev = cur;
        cur = cur->next;
    }
}

HNode *buildHuffman(const int freq[256])
{
    NodeList *list = nullptr;
    for (int c = 0; c < 256; ++c)
    {
        if (freq[c] > 0)
            pushNode(list, makeLeaf((unsigned char)c, freq[c]));
    }

    if (!list)
        return nullptr;
    if (!list->next)
        return list->node;

    while (list && list->next)
    {
        HNode *n1, *n2;
        NodeList *p1, *p2;
        pickTwoMin(list, n1, n2, p1, p2);
        removeNode(list, p1, p1 ? p1->next : list);
        removeNode(list, p2, p2 ? p2->next : list);
        HNode *parent = makeParent(n1, n2);
        pushNode(list, parent);
    }

    HNode *root = list->node;
    delete list;
    return root;
}

void buildCodesDfs(HNode *t, BitNode *path, CodeTable *&table)
{
    if (!t)
        return;
    if (t->isLeaf)
    {
        CodeTable *entry = new CodeTable;
        entry->ch = t->ch;
        entry->code = nullptr;

        BitNode *curPath = path, *last = nullptr;
        while (curPath)
        {
            BitNode *b = new BitNode{curPath->bit, nullptr};
            if (!entry->code)
                entry->code = b;
            else
                last->next = b;
            last = b;
            curPath = curPath->next;
        }
        entry->next = table;
        table = entry;
        return;
    }

    BitNode leftBit{'0', path};
    buildCodesDfs(t->left, &leftBit, table);

    BitNode rightBit{'1', path};
    buildCodesDfs(t->right, &rightBit, table);
}

BitNode *getCode(CodeTable *table, unsigned char c)
{
    while (table)
    {
        if (table->ch == c)
            return table->code;
        table = table->next;
    }
    return nullptr;
}

BitNode *encode(const char *s, CodeTable *table)
{
    BitNode *head = nullptr, *tail = nullptr;
    for (int i = 0; s[i]; ++i)
    {
        BitNode *code = getCode(table, (unsigned char)s[i]);
        BitNode *cur = code;
        while (cur)
        {
            BitNode *b = new BitNode{cur->bit, nullptr};
            if (!head)
                head = tail = b;
            else
                tail->next = b, tail = b;
            cur = cur->next;
        }
    }
    return head;
}

void decode(BitNode *encoded, HNode *root, string &out)
{
    if (!root)
        return;
    if (root->isLeaf)
    {
        while (encoded)
        {
            out.push_back(root->ch);
            encoded = encoded->next;
        }
        return;
    }

    HNode *cur = root;
    while (encoded)
    {
        if (encoded->bit == '0')
            cur = cur->left;
        else
            cur = cur->right;

        if (cur->isLeaf)
        {
            out.push_back(cur->ch);
            cur = root;
        }
        encoded = encoded->next;
    }
}

void printCode(BitNode *code)
{
    while (code)
    {
        cout << code->bit;
        code = code->next;
    }
}

void printTable(CodeTable *table)
{
    cout << "\nТаблица кодов (символ : код):\n";
    while (table)
    {
        if (table->ch >= 32 && table->ch < 127)
            cout << "  " << table->ch << " : ";
        else
            cout << "  [#" << (int)table->ch << "] : ";
        printCode(table->code);
        cout << "\n";
        table = table->next;
    }
}

int main()
{
    cout << "Введите исходную строку (без пробелов): ";
    string input;
    cin >> input;

    int freq[256] = {};
    for (char c : input)
        freq[(unsigned char)c]++;

    HNode *root = buildHuffman(freq);

    CodeTable *table = nullptr;
    buildCodesDfs(root, nullptr, table);

    BitNode *encoded = encode(input.c_str(), table);

    string decoded;
    decode(encoded, root, decoded);

    cout << "\nКод Хаффмана: ";
    printCode(encoded);
    cout << "\n";

    printTable(table);

    cout << "\nВосстановленная строка: " << decoded << "\n";
    cout << (input == decoded ? "Проверка: OK\n" : "Проверка: НЕ СОВПАЛО\n");

    freeTree(root);
    return 0;
}
