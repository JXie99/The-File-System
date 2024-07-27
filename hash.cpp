/*
 * File:          hash.cpp
 * Project:       CMSC 341 Project 4 Fall 2021
 * Author:        Johnson Xie 
 *
 * Description:   This file contains the hash.cpp
 *
 *
 */

#include "hash.h"
#include "math.h"

//alternative constructor 
HashTable::HashTable(unsigned size, hash_fn hash)
{
    if(size < MINPRIME)
    {
        m_capacity1 = MINPRIME;
    }

    else if(size > MAXPRIME)
    {
        m_capacity1 = MAXPRIME;
    }

    else if(isPrime(size) == false)
    {
        m_capacity1 = findNextPrime(size);
    }

    else if(isPrime(size) == true)
    {
        m_capacity1 = size;
    }

    m_hash = hash;

    m_table1 = new File[m_capacity1];
    m_size1 = 0;
    m_numDeleted1 = 0;

    m_table2 = nullptr;
    m_size2 = 0;
    m_capacity2 = 0;
    m_numDeleted2 = 0;

    m_newTable = TABLE1;
}

//destructor 
HashTable::~HashTable()
{
    if(m_table1 != nullptr)
    {
        for(int i = 0; i < m_capacity1; i++)
        {
            m_table1[i] = EMPTY;
        }

        m_table1 = nullptr;
    }

    if(m_table2 != nullptr)
    {
        for(int i = 0; i < m_capacity2; i++)
        {
            m_table2[i] = EMPTY;
        }

        m_table2 = nullptr;
    }
}

//looks for file object with name and discblock
File HashTable::getFile(string name, unsigned int diskBlock)
{

    File theFile = EMPTY;

    if(m_table1 != nullptr)
    {
        for(int i = 0; i < m_capacity1; i++)
        {
            if(m_table1[i].key() == name && m_table1[i].diskBlock() == diskBlock)
            {
                theFile = m_table1[i];
            }
        }
    }

    if(m_table2 != nullptr)
    {
        for (int i = 0; i < m_capacity2; i++)
        {
            if (m_table2[i].key() == name && m_table2[i].diskBlock() == diskBlock)
            {
                theFile = m_table2[i];
            }
        }
    }

    return theFile;
}

//inserts object into hash table
bool HashTable::insert(File file)
{

    // index if there is no collision
    unsigned int index = (m_hash(file.key()) % tableSize(m_newTable));
    bool inserted = false;
    int quadNum = 0;


    //if we are working with table 1
    if(m_newTable == TABLE1)
    {
        while (!(m_table1[index] == EMPTY) && !(m_table1[index] == DELETED) && (index <= tableSize(m_newTable)))
        {
            index = (index + (quadNum * quadNum)) % tableSize(m_newTable);
            quadNum++;
        }

        if(m_table1[index] == EMPTY || m_table1[index] == DELETED)
        {
            m_table1[index] = file;
            m_size1++;
            inserted = true;
        }
    }

    //if we are working with table 2
    if(m_newTable == TABLE2)
    {
        while (!(m_table2[index] == EMPTY) && !(m_table2[index] == DELETED) && index <= tableSize(m_newTable))
        {
            index = (index + (quadNum * quadNum)) % tableSize(m_newTable);
            quadNum++;
        }


        if(m_table2[index] == EMPTY || m_table2[index] == DELETED)
        {
            m_table2[index] = file;
            m_size2++;
            inserted = true;
        }
    }

    return inserted;

}

//removes data point from the hash table
bool HashTable::remove(File file)
{
    bool found = false;
    int quadNum = 0;
    unsigned int index = (m_hash(file.key()) % tableSize(m_newTable));


    //if the file exists or has not been deleted before
    if(!(getFile(file.key(), file.diskBlock()) == EMPTY) && !(getFile(file.key(), file.diskBlock()) == DELETED))
    {
        if(m_table1 != nullptr)
        {
            while (m_table1[index].diskBlock() != file.diskBlock())
            {
                index = (index + quadNum * quadNum) % tableSize(m_newTable);
                quadNum++;
            }

            if(m_table1[index] == file)
            {
                m_table1[index] = DELETED;
                m_numDeleted1++;
                found = true;
            }

            quadNum = 0;
        }

        if(m_table2 != nullptr)
        {
            while((index <= tableSize(m_newTable)) && !(m_table2[index] == file) && !(m_table2[index] == DELETED))
            {
                index = (index + (quadNum * quadNum)) % tableSize(m_newTable);
                quadNum++;
            }

            if(m_table2[index] == file)
            {
                m_table2[index] = DELETED;
                m_numDeleted2++;
                found = true;
            }
        }
    }

    return found;
}

//returns the ratio of occupied buckets to the table capacity aka load factor.
float HashTable::lambda(TABLENAME tablename) const
 {
    float ratio = 0;

    if(tablename == TABLE1)
    {
        ratio = float(m_size1) / float(m_capacity1);
    }

    if(tablename == TABLE2)
    {
        ratio = float(m_size2) / float(m_capacity2);
    }

    return ratio;
}

//returns ratio of deleted buckets to the total num of occupied buckets 
float HashTable::deletedRatio(TABLENAME tableName) const 
{
    float ratio = 0;

    if(tableName == TABLE1)
    {
        ratio = float(m_numDeleted1) / float(m_size1);
    }

    if(tableName == TABLE2)
    {
        ratio = float(m_numDeleted2) / float(m_size2);
    }

    return ratio;
}

//dumps contents of hash table in index order 
void HashTable::dump() const {
    cout << "Dump for table 1: " << endl;
    if (m_table1 != nullptr)
        for (int i = 0; i < m_capacity1; i++) 
        {
            cout << "[" << i << "] : " << m_table1[i] << endl;
        }
    cout << "Dump for table 2: " << endl;
    if (m_table2 != nullptr)
        for (int i = 0; i < m_capacity2; i++) 
        {
            cout << "[" << i << "] : " << m_table2[i] << endl;
        }
}

//returns if is prinme or not 
bool HashTable::isPrime(int number)
{
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) 
    {
        if (number % i == 0) 
        {
            result = false;
            break;
        }
    }
    return result;
}

//returns smallest prime num greater than the passed arugement 
int HashTable::findNextPrime(int current)
{
    //we always stay within the range [MINPRIME-MAXPRIME]
    //the smallest prime starts at MINPRIME
    if (current < MINPRIME) current = MINPRIME-1;
    for (int i=current; i<MAXPRIME; i++) 
    {
        for (int j=2; j*j<=i; j++) 
        {
            if (i % j == 0)
                break;
            else if (j+1 > sqrt(i) && i != current)
             {
                return i;
            }
        }
    }
    //if a user tries to go over MAXPRIME
    return MAXPRIME;
}
