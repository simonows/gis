#ifndef __HASH_H__
#define __HASH_H__

#include <vector>

#define TABLE_SIZE 1024


template <typename K, typename V>
class HashNode
{
    K _key;
    V _value;
    // next bucket with the same key
    HashNode *_next;

public:
    HashNode(const K &key, const V &value):
        _key(key)
      , _value(value)
      , _next(nullptr)
    {
    }

    K getKey() const
    {
        return _key;
    }

    V getValue() const
    {
        return _value;
    }

    void setValue(V value)
    {
        _value = value;
    }

    HashNode *getNext() const
    {
        return _next;
    }

    void setNext(HashNode *next)
    {
        _next = next;
    }
};


// Default hash function class
template <typename K>
struct KeyHash {
    unsigned long operator()(const K& key) const
    {
        return static_cast<unsigned long>(key[0] + key[1] + key[2] + key[3]) % TABLE_SIZE;
    }
};

template <typename K, typename V, typename F = KeyHash<K>>
class HashMap {
    // hash table
    HashNode<K, V> **table;
    F hashFunc;

public:
    HashMap()
    {
        table = new HashNode<K, V> *[TABLE_SIZE]();
    }
    ~HashMap()
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


    bool get(const K &key, V &value)
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


    void put(const K &key, const V &value)
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


    void remove(const K &key)
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


    void getAll(std::vector<long> &mas, std::vector<unsigned long> &mas2)
    {
        HashNode<K, V> *prev = nullptr;
        HashNode<K, V> *entry = nullptr;

        for (size_t i = 0; i < TABLE_SIZE; i++)
        {
            entry = table[i];
            while (entry != nullptr)
            {
                mas.push_back(entry->getValue());
                mas2.push_back(i);
                entry = entry->getNext();
            }
        }
    }

};

#endif /* __HASH_H__ */

