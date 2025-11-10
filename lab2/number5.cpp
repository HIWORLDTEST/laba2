#include <iostream>
#include <string>

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

void removeNodeByPtr(NodeList *&head, NodeList *target)
{
    if (!head || !target)
        return;
    if (head == target)
    {
        head = head->next;
        delete target;
        return;
    }
    NodeList *cur = head;
    while (cur->next && cur->next != target)
        cur = cur->next;
    if (cur->next == target)
    {
        cur->next = target->next;
        delete target;
    }
}

void pickTwoMin(NodeList *head, NodeList *&min1, NodeList *&min2)
{
    min1 = min2 = nullptr;
    NodeList *cur = head;
    while (cur)
    {
        if (!min1 || cur->node->freq < min1->node->freq)
        {
            min2 = min1;
            min1 = cur;
        }
        else if (!min2 || cur->node->freq < min2->node->freq)
        {
            min2 = cur;
        }
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
    {
        HNode *single = list->node;
        delete list;
        return single;
    }

    while (list && list->next)
    {
        NodeList *min1, *min2;
        pickTwoMin(list, min1, min2);

        if (!min1 || !min2)
            break;

        HNode *n1 = min1->node;
        HNode *n2 = min2->node;

        removeNodeByPtr(list, min1);
        removeNodeByPtr(list, min2);

        HNode *parent = makeParent(n1, n2);
        pushNode(list, parent);
    }

    HNode *root = nullptr;
    if (list)
    {
        root = list->node;
        delete list;
    }
    return root;
}

BitNode *makeBitListFromString(const string &path)
{
    BitNode *head = nullptr, *tail = nullptr;
    for (char c : path)
    {
        BitNode *b = new BitNode{c, nullptr};
        if (!head)
            head = tail = b;
        else
        {
            tail->next = b;
            tail = b;
        }
    }
    return head;
}

void buildCodesDfs(HNode *t, string &path, CodeTable *&table)
{
    if (!t)
        return;
    if (t->isLeaf)
    {
        CodeTable *entry = new CodeTable;
        entry->ch = t->ch;
        entry->code = makeBitListFromString(path);
        entry->next = table;
        table = entry;
        return;
    }
    // идти влево с добавлением '0'
    path.push_back('0');
    buildCodesDfs(t->left, path, table);
    path.pop_back();

    // идти вправо с добавлением '1'
    path.push_back('1');
    buildCodesDfs(t->right, path, table);
    path.pop_back();
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
            {
                tail->next = b;
                tail = b;
            }
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

        if (!cur) // защитная проверка
            return;

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

void freeBitList(BitNode *b)
{
    while (b)
    {
        BitNode *nx = b->next;
        delete b;
        b = nx;
    }
}

void freeCodeTable(CodeTable *table)
{
    while (table)
    {
        CodeTable *nx = table->next;
        freeBitList(table->code);
        delete table;
        table = nx;
    }
}

int main()
{
    cout << "Введите исходную строку (без пробелов): ";
    string input;
    cin >> input;

    int freq[256] = {};
    for (unsigned char c : input)
        freq[c]++;

    HNode *root = buildHuffman(freq);

    CodeTable *table = nullptr;
    string path;
    buildCodesDfs(root, path, table);

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
    freeCodeTable(table);
    freeBitList(encoded);

    return 0;
}
