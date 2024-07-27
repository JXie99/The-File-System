// CMSC 341 - Fall 2021 - Project 4
#include "hash.h"
#include <iostream>
#include <random>
#include <vector>

using namespace std;
enum RANDOM {UNIFORM, NORMAL};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORM) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 and standard deviation of 20
            m_normdist = std::normal_distribution<>(50,20);
        }
        else{
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
    }

    int getRandNum(){
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else{
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//uniform distribution

};

// The hash function used by HashTable class
unsigned int hashCode(const string str);

class Tester{ // Tester class to implement test functions
public:
    void testInsert();
    void testGetFile();
    void testRemove();
};


int main(){
    // This program presents a sample use of the class HashTable
    // It does not represent any rehashing
    Random diskBlockGen(DISKMIN,DISKMAX);
    int tempDiskBlocks[50] = {0};
    HashTable aTable(MINPRIME,hashCode);
    int temp = 0;
    int secondIndex = 0;
    for (int i=0;i<50;i++){
        temp = diskBlockGen.getRandNum();
        if (i%3 == 0){//this saves 17 numbers from the index range [0-49]
            tempDiskBlocks[secondIndex] = temp;
            cout << temp << " was saved for later use." << endl;
            secondIndex++;
        }
        cout << "Insertion # " << i << " => " << temp << endl;
        if (i%3 != 0)
            aTable.insert(File("test.txt", temp));
        else
            // these will be deleted
            aTable.insert(File("driver.cpp", temp));
    }

    cout << "Message: dump after 50 insertions in a table with MINPRIME (101) buckets:" << endl;
    aTable.dump();

    for (int i = 0;i<14;i++)
        aTable.remove(File("driver.cpp", tempDiskBlocks[i]));
    cout << "Message: dump after removing 14 buckets," << endl;
    aTable.dump();

    Tester testing;
    testing.testInsert();
    testing.testGetFile();
    testing.testRemove();

    return 0;
}

unsigned int hashCode(const string str) {
    unsigned int val = 0 ;
    const unsigned int thirtyThree = 33 ;  // magic number from textbook
    for ( int i = 0 ; i < str.length(); i++)
        val = val * thirtyThree + str[i] ;
    return val ;
}

void Tester::testInsert() {
    //tests if file are insert in the correct buckets and if data size is correct after inserts

    int tests = 0;
    cout << "Testing Insert function\n" << endl;

    HashTable aTable(MINPRIME,hashCode);


    aTable.insert(File("test.txt", 250100));
    aTable.insert(File("testA.txt", 250101));
    aTable.insert(File("testAB.txt", 250102));
    aTable.insert(File("testABC.txt", 250103));


    if(aTable.m_size1 == 4)
    {
        cout << "data size is correct" << endl;
        tests++;
    }
    else
    {
        cout << "insert appears not to be working" << endl;
    }

    unsigned int index1 = hashCode("test.txt") % aTable.tableSize(aTable.m_newTable);
    unsigned int index2 = hashCode("testA.txt") % aTable.tableSize(aTable.m_newTable);
    unsigned int index3 = hashCode("testAB.txt") % aTable.tableSize(aTable.m_newTable);
    unsigned int index4 = hashCode("testABC.txt") % aTable.tableSize(aTable.m_newTable);
    int counter = 0;

    if(aTable.m_table1[index1].key() == "test.txt")
    {
        counter++;
    }

    if(aTable.m_table1[index2].key() == "testA.txt")
    {
        counter++;
    }

    if(aTable.m_table1[index3].key() == "testAB.txt")
    {
        counter++;
    }

    if(aTable.m_table1[index4].key() == "testABC.txt")
    {
        counter++;
    }

    if(counter == 4)
    {
        cout << "all inserts appear to be in the correct bucket" << endl;
        tests++;
    }

    if(tests == 2)
    {
        cout << "insert appears to be working\n" << endl;
    }
    else
    {
        cout << "insert appears to have failed\n" << endl;
    }
}

void Tester::testGetFile() {

    cout << "testing getfile\n " << endl;
    int tests = 0;
    int counter1 = 0;
    int counter2 = 0;
    HashTable aTable(MINPRIME, hashCode);

    aTable.insert(File("test.txt", 250100));
    aTable.insert(File("testA.txt", 250101));
    aTable.insert(File("testAB.txt", 250102));
    aTable.insert(File("testABC.txt", 250103));

    if (!(aTable.getFile("test.txt", 250100) == EMPTY))
        counter1++;

    if (!(aTable.getFile("testA.txt", 250101) == EMPTY))
        counter1++;

    if (!(aTable.getFile("testAB.txt", 250102) == EMPTY))
        counter1++;

    if (!(aTable.getFile("testABC.txt", 250103) == EMPTY))
        counter1++;

    if (counter1 == 4) {
        cout << "find with non colliding keys has worked" << endl;
        tests++;
    }
    else
    {
        cout << "find with non colliding keys has failed" << endl;
    }

    HashTable aTable2(MINPRIME, hashCode);

    aTable2.insert(File("driver.txt", 350100));
    aTable2.insert(File("driver.txt", 350101));
    aTable2.insert(File("driver.txt", 350102));
    aTable2.insert(File("driver.txt", 350103));

    if (!(aTable2.getFile("driver.txt", 350100) == EMPTY))
        counter2++;

    if (!(aTable2.getFile("driver.txt", 350101) == EMPTY))
        counter2++;

    if (!(aTable2.getFile("driver.txt", 350102) == EMPTY))
        counter2++;

    if (!(aTable2.getFile("driver.txt", 350103) == EMPTY))
        counter2++;

    if (counter2 == 4) {
        cout << "find with colliding keys has worked" << endl;
        tests++;
    }
    else
    {
        cout << "find with colliding keys has failed" << endl;
    }

    if(aTable.getFile("driver.txt", 234000) == EMPTY)
    {
        cout << "find with a non existing file in table works" << endl;
        tests++;
    }
    else
    {
        cout << "find with a non existing file in table failed" << endl;
    }

    if(tests == 3)
    {
        cout << "getFile appears to be working correctly\n" << endl;
    }
    else
    {
        cout << "getFile has failed\n " << endl;
    }

}

void Tester::testRemove() {

    cout << "Testing Remove function\n " << endl;
    int tests = 0;
    int counter = 0;

    HashTable aTable(MINPRIME, hashCode);

    aTable.insert(File("test.txt", 250100));
    aTable.insert(File("testA.txt", 250101));
    aTable.insert(File("testAB.txt", 250102));
    aTable.insert(File("testABC.txt", 250103));

    unsigned int index1 = hashCode("test.txt") % aTable.tableSize(aTable.m_newTable);
    unsigned int index2 = hashCode("testA.txt") % aTable.tableSize(aTable.m_newTable);
    unsigned int index3 = hashCode("testAB.txt") % aTable.tableSize(aTable.m_newTable);
    unsigned int index4 = hashCode("testABC.txt") % aTable.tableSize(aTable.m_newTable);

    aTable.remove(File("test.txt", 250100));
    aTable.remove(File("testA.txt", 250101));
    aTable.remove(File("testAB.txt", 250102));
    aTable.remove(File("testABC.txt", 250103));

    if(aTable.m_table1[index1] == DELETED)
    {
        counter++;
    }

    if(aTable.m_table1[index2] == DELETED)
    {
        counter++;
    }

    if(aTable.m_table1[index3] == DELETED)
    {
        counter++;
    }

    if(aTable.m_table1[index4] == DELETED)
    {
        counter++;
    }

    if(counter == 4)
    {
        cout << "remove with non colliding keys appears to be working" << endl;
        tests++;
    }
    else
    {
        cout << "remove with non colliding keys has failed" << endl;
    }

    if(tests == 1)
    {
        cout << "remove appears to be working\n" << endl;
    }
    else
    {
        cout << "remove has failed\n" << endl;
    }

}