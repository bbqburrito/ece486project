/* takes a filename as argument
 * reads file line by line
 * ignores first character, then ignores any
 * lines not 7 digits. Uses strtol to convert
 * string to long int, reading as octal number.
 * Then parses integer to first digit, then 
 * second digit.
 */

#include "pdp11.h"

using namespace std;

int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        cout << "usage: readFile <filename>\n";
        exit(0);
    }

    char address[100];
    char disposition;
    char * endptr;
    long data;
    int bits1412;
    int firstbit;

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

        disposition = address[0];

        address[0] = '0';

        data = strtol(address, &endptr, 8);

        cout << data << endl;

        cout << "disposition: " << disposition << endl;

        firstbit = data >> 15;

        cout << "first bit: " << (data >> 15) << endl;

        bits1412 = (data >> 12) - (data >> 15)*8;
        
        cout << "bits 14-12: " << bits1412 << endl;

        filein.getline(address, 100);
    }

    return 0;
}



