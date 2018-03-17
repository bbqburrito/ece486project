//William Boyd, Jonathan Anchell, Thao Tran, Dai Ho
//ECE 486 final project
//Winter 2018
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
    bool verbose = false;
    bool start_entry = false;
    command * new_command = nullptr;
    char make_disposition = '0';
    char * disposition = &make_disposition;         //initialize disposition
    char * trace;
    char br_trace[] = "branch_trace.out";
    ofstream trfile;
    ofstream outfile;
    ifstream mkfile;
    time_t timer;
    struct tm * timeinfo;

    //parse options
    while ((c = getopt (argc, argv, "benstuvAET"))
           != -1)
    {
        switch (c)
        {
            case 's':
                start_entry = true;
                break;

            case 'v':
                verbose = true;
                break;

            default:
                c = 0;
                //usage (EXIT_FAILURE);
        }
    }



    if(argc < 3)
    {
        cout << "usage: pdp11 [-v] <input file> <output file> [-s <start>]" << endl;

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

    j = j - i;          //get number of argv that are flags

    if(i != (2 + start_entry))
    {
        cout << "usage: pdp11 [-v] <input file> <output file> [-s <start>]" << endl;
        return 0;
    }

    trace = argv[argc - 1 - start_entry];

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
    to_interpret = line_reader(argv[argc - 2 - start_entry], disposition, filepos);
    while((to_interpret != -1) && (i < I_SIZE))
    {
        if(*disposition == '@')
        {
            i = to_interpret;
            prog_size = i;
            start = i;
        } else {

            prog_mem[i].disposition = *disposition;
            prog_mem[i].data = uint16_t(to_interpret);     //low bytes stored at even-numbered memory locations 
            prog_mem[++i].data = uint16_t(to_interpret);    //high bytes stored at odd-numbered memory locations
            prog_mem[i].disposition = *disposition;
            ++i;
        }
        to_interpret = line_reader(argv[argc - 2 - start_entry], disposition, filepos);
    }

    prog_size = i - prog_size;

    filepos = 0;

    //get start point relative to program load address
    if(start_entry)
    {
        start += strtol(argv[argc - 1], nullptr, 8);
    } else {
        start += findstart(prog_mem, prog_size);       //get start point
    }

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

    if(new_command)
    {
        delete new_command;
        new_command = nullptr;
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

        if(new_command)
        {
            delete new_command;
            new_command = nullptr;
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


