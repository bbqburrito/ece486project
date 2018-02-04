/* takes a filename as argument
 * reads file line by line
 * ignores first character, then ignores any
 * lines not 7 digits. Uses strtol to convert
 * string to long int, reading as octal number.
 * Then parses integer to first digit, then 
 * second digit.
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

using namespace std;

int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        cout << "usage: readFile <filename>\n";
        exit(0);
    }

    char address[100];
    char * endptr;
    long data;

    ifstream filein;

    filein.open(argv[1]);

    if(!filein.is_open())
    {
        cout << "file not found\n";
        exit(0);
    }

    filein.getline(address, 100);

    while(!filein.eof())
    {
        if(strlen(address) != 7) 
        {
            filein.getline(address, 100);
            continue;
        }

        address[0] = '0';

        data = strtol(address, &endptr, 8);

        cout << data << endl;

        cout << "first bit: " << (data >> 15) << endl;

        cout << "bits 14-12: " << (data >> 12) - (data >> 15)*8 << endl;

        filein.getline(address, 100);
    }

    return 0;
}



