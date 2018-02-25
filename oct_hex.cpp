#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[])
{
    int i;
    int j = 0;
    int k;
    int to_convert;
    int result;
    char make_convert[100];
    char make_result[100];
    char make_comment[100] = "//";
    ifstream filein;
    ofstream fileout;

    if(argc < 2)
        return 0;

    fileout.open(argv[2]);
    filein.open(argv[1]);

    if(filein.is_open() && fileout.is_open())
    {
        filein.getline(make_convert, 100);
        while(!filein.eof())
        {
            j = 0;
            k = 0;
            cout << make_convert << endl;
            
            for(i = 0; i < 100 && make_convert[i] != '\0' && make_convert[i] != '\n'; ++i)
            {
                if(make_convert[i] <= '9' && make_convert[i] >= '0')
                {
                    make_result[j] = make_convert[i];
                    make_comment[k] = make_convert[i];
                    make_convert[i] = '\0';
                    ++j;
                    ++k;
                }
            }
            make_result[j] = '\0';
            make_comment[k] = '\0';
            result = strtol(make_result, NULL, 8);
            cout << make_result << ' ' << result << endl;
            sprintf(make_result, "%s\n", make_comment);
            fileout << make_result;
            cout << make_result;
            filein.getline(make_convert, 100);
        }

        cout << make_convert << endl;
        filein.close();
        fileout.close();
    }

    return 0;
}


