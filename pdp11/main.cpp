//compile with -std=c++11
//uses auto type declaration, range-based for loops,
//nullptr


#include "pdp11.h"

using namespace std;


int main(int argc, char* argv[])
{
    char answer[25];
    int start;
    i_cache prog_mem[I_SIZE];
    //int memory[MEM_SIZE];             //data memory and program memory same,
                                        //so no separate data memory required.
    int gps[8];
    CPSR status_reg;
    int firstbit;
    int make_instruction;
    int to_interpret;
    int filepos = 0;
    int prog_size;
    int i = 0;
    int j;
    int c;
    command * new_command;
    char make_disposition = '0';
    char * disposition = &make_disposition;         //initialize disposition
    char * trace;
    ofstream trfile;
    ifstream mkfile;
    time_t timer;
    struct tm * timeinfo;

    static struct option const long_options[] =
            {
                    //{"number-nonblank", no_argument, NULL, 'b'},
                    //{"number", no_argument, NULL, 'n'},
                    //{"squeeze-blank", no_argument, NULL, 's'},
                    //{"show-nonprinting", no_argument, NULL, 'v'},
                    //{"show-ends", no_argument, NULL, 'E'},
                    //{"show-tabs", no_argument, NULL, 'T'},
                    //{"show-all", no_argument, NULL, 'A'},
                    //{GETOPT_HELP_OPTION_DECL},
                    //{GETOPT_VERSION_OPTION_DECL},
                    {nullptr, 0, nullptr, 0}
            };


    //parse options
    while ((c = getopt (argc, argv, "benstuvAET"))
           != -1)
    {
        switch (c)
        {
            /*case 'b':
                number = true;
                number_nonblank = true;
                break;

            case 'e':
                show_ends = true;
                show_nonprinting = true;
                break;

            case 'n':
                number = true;
                break;

            case 's':
                squeeze_blank = true;
                break;

            */case 't':
                cout << "option t" << endl;/*
                show_tabs = true;
                show_nonprinting = true;
                break;

            case 'u':
                /* We provide the -u feature unconditionally.  */
                /*break;

            case 'v':
                show_nonprinting = true;
                break;

            case 'A':
                show_nonprinting = true;
                show_ends = true;
                show_tabs = true;
                break;

            case 'E':
                show_ends = true;
                break;

            case 'T':
                show_tabs = true;
                break;

                case_GETOPT_HELP_CHAR;

                case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
*/
            default:
                c = 0;
                //usage (EXIT_FAILURE);

        }
    }



    if(argc < 3)
    {
        cout << "usage: pdp11 [-options] <input file> <output file>" << endl;

        return 0;
    }

    i = 0;

    for(j = 1; j < argc; ++j)
    {
        if(argv[j][0] != '-')
        {
            ++i;
        }                
    }

    if(i != 2)
    {
        cout << "usage: pdp11 [-options] <input file> <output file>" << endl;
        return 0;
    }

    trace = argv[argc - 1];

    mkfile.open(trace);

    if(mkfile.is_open())
    {
        cin.sync();
        cout << "trace file already exists. erase and use this filename (Y/n)";
        cin.getline(answer, 25);
        cin.sync();
        if(toupper(answer[0]) == 'N')
        {
            //get filename from stdin
            while(mkfile.is_open())
            {
                mkfile.close();
                cin.sync();
                cout << "enter name for new trace file: ";
                cin.getline(trace, 100);
                cin.sync();
                mkfile.open(trace);
            }
        }
    }

    //erase existing file
    trfile.open(trace, ios::trunc | ios::out);
    if(!trfile.is_open())
    {
        cout << "cannot open\n";                    
        return 0;
    }

    //write to file. put timestamp and column headers
    time(&timer);
    timeinfo = localtime(&timer);

    trfile << asctime(timeinfo) << endl;

    trfile << "type\taddress" << endl;
    trfile.close();

    //to_interpret = atoi(argv[1]);

    //instruction = interpreter(to_interpret, &firstbit, new_command);


    to_interpret = line_reader(argv[argc - 2], disposition, filepos);

    i = 0;

    //read file into memory
    while(to_interpret != -1)
    {
        prog_mem[i].disposition = *disposition;
        prog_mem[i].data = to_interpret;
        ++i;

        to_interpret = line_reader(argv[argc - 2], disposition, filepos);
    }

    prog_size = i;

    filepos = 0;

    for(j = 0; j <= i; ++j)
    {
        cout << prog_mem[j].disposition << prog_mem[j].data << endl;
    }

    start= findstart(prog_mem, prog_size);       //get start point

    cout << "start point: " << start << endl;
    
    for(i = 0; i < prog_size; ++i)
    {
        cout << "mem[" << i << "] = " << prog_mem[i].data << endl;
    }

    gps[7] = start * 2;

    while(gps[7]/2 < prog_size)
    {
        to_interpret = interpreter(prog_mem[gps[7]/2].data, &firstbit, new_command, trace);

        to_interpret = new_command->instruction(gps, &status_reg, prog_mem);
    }

    cout << "program size: " << prog_size << endl;

    for(i = 0; i < prog_size; ++i)
    {
        cout << "mem[" << i << "] = " << prog_mem[i].data << endl;
    }


    return 0;
}

