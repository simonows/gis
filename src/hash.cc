#include <gis/hash.h>

/*
template <typename K, typename V, typename F>
HashMap<K, V, F>::HashMap()
{
    table = new HashNode<K, V> *[TABLE_SIZE]();
}


template <typename K, typename V, typename F>
HashMap<K, V, F>::~HashMap()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        HashNode<K, V> *entry = table[i];
        while (entry != nullptr)
        {
            HashNode<K, V> *prev = entry;
            entry = entry->getNext();
            delete prev;
        }
        table[i] = nullptr;
    }

    delete [] table;
}


template <typename K, typename V, typename F>
bool HashMap<K, V, F>::get(const K &key, V &value)
{
    unsigned long hashValue = hashFunc(key);
    HashNode<K, V> *entry = table[hashValue];

    while (entry != nullptr)
    {
        if (entry->getKey() == key)
        {
            value = entry->getValue();
            return true;
        }
        entry = entry->getNext();
    }
    return false;
}


template <typename K, typename V, typename F>
void HashMap<K, V, F>::put(const K &key, const V &value)
{
    unsigned long hashValue = hashFunc(key);
    HashNode<K, V> *prev = nullptr;
    HashNode<K, V> *entry = table[hashValue];

    while (entry != nullptr && entry->getKey() != key)
    {
        prev = entry;
        entry = entry->getNext();
    }

    if (entry == nullptr)
    {
        entry = new HashNode<K, V>(key, value);
        if (prev == nullptr)
        {
            table[hashValue] = entry;
        } else {
            prev->setNext(entry);
        }
    } else
    {
        entry->setValue(value);
    }
}


template <typename K, typename V, typename F>
void HashMap<K, V, F>::remove(const K &key)
{
    unsigned long hashValue = hashFunc(key);
    HashNode<K, V> *prev = nullptr;
    HashNode<K, V> *entry = table[hashValue];

    while (entry != nullptr && entry->getKey() != key)
    {
        prev = entry;
        entry = entry->getNext();
    }

    if (entry == nullptr)
    {
        // key not found
        return;
    }
    else
    {
        if (prev == nullptr)
        {
            table[hashValue] = entry->getNext();
        }
        else
        {
            prev->setNext(entry->getNext());
        }
        delete entry;
    }
}


template <typename K, typename V, typename F>
void HashMap<K, V, F>::getAll(std::vector<long> &mas)
{
    HashNode<K, V> *prev = nullptr;
    HashNode<K, V> *entry = nullptr;

    for (size_t i = 0; i < TABLE_SIZE; i++)
    {
        entry = table[i];
        while (entry != nullptr)
        {
            mas.push_back(entry->getValue());
            entry = entry->getNext();
        }
    }
}
*/
