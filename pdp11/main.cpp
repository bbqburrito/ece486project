//compile with -std=c++11
//uses auto type declaration, range-based for loops,
//nullptr 
//outputs memory contents to "memory.txt"
//will delete any file in current folder
//named memory.txt


#include "pdp11.h"

using namespace std;


int main(int argc, char* argv[])
{
    char answer[25];
    int start;
    i_cache prog_mem[I_SIZE];
    //int memory[MEM_SIZE];             //data memory and program memory same,
                                        //so no separate data memory required.
    uint16_t gps[8];
    uint16_t to_run;
    CPSR status_reg;
    int condition;
    int firstbit;
    int to_interpret;
    int filepos = 0;
    int prog_size;
    int i = 0;
    int j;
    int c;
    int instructions_executed = 0;
    bool verbose;
    command * new_command;
    char make_disposition = '0';
    char * disposition = &make_disposition;         //initialize disposition
    char * trace;
    char br_trace[] = "branch_trace.out";
    ofstream trfile;
    ofstream outfile;
    ifstream mkfile;
    time_t timer;
    struct tm * timeinfo;

    static struct option const long_options[] =
            {
                    //{"number-nonblank", no_argument, NULL, 'b'},
                    //{"number", no_argument, NULL, 'n'},
                    //{"squeeze-blank", no_argument, NULL, 's'},
                    {"verbose", no_argument, NULL, 'v'},
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

            case 't':
                cout << "option t" << endl;
                show_tabs = true;
                show_nonprinting = true;
                break;

            case 'u':
                 We provide the -u feature unconditionally.
                break;*/

            case 'v':
                verbose = true;
                break;

            /*case 'A':
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
        cout << "usage: pdp11 [-v] <input file> <output file>" << endl;

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
        cout << "usage: pdp11 [-v] <input file> <output file>" << endl;
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

    mkfile.open(br_trace);

    if(mkfile.is_open())
    {
        cin.sync();
        cout << "branch trace file already exists. erase and use this filename (Y/n)";
        cin.getline(answer, 25);
        cin.sync();
        if(toupper(answer[0]) == 'N')
        {
            //get filename from stdin
            while(mkfile.is_open())
            {
                mkfile.close();
                cin.sync();
                cout << "File already exists. Enter name for new branch trace file: ";
                cin.getline(br_trace, 100);
                cin.sync();
                mkfile.open(br_trace);
            }
        }
        mkfile.close();
    }

    //erase existing file
    trfile.open(br_trace, ios::trunc | ios::out);
    if(!trfile.is_open())
    {
        cout << "cannot open\n";                    
        return 0;
    }

    //write to file. put timestamp and column headers
    time(&timer);
    timeinfo = localtime(&timer);

    trfile << asctime(timeinfo) << endl;

    trfile << "Branch trace record\n" << endl;

    trfile << "type\taddress\ttaken?" << endl;
    trfile.close();


    //read file into memory
    i = 0;
    to_interpret = line_reader(argv[argc - 2], disposition, filepos);
    while((to_interpret != -1) && (i < I_SIZE))
    {
        if(*disposition == '@')
        {
            i = to_interpret;
        } else {

            prog_mem[i].disposition = *disposition;
            prog_mem[i].data = uint16_t(to_interpret);     //low bytes stored at even-numbered memory locations 
            prog_mem[++i].data = uint16_t(to_interpret);    //high bytes stored at odd-numbered memory locations
            prog_mem[i].disposition = *disposition;
            ++i;
        }
        to_interpret = line_reader(argv[argc - 2], disposition, filepos);
    }

    prog_size = i;

    filepos = 0;

    //output memory contents
    /*for(j = 0; j <= i; ++j)
    {
        cout << prog_mem[j].disposition << prog_mem[j].data << " " << prog_mem[j + 1].data << endl;
        ++j;
    }*/
 
    start = findstart(prog_mem, prog_size);       //get start point

    cout << "start point: " << start << endl;
    

    gps[PC] = uint16_t(start);

    if(verbose)         //display registers before executing
        {
            condition = status_reg.get_condition();

            cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
            cout << "\t\t" << ((condition >> 3) & 1) << "\t";
            cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
            cout << "\t" << (condition & 1) << endl;
            for(i = 0; i < 8; ++i)
            {
                cout << "register " << i << ": " << gps[i] << "\t";
            }
            cout << endl;
        }

    //loop to run program
    while(gps[PC] < I_SIZE)
    {
        to_run = prog_mem[gps[PC]].data;    //get instruction from memory
        trace_file(trace, 2, gps[PC]);      //write to trace file
        gps[PC] += 2;
        ++instructions_executed;
        to_interpret = interpreter((uint16_t)to_run, &firstbit, new_command, trace);
        new_command->set_br_trace(br_trace);
        to_interpret = new_command->instruction(gps, &status_reg, prog_mem);

        if(verbose)
        {
            new_command->fetch_display(gps, &status_reg);
        }

    }

    cout << "program size: " << prog_size << endl;
    cout << "Total number of instructions executed: " << instructions_executed << endl;

    //write memory to file. put timestamp and column headers
    outfile.open("memory.txt", ios_base::trunc | ios_base::out);
    time(&timer);
    timeinfo = localtime(&timer);

    outfile << asctime(timeinfo) << endl;

    outfile << "Memory contents:\n" << endl;

    for(i = 0; i < I_SIZE; ++i)
    {
        outfile << "mem[" << oct << i << "] = " << prog_mem[i].data << endl;
    }

    outfile.close();

    return 0;
}


