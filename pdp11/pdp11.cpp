//all functions in application, except those in double operand
//class. std namespace declared.
//compile with -std=c++11
//uses auto type declaration, range-based for loops,
//nullptr


#include "pdp11.h"

using namespace std;

/*reads the file by line and returns the command by long
 * takes char pointer to store disposition of line
 */
long line_reader(char *filename, char *&disposition, int &filepos) {
    char address[100];
    char * endptr;
    long data;

    ifstream filein;

    for (auto &addres : address) {
        addres = '\0';
    }

    filein.open(filename);

    if(!filein.is_open())
    {
        cout << "file not found\n";
        return -1;
    }

    filein.seekg(filepos);      //maintain file position

    filein.getline(address, 100);
    if(address[7] == '\r')
    {
        address[7] = '\0';
    }

    //while((strlen(address) != 7) && !filein.eof()) //if line length is not 7
    //and file is not ended
    //{
    //    filein.getline(address, 100);
    //}

    if(filein.eof())
    {
        *disposition = '\0';
        data = -1;
        filepos = static_cast<int>(filein.tellg());   //pass file position.

        for (auto &addres : address) {
            addres = '\0';
        }

        filein.close();
        return data;
    }

    while((strlen(address) != 7))       //if string loaded not 7 characters
    {                                   //read next line
        if(filein.eof())
        {
            *disposition = '\0';
            data = -1;
            filepos = static_cast<int>(filein.tellg());   //pass file position

            for (char &addres : address) {
                addres = '\0';
            }

            filein.close();
            return data;
        }

        for (auto &addres : address) {
            addres = '\0';          //reset address
        }

        filein.getline(address, 100);
        if(address[7] == '\r')
            address[7] = '\0';
    }

    *disposition = address[0];          //get disposition
    address[0] = '0';                   //set first char to 0

    for(int i = 0; i < 7; ++i)          //invalid characters converted to 0
    {
        if ('7' < address[i]) {
            if (address[i] < '0')
                address[i] = '0';
        }
    }

    data = strtol(address, &endptr, 8); //convert char to long
    //by octal
    //data = strtol(address, &endptr, 10); //convert char to long
    filepos = static_cast<int>(filein.tellg());           //pass file position

    filein.close();

    return data;
}


int findstart(i_cache *prog_mem, int prog_length) {
    char answer[25];
    int start = 0;
    int i;
    int j;

    cin.sync();
    cout << "input start point? (y/N) ";    //if start point not inputted as arg
    //ask user for start point
    //if no, find * disposition
    //if no *, start from 0
    cin.getline(answer, 25);
    cin.sync();

    if(toupper(answer[0]) == 'Y')
    {
        cin.sync();
        cout << "enter start: ";
        cin.getline(answer, 25);
        cin.sync();
        start = strtol(answer, nullptr, 8);
        if(start > prog_length)
        {
            cout << "program only " << prog_length << " lines. Start set to 0\n";
            start = 0;
        }
    }
    else {
        cout << "find '*' to start at or start at 0 + indicated by @ (if present." << endl;
        i = 0;
        j = 0;                              //indicate '*' found
        while(prog_mem[i].disposition != '*' && i < I_SIZE)
        {
            ++i;
            if(prog_mem[i].disposition == '*')
            {
                start = i;
                j = 1;
            }
        }

        if(!j)
            start = 0;
    }
    return start;
}

//write to branch trace file
int branch_trace(char * filename, char * type, uint16_t address, char taken)
{
    char make_address[100];
    ofstream outfile;
    int i;
    int outcome;

    //initialize make_address to all NULL
    for(i = 0; i < 100; ++i)
    {
        make_address[i] = '\0';
    }

    outfile.open(filename, ios::app);

    if(!outfile.is_open())
    {
        cout << "file error\n";
        return -1;
    }

    outcome = sprintf(make_address, "%s\t\t%o\t\t%c\n", type, address, taken);

    outfile << make_address;

    outfile.close();

    return outcome;
}


//writes to file specified by filename. writes address parameter
//as an octal. returns an int to indicate outcome.
//takes a char to indicate filename, an int to indicate
//the type write, and an int to indicate the address write
int trace_file(char *filename, int type, uint16_t address) {
    char make_address[100];
    ofstream outfile;
    int i;
    int outcome;

    //initialize make_address to all NULL
    for(i = 0; i < 100; ++i)
    {
        make_address[i] = '\0';
    }

    outfile.open(filename, ios::app);

    if(!outfile.is_open())
    {
        cout << "file error\n";
        return -1;
    }

    outcome = sprintf(make_address, "%d\t\t%o\n", type, address);

    outfile << make_address;

    outfile.close();

    return outcome;
}

//takes int as a command to parse, an int pointer to pass firstbit, and a command 
//pointer to assign to command class object by upcasting. returns an int representing
//the function code of passed command. parses command, then creates an object of 
//class type and stores in new_command pointer
int interpreter(uint16_t to_interpret, int * firstbit, command *& new_command, char * tracefile)
{
    //int i;
    string instruction;
    int code;
    //int firstbit;
    int bit1214;
    int bit0911;
    int bit0608;
    int bit0305;
    int bit0002;
    //double_operand * make_command;

    //first bit indicates whether byte instruction
    //for some instructions
    *firstbit = to_interpret >> 15;

    //cout << *firstbit << endl;

    //next 3 bits indicate function code for double
    //operand commands
    bit1214 = (to_interpret >> 12) & 7;

    //cout << bit1214 << endl;

    //next 3 bits indicate more of function code for
    //extended instruction set
    bit0911 = (to_interpret >> 9) & 7;

    //cout << bit0911 << endl;

    //next 3 bits indicate function code for
    //single instruction set
    bit0608 = (to_interpret >> 6) & 7;

    //cout << (to_interpret >> 6) << " - " << *firstbit*512 << " - " << bit1214*64 << " - " << bit0911*8 << endl;

    //cout << bit0608 << endl;

    //next 3 bits indicate function code for
    //most control functions
    bit0305 = (to_interpret >> 3) & 7;

    //cout << bit0305 << endl;

    //next 3 bits indicate function code for
    //some control functions
    bit0002 = to_interpret & 7;

    //cout << bit0002 << endl;

    if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ ADD))
    {
        instruction = "ADD";
        code = ADD;
        //cout << "ADD" << endl;
        new_command = new double_operand(to_interpret, '-', tracefile, code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ CLR))
    {
        instruction = "CLR";
        code = CLR;
        //cout << "CLR" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ CLRB))
    {
        instruction = "CLRB";
        code = CLRB;
        //cout << "CLRB" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ COM))
    {
        instruction = "COM";
        code = COM;
        //cout << "COM" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ COMB))
    {
        instruction = "COMB";
        code = COMB;
        //cout << "COMB" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ INC))
    {
        instruction = "INC";
        code = INC;
        //cout << "INC" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ INCB))
    {
        instruction = "INCB";
        code = INCB;
        //cout << "INCB" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ DEC))
    {
        instruction = "DEC";
        code = DEC;
        //cout << "DEC" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ DECB))
    {
        instruction = "DECB";
        code = DECB;
        //cout << "DECB" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ NEG))
    {
        instruction = "NEG";
        code = NEG;
        //cout << "NEG" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ NEGB))
    {
        instruction = "NEGB";
        code = NEGB;
        //cout << "NEGB" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ TST))
    {
        instruction = "TST";
        code = TST;
        //cout << "TST" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ TSTB))
    {
        instruction = "TSTB";
        code = TSTB;
        //cout << "TSTB" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ ASR))
    {
        instruction = "ASR";
        code = ASR;
        //cout << "ASR" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ ASRB))
    {
        instruction = "ASRB";
        code = ASRB;
        //cout << "ASRB" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ ASL))
    {
        instruction = "ASL";
        code = ASL;
        //cout << "ASL" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ ASLB))
    {
        instruction = "ASLB";
        code = ASLB;
        //cout << "ASLB" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ ROR))
    {
        instruction = "ROR";
        code = ROR;
        //cout << "ROR" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ RORB))
    {
        instruction = "RORB";
        code = RORB;
        //cout << "RORB" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ ROL))
    {
        instruction = "ROL";
        code = ROL;
        //cout << "ROL" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ ROLB))
    {
        instruction = "ROLB";
        code = ROLB;
        //cout << "ROLB" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ SWAB))
    {
        instruction = "SWAB";
        code = SWAB;
        //cout << "SWAB" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ ADC))
    {
        instruction = "ADC";
        code = ADC;
        //cout << "ADC" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ ADCB))
    {
        instruction = "ADCB";
        code = ADCB;
        //cout << "ADCB" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ SBC))
    {
        instruction = "SBC";
        code = SBC;
        //cout << "SBC" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ SBCB))
    {
        instruction = "SBCB";
        code = SBCB;
        //cout << "SBCB" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ SXT))
    {
        instruction = "SXT";
        code = SXT;
        //cout << "SXT" << endl;
        new_command = new single_operand(to_interpret, '-', tracefile, code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ MOV))
    {
        instruction = "MOV";
        code = MOV;
        //cout << "MOV" << endl;
        new_command = new double_operand(to_interpret, '-', tracefile, code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ MOVB))
    {
        instruction = "MOVB";
        code = MOVB;
        //cout << "MOVB" << endl;
        new_command = new double_operand(to_interpret, '-', tracefile, code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ CMP))
    {
        instruction = "CMP";
        code = CMP;
        //cout << "CMP" << endl;
        new_command = new double_operand(to_interpret, '-', tracefile, code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ CMPB))
    {
        instruction = "CMPB";
        code = CMPB;
        //cout << "CMPB" << endl;
        new_command = new double_operand(to_interpret, '-', tracefile, code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ SUB))
    {
        instruction = "SUB";
        code = SUB;
        //cout << "SUB" << endl;
        new_command = new double_operand(to_interpret, '-', tracefile, code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ BIT))
    {
        instruction = "BIT";
        code = BIT;
        //cout << "BIT" << endl;
        new_command = new double_operand(to_interpret, '-', tracefile, code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ BITB))
    {
        instruction = "BITB";
        code =  BITB;
        //cout << "BITB" << endl;
        new_command = new double_operand(to_interpret, '-', tracefile, code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ BIC))
    {
        instruction = "BIC";
        code = BIC;
        //cout << "BIC" << endl;
        new_command = new double_operand(to_interpret, '-', tracefile, code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ BICB))
    {
        instruction = "BICB";
        code = BICB;
        //cout << "BICB" << endl;
        new_command = new double_operand(to_interpret, '-', tracefile, code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ BIS))
    {
        instruction = "BIS";
        code = BIS;
        //cout << "BIS" << endl;
        new_command = new double_operand(to_interpret, '-', tracefile, code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ BISB))
    {
        instruction = "BISB";
        code = BISB;
        //cout << "BISB" << endl;
        new_command = new double_operand(to_interpret, '-', tracefile, code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ BR))
    {
        instruction = "BR";
        code = BR;
        //cout << "BR" << endl;
        new_command = new branch(to_interpret, '-', tracefile, code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ BNE))
    {
        instruction = "BNE";
        code = BNE;
        //cout << "BNE" << endl;
        new_command = new branch(to_interpret, '-', tracefile, code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ BEQ))
    {
        instruction = "BEQ";
        code = BEQ;
        //cout << "BEQ" << endl;
        new_command = new branch(to_interpret, '-', tracefile, code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ BPL))
    {
        instruction = "BPL";
        code = BPL;
        //cout << "BPL" << endl;
        new_command = new branch(to_interpret, '-', tracefile, code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ BMI))
    {
        instruction = "BMI";
        code = BMI;
        //cout << "BMI" << endl;
        new_command = new branch(to_interpret, '-', tracefile, code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ BVC))
    {
        instruction = "BVC";
        code = BVC;
        //cout << "BVC" << endl;
        new_command = new branch(to_interpret, '-', tracefile, code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ BVS))
    {
        instruction = "BVS";
        code = BVS;
        //cout << "BVS" << endl;
        new_command = new branch(to_interpret, '-', tracefile, code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ BCC))
    {
        instruction = "BCC";
        code = BCC;
        //cout << "BCC" << endl;
        new_command = new branch(to_interpret, '-', tracefile, code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ BCS))
    {
        instruction = "BCS";
        code = BCS;
        //cout << "BCS" << endl;
        new_command = new branch(to_interpret, '-', tracefile, code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ BGE))
    {
        instruction = "BGE";
        code = BGE;
        //cout << "BGE" << endl;
        new_command = new branch(to_interpret, '-', tracefile, code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ BLT))
    {
        instruction = "BLT";
        code = BLT;
        //cout << "BLT" << endl;
        new_command = new branch(to_interpret, '-', tracefile, code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ BGT))
    {
        instruction = "BGT";
        code = BGT;
        //cout << "BGT" << endl;
        new_command = new branch(to_interpret, '-', tracefile, code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ BLE))
    {
        instruction = "BLE";
        code = BLE;
        //cout << "BLE" << endl;
        new_command = new branch(to_interpret, '-', tracefile, code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ BHI))
    {
        instruction = "BHI";
        code = BHI;
        //cout << "BHI" << endl;
        new_command = new branch(to_interpret, '-', tracefile, code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ BLOS))
    {
        instruction = "BLOS";
        code = BLOS;
        //cout << "BLOS" << endl;
        new_command = new branch(to_interpret, '-', tracefile, code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ JMP))
    {
        instruction = "JMP";
        code = JMP;
        //cout << "JMP" << endl;
        new_command = new jump_sub(to_interpret, '-', tracefile, code, 0, bit0305, bit0002, 0);

        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3)) ^ JSR))
    {
        instruction = "JSR";
        code = JSR;
        //cout << "JSR" << endl;
        new_command = new jump_sub(to_interpret, '-', tracefile, code, bit0608, bit0305, bit0002, 0);

        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2) + bit0305*8) ^ RTS))
    {
        instruction = "RTS";
        code = RTS;
        //cout << "RTS" << endl;
        new_command = new jump_sub(to_interpret, '-', tracefile, code, bit0002, 0, 0, 0);

        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3)) ^ TRAP))
    {
        instruction = "TRAP";
        code = TRAP;
        //cout << "TRAP" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code,
                                          static_cast<int>(bit0608 * pow(8, 2) + bit0305 * pow(8, 1) + bit0002));

        //new_command->disp();
    }

    else if(!(to_interpret ^ RTI))
    {
        instruction = "RTI";
        code = RTI;
        //cout << "RTI" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code, 0);

        //new_command->disp();
    }

    else if(!(to_interpret ^ HALT))
    {
        instruction = "HALT";
        code = HALT;
        //cout << "HALT" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code, 0);

        //new_command->disp();
    }

    else if(!(to_interpret ^ WAIT))
    {
        instruction = "WAIT";
        code = WAIT;
        //cout << "WAIT" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code, 0);

        //new_command->disp();
    }

    else if(!(to_interpret ^ RESET))
    {
        instruction = "RESET";
        code = RESET;
        //cout << "RESET" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code, 0);

        //new_command->disp();
    }

    else if(!(to_interpret ^ CLC))
    {
        instruction = "CLC";
        code = CLC;
        //cout << "CLC" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code, 0);

        //new_command->disp();
    }

    else if(!(to_interpret ^ CLV))
    {
        instruction = "CLV";
        code = RTI;
        //cout << "CLV" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code, 0);

        //new_command->disp();
    }

    else if(!(to_interpret ^ CLZ))
    {
        instruction = "CLZ";
        code = CLZ;
        //cout << "CLZ" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code, 0);

        //new_command->disp();
    }

    else if(!(to_interpret ^ CLN))
    {
        instruction = "CLN";
        code = CLN;
        //cout << "CLN" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code, 0);

        //new_command->disp();
    }

    else if(!(to_interpret ^ CCC))
    {
        instruction = "CCC";
        code = CCC;
        //cout << "CCC" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code, 0);

        //new_command->disp();
    }

    else if(!(to_interpret ^ SEC))
    {
        instruction = "SEC";
        code = SEC;
        //cout << "SEC" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code, 0);

        //new_command->disp();
    }

    else if(!(to_interpret ^ SEV))
    {
        instruction = "SEV";
        code = SEV;
        //cout << "SEV" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code, 0);

        //new_command->disp();
    }

    else if(!(to_interpret ^ SEZ))
    {
        instruction = "SEZ";
        code = SEZ;
        //cout << "SEZ" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code, 0);

        //new_command->disp();
    }

    else if(!(to_interpret ^ SEN))
    {
        instruction = "SEN";
        code = SEN;
        //cout << "SEN" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code, 0);

        //new_command->disp();
    }

    else if(!(to_interpret ^ SCC))
    {
        instruction = "SCC";
        code = SCC;
        //cout << "SCC" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code, 0);

        //new_command->disp();
    }

    else if(!(to_interpret ^ NOP))
    {
        instruction = "NOP";
        code = NOP;
        //cout << "NOP" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', tracefile, code, 0);

        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3)) ^ MUL))
    {
        instruction = "MUL";
        code = MUL;
        //cout << "MUL" << endl;
        new_command = new extended(to_interpret, '-', tracefile, code, bit0608, bit0305, bit0002);

        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3)) ^ DIV))
    {
        instruction = "DIV";
        code = DIV;
        //cout << "DIV" << endl;
        new_command = new extended(to_interpret, '-', tracefile, code, bit0608, bit0305, bit0002);

        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3)) ^ ASH))
    {
        instruction = "ASH";
        code = ASH;
        //cout << "ASH" << endl;
        new_command = new extended(to_interpret, '-', tracefile, code, bit0608, bit0305, bit0002);

        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3)) ^ ASHC))
    {
        instruction = "ASHC";
        code = ASHC;
        //cout << "ASHC" << endl;
        new_command = new extended(to_interpret, '-', tracefile, code, bit0608, bit0305, bit0002);

        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3)) ^ XOR))
    {
        instruction = "XOR";
        code = XOR;
        //cout << "XOR" << endl;
        new_command = new extended(to_interpret, '-', tracefile, code, bit0608, bit0305, bit0002);

        //new_command->disp();
    }

    else
    {
        instruction = "nope";
        code = -1;
        cout << "instruction code: " << to_interpret << ": nope\n";
        new_command = new command(to_interpret, '-', tracefile);
    }

    return code;
}


//i_cache
i_cache::i_cache(): data(0), disposition('\0')
{
}


//CPSR
CPSR::CPSR(): T(0), N(0), Z(0), V(0), C(0), priority(0)
{
}

CPSR::~CPSR()
= default;

CPSR::CPSR(const CPSR &to_copy)
{
    int condition = to_copy.get_condition();
    priority = condition >> 5;
    T = (condition >> 4) & 1;
    N = (condition >> 3) & 1;
    Z = (condition >> 2) & 1;
    V = (condition >> 1) & 1;
    C = condition & 1;
}

int CPSR::get_condition() const
{
    int result = (priority << 5) + (T << 4) + (N << 3) + (Z << 2) + (V << 1) + C;

    return result;
}

int CPSR::set_condition(int to_set)
{
    T = (to_set >> 4) & 1;
    N = (to_set >> 3) & 1;
    Z = (to_set >> 2) & 1;
    V = (to_set >> 1) & 1;
    C = to_set & 1;
    return to_set;
}


//command
command::command(): data(0), disposition(0), tracefile(nullptr), br_trace(nullptr), program_counter(0)
{
}

command::command(int to_data, char to_disposition, char * to_tracefile): data(to_data), disposition(to_disposition), tracefile(to_tracefile), program_counter(0)
{
    char name_tmp[] = "branch_trace.out";
    br_trace = new char[strlen(name_tmp)];
    strcpy(br_trace, name_tmp);
    //br_trace = "branch_trace.out";
}

void command::disp()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
}


int command::instruction(uint16_t *regs, CPSR *states, i_cache *program)
{
    return 0;
}

int command::instructionB(uint16_t *regs, CPSR *states, i_cache * program)
{
    return 0;
}

void command::set_br_trace(char * to_set)
{
    if(br_trace)
        delete [] br_trace;
    br_trace = new char[strlen(to_set)];
    strcpy(br_trace, to_set);
}


command::~command() {
    if(br_trace)
        delete [] br_trace;
}

int command::fetch_display(uint16_t *regs, CPSR *states)
{
    return 0;
}

//extended
extended::extended(): command(), function_code(0), destination(0), source_mode(0), source(0)
{
}

extended::extended(int to_data, char disposition, char * to_tracefile, int to_function_code, int to_destination, int to_source_mode, int to_source): command(to_data, disposition, to_tracefile), function_code(to_function_code), destination(to_destination), source_mode(to_source_mode), source(to_source)
{
}

void extended::disp()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
    cout << "function code: " << function_code << endl;
    cout << "destination" << destination << endl;
    cout << "source_mode: " << source_mode << endl;
    cout << "source: " << source << endl;
}

int extended::instruction(uint16_t *regs, CPSR *states, i_cache * program)
{
    switch(function_code)
    {
        case MUL:
            {
                cout << "MUL" << endl;
                break;
            }
        case DIV:
            {
                cout << "DIV" << endl;
                break;
            }
        case ASH:
            {
                cout << "ASH" << endl;
                break;
            }
        case ASHC:
            {
                cout << "ASHC" << endl;
                break;
            }
        case XOR:
            {
                cout << "XOR" << endl;
                break;
            }
        default:
            {
                cout << "invalid" << endl;
                break;
            }
    }

    return 0;
}

int extended::fetch_display(uint16_t *regs, CPSR *states)
{
    return 0;
}

extended::~extended()
{
}

//trapIntMiscCond
trapIntMiscCond::trapIntMiscCond(): command(), function_code(0), trap_code(0)
{
}


trapIntMiscCond::trapIntMiscCond(int to_data, char to_disposition, char * to_tracefile, int to_function_code, int to_trap_code): command(to_data, to_disposition, to_tracefile), function_code(to_function_code), trap_code(to_trap_code)
{
}

void trapIntMiscCond::disp()
{
    cout << "data: " << data << endl;
    cout << "dispostion: " << disposition << endl;
    cout << "function code: " << function_code << endl;
    cout << "trap code: " << trap_code << endl;
}

int trapIntMiscCond::instruction(uint16_t *regs, CPSR *states, i_cache *program)
{
    char answer[25];

   switch(function_code)
    {
        case TRAP:
            {
                cout << "TRAP" << endl;
                break;
            }
        case RTI:
            {
                cout << "RTI" << endl;
                break;
            }
        case HALT:
            {
                cin.sync();
                cout << "End program?(Y/n) ";
                cin.getline(answer, 25);
                cin.sync();

                if(toupper(answer[0]) != 'N')
                    regs[PC] = I_SIZE;  //set to 65535, so next fetch
                                        //ends execution
                break;
            }
        case WAIT:
            {
                cin.sync();
                cout << "End program?(y/N) ";
                cin.getline(answer, 25);
                cin.sync();

                if(toupper(answer[0]) != 'Y')
                    regs[PC] = I_SIZE;  //set to 65535, so next fetch
                                        //ends execution

               break;
            }
        case RESET:
            {
                break;
            }
        case CLC:
            {
                states->set_condition(states->get_condition() & ~CARRY);
                break;
            }
        case CLV:
            {
                states->set_condition(states->get_condition() & ~V_OVERFLOW);
                break;
            }
        case CLZ:
            {
                states->set_condition(states->get_condition() & ~ZERO);
                break;
            }
        case CLN:
            {
                states->set_condition(states->get_condition() & ~NEGATIVE);
                break;
            }
        case CCC:
            {
                states->set_condition(states->get_condition() & 0xF0);
                break;
            }
        case SEC:
            {
                states->set_condition(states->get_condition() | CARRY);
                break;
            }
        case SEV:
            {
                states->set_condition(states->get_condition() | V_OVERFLOW);
                break;
            }
        case SEZ:
            {
                states->set_condition(states->get_condition() | ZERO);
                break;
            }
        case SEN:
            {
                states->set_condition(states->get_condition() | NEGATIVE);
                break;
            }
        case SCC:
            {
                states->set_condition(states->get_condition() | 0xF);
                break;
            }
        case NOP:
            {
                break;
            }
        default:
            {
                cout << "nope" << endl;
                break;
            }
    }
    return 0;
}

//display information when -v option set
int trapIntMiscCond::fetch_display(uint16_t *regs, CPSR *states)
{
    int condition;
    int i;

   switch(function_code)
    {
        case TRAP:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "TRAP" << endl;
                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case RTI:
            {
                cout << "RTI" << endl;
                break;
            }
        case HALT:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "HALT" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case WAIT:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "WAIT" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case RESET:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "RESET" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case CLC:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "CLC" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case CLV:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "CLV" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case CLZ:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "CLZ" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case CLN:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "CLN" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case CCC:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "CCC" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case SEC:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "SEC" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case SEV:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "SEV" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case SEZ:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "SEZ" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case SEN:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "SEN" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case SCC:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "SCC" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case NOP:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "NOP" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        default:
            {
                cout << "nope" << endl;
                break;
            }
    }
    return 0;
}

trapIntMiscCond::~trapIntMiscCond()
{
}


//jump_sub
jump_sub::jump_sub(): command(), function_code(0), linkage_reg(0), destination_code(0), destination(0), parameters(0)
{
}

jump_sub::jump_sub(int to_data, char to_disposition, char * to_tracefile, int to_function_code, int to_linkage_reg, int to_destination_code, int to_destination, int to_parameters): command(to_data, to_disposition, to_tracefile), function_code(to_function_code), linkage_reg(to_linkage_reg), destination_code(to_destination_code), destination(to_destination), parameters(to_parameters)
{
}

void jump_sub::disp()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
    cout << "function_code: " << function_code << endl;
    cout << "linkage_reg: " << linkage_reg << endl;
    cout << "destination_code: " << destination_code << endl;
    cout << "destination: " << destination << endl;
    cout << "parameters: " << parameters << endl;
}

int jump_sub::instruction(uint16_t *regs, CPSR *states, i_cache *program)
{
    uint16_t index;
    int deferred;
    char taken = 'y';
    char type[10];


   switch(function_code)
    {
        case JMP:
            {
                switch(destination_code) {
                    case 0: { //register mode illegal
                                cout << "illegal instruction\n";
                                break;
                            }

                    case 1: { //register deferred:
                                //jump to address in register
                                if(regs[destination] % 2)
                                {
                                    cout << "boundary error\n";
                                    break;
                                }
                                regs[PC] = regs[destination];
                                sprintf(type, "JMP");
                                branch_trace(br_trace, type, regs[PC], taken);
                                break;
                            }
                    case 2: {   //autoincrement:
                                //jump to memory location pointed to 
                                //by memory location in register then 
                                //increment register
                                if(regs[destination] % 2)
                                {
                                    cout << "boundary error\n";
                                    break;
                                }
                                regs[PC] = regs[destination];
                                regs[destination] += 2;
                                sprintf(type, "JMP");
                                branch_trace(br_trace, type, regs[PC], taken);
                                break;
                            }
                    case 3: {   //autoincrement deferred:
                                //jump to memory location pointed to by
                                //memory at location pointed to by register,
                                //then increment register
                                deferred = program[regs[destination]].data;
                                if(regs[destination] % 2)
                                {
                                    cout << "unaligned reference\n";
                                    break;
                                }

                                regs[PC] = deferred;
                                trace_file(tracefile, 0, regs[destination]);
                                sprintf(type, "JMP");
                                branch_trace(br_trace, type, regs[PC], taken);
                                regs[destination] += 2;
                                break;
                            }
                    case 4: {   //autodecrement:
                                //decrement register, then jump
                                //to memory location pointed to 
                                //by register
                                if(regs[destination] % 2)
                                {
                                    cout << "boundary error\n";
                                    break;
                                }
                                regs[destination] -= 2;
                                regs[PC] = regs[destination];
                                sprintf(type, "JMP");
                                branch_trace(br_trace, type, regs[PC], taken);
                                break;
                            }
                    case 5: {   //autodecrement deferred:
                                //decrement register, then jump to
                                //memory location pointed to by memory
                                //location pointed to by register
                                regs[destination] -= 2;
                                deferred = program[regs[destination]].data;
                                if((deferred % 2) || (regs[destination] % 2))
                                {
                                    cout << "boundary error\n";
                                    break;
                                }

                                regs[PC] = deferred;
                                trace_file(tracefile, 0, regs[destination]);
                                sprintf(type, "JMP");
                                branch_trace(br_trace, type, regs[PC], taken);
                                break;
                            }
                    case 6: {   //index: jump to 
                                //memory location pointed to by register 
                                //plus index, which is located just after
                                //instruction
                                if(destination == PC)
                                    index = regs[PC] + 2 + program[regs[PC]].data;
                                else index = regs[destination] + program[regs[PC]].data;
                                if(regs[destination])
                                {
                                    cout << "boundary error\n";
                                    regs[PC] += 2;
                                    break;
                                }

                                regs[PC] = index;
                                trace_file(tracefile, 0, regs[PC]);
                                sprintf(type, "JMP");
                                branch_trace(br_trace, type, regs[PC], taken);
                                break;
                            }
                    case 7: {   //index deferred:
                                //jump to memory location pointed to by 
                                //memory pointed to by register plus 
                                //index, which is located just after
                                //instruction

                                if(destination == PC)
                                    index = regs[PC] + 2 + program[regs[PC]].data;
                                else index = regs[destination] + program[regs[PC]].data;
                                if((index % 2) || (regs[destination]))
                                {
                                    cout << "boundary error\n";
                                    regs[PC] += 2;
                                    break;
                                }

                                regs[PC] = program[index].data;
                                trace_file(tracefile, 0, regs[PC]);
                                trace_file(tracefile, 0, index);
                                sprintf(type, "JMP");
                                branch_trace(br_trace, type, regs[PC], taken);
                                break;
                            }
                    default:
                            {
                                cout << "illegal instruction\n";
                                break;
                            }
                }

                break;
            }
        case JSR:
            {
                switch(destination_code) {
                    case 0: {   //register mode
                                //not legal
                                cout << "Illegal instruction\n";
                                break;
                            }
                    case 1: {   //register deferred:
                                //push linkage register contents 
                                //to stack, replace with location following 
                                //instruction, jump to address pointed to
                                //destination register
                                if(regs[destination] % 2)
                                {
                                    cout << "boundary error\n";
                                    break;
                                }
                                regs[SP] -= 2;
                                program[regs[SP]].data = regs[linkage_reg];
                                program[regs[SP] + 1].data = program[regs[SP]].data;
                                regs[linkage_reg] = regs[PC];
                                regs[PC] = regs[destination];
                                trace_file(tracefile, 1, regs[SP]);
                                sprintf(type, "JSR");
                                branch_trace(br_trace, type, regs[PC], taken);
                                break;
                            }
                    case 2: {   //autoincrement:
                                //push linkage register contents to stack,
                                //replace with location following 
                                //the instruction, jump to address pointed
                                //to by destination register, then
                                //increment register
                                if(regs[destination] % 2)
                                {
                                    cout << "boundary error\n";
                                    break;
                                }
                                regs[SP] -= 2;
                                program[regs[SP]].data = regs[linkage_reg];
                                program[regs[SP] + 1].data = program[regs[SP]].data;
                                regs[linkage_reg] = regs[PC];
                                regs[PC] = regs[destination];
                                trace_file(tracefile, 1, regs[SP]);
                                sprintf(type, "JSR");
                                branch_trace(br_trace, type, regs[PC], taken);
                                regs[destination] += 2;
                                break;
                            }
                    case 3: {   //autoincrement deferred:
                                //push linkage register contents to stack,
                                //replace with location following the 
                                //instruction, jump to address pointed to
                                //by address pointed to by destination
                                //register, then increment register
                                deferred = program[regs[destination]].data;
                                if(regs[destination] % 2)
                                {
                                    cout << "boundary error\n";
                                    break;
                                }
                                regs[SP] -= 2;
                                program[regs[SP]].data = regs[linkage_reg];
                                program[regs[SP] + 1].data = program[regs[SP]].data;
                                regs[linkage_reg] = regs[PC];
                                regs[PC] = program[regs[destination]].data;
                                trace_file(tracefile, 1, regs[SP]);
                                trace_file(tracefile, 0, regs[destination]);
                                sprintf(type, "JSR");
                                branch_trace(br_trace, type, regs[PC], taken);
                                regs[destination] += 2;
                                break;
                            }
                    case 4: {   //autodecrement:
                                //decrement destination register,
                                //push linkage register contents to stack,
                                //replace with location following instruction
                                //jump to address pointed to by destination
                                //register
                                if(regs[destination] % 2)
                                {
                                    cout << "boundary error\n";
                                    break;
                                }
                                regs[destination] -= 2;
                                regs[SP] -= 2;
                                program[regs[SP]].data = regs[linkage_reg];
                                program[regs[SP] + 1].data = program[regs[SP]].data;
                                regs[linkage_reg] = regs[PC];
                                regs[PC] = regs[destination];
                                trace_file(tracefile, 1, regs[SP]);
                                sprintf(type, "JSR");
                                branch_trace(br_trace, type, regs[PC], taken);
                                break;
                            }
                    case 5: {   //autodecrement deferred:
                                //decrement destination register,
                                //push linkage register contents to stack,
                                //replace with location following the 
                                //instruction, jump to address pointed to
                                //by address pointed to by destination
                                //register
                                deferred = program[regs[destination]].data;
                                if(regs[destination] % 2)
                                {
                                    cout << "boundary error\n";
                                    break;
                                }
                                regs[destination] -= 2;
                                regs[SP] -= 2;
                                program[regs[SP]].data = regs[linkage_reg];
                                program[regs[SP] + 1].data = program[regs[SP]].data;
                                regs[linkage_reg] = regs[PC];
                                regs[PC] = program[regs[destination]].data;
                                trace_file(tracefile, 1, regs[SP]);
                                trace_file(tracefile, 0, regs[destination]);
                                sprintf(type, "JSR");
                                branch_trace(br_trace, type, regs[PC], taken);
                                break;
                            }
                    case 6: {   //index: push linkage register contents to 
                                //stack, replace with location following
                                //location following PC, jump to address
                                //pointed to by register plus index
                                if(destination == PC)
                                    index = regs[PC] + 2 + program[regs[PC]].data;
                                else index = regs[destination] + program[regs[PC]].data;
                                if(regs[destination] % 2)
                                {
                                    cout << "boundary error\n";
                                    regs[PC] += 2;
                                    break;
                                }

                                regs[SP] -= 2;
                                program[regs[SP]].data = regs[linkage_reg];
                                program[regs[SP] + 1].data = program[regs[SP]].data;
                                regs[linkage_reg] = regs[PC];
                                regs[PC] = index;
                                trace_file(tracefile, 1, (regs[SP]));
                                trace_file(tracefile, 0, regs[PC]);
                                sprintf(type, "JSR");
                                branch_trace(br_trace, type, regs[PC], taken);
                                break;
                            }
                    case 7: {   //index deferred: push linkage register contents to 
                                //stack, replace with location following
                                //location following PC, jump to address
                                //pointed to by address pointed to by 
                                //register plus index
                                if(destination == PC)
                                    index = regs[PC] + 2 + program[regs[PC]].data;
                                else index = regs[destination] + program[regs[PC]].data;
                                if(index % 2 || (regs[destination] % 2))
                                {
                                    cout << "boundary error\n";
                                    regs[PC] += 2;
                                    break;
                                }

                                regs[SP] -= 2;
                                program[regs[SP]].data = regs[linkage_reg];
                                program[regs[SP] + 1].data = program[regs[SP]].data;
                                regs[linkage_reg] = regs[PC];
                                regs[PC] = program[index].data;
                                trace_file(tracefile, 1, regs[SP]);
                                trace_file(tracefile, 0, regs[PC]);
                                trace_file(tracefile, 0, index);
                                sprintf(type, "JSR");
                                branch_trace(br_trace, type, regs[PC], taken);
                                break;
                            }
                    default: {
                                 cout << "Illegal instruction\n";
                                 break;
                             }
                }


                break;
            }
        case RTS:
            {

                //register mode
                //load contents of register to PC                        
                //pop from stack to register
                if(regs[linkage_reg] % 2)
                {
                    cout << "boundary error\n";
                    break;
                }
                regs[PC] = regs[linkage_reg];
                regs[linkage_reg] = program[regs[SP]].data;
                trace_file(tracefile, 0, regs[SP]);
                sprintf(type, "RTS");
                branch_trace(br_trace, type, regs[PC], taken);
                regs[SP] += 2;
                break;
            }
        default:
            {
                cout << "nope" << endl;
                break;
            }
    }

    return 0;
}

//display information when -v option set
int jump_sub::fetch_display(uint16_t *regs, CPSR *states)
{
    int condition;
    int i;

   switch(function_code)
    {
        case JMP:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "JMP" << endl;
                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case JSR:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "JSR" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case RTS:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "RTS" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        default:
            {
                cout << "nope" << endl;
                break;
            }
    }
    return 0;
}

jump_sub::~jump_sub()
{
}


//branch
branch::branch(): command(), function_code(0), offset(0)
{
}

branch::branch(int to_data, char to_disposition, char * to_tracefile, int to_function_code,
               int to_offset): command(to_data, to_disposition, to_tracefile), function_code(to_function_code), offset(to_offset)
{
}

void branch::disp()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
    cout << "function code: " << function_code << endl;
    cout << "offset: " << offset << endl;
}

int branch::instruction(uint16_t *regs, CPSR *states, i_cache *program)
{
    char taken = 'y';
    char type[10];
    int condition = 0;

   switch(function_code)
    {
        case BR:
            {
                regs[PC] += (2 * (int8_t)offset);
                sprintf(type, "BR");
                branch_trace(br_trace, type, regs[PC], taken); 

                break;
            }
        case BNE:
            {
                taken = 'n';

                condition = states->get_condition() & ZERO;
                if(!condition)
                {
                    regs[PC] += (2 * (int8_t)offset);
                    taken = 'y';
                }
                sprintf(type, "BNE");
                branch_trace(br_trace, type, regs[PC], taken); 
                break;
            }
        case BEQ:
            {
                taken = 'n';

                condition = states->get_condition() & ZERO;
                if(condition)
                {
                    regs[PC] += (2 * (int8_t)offset);
                    taken = 'y';
                }
                sprintf(type, "BEQ");
                branch_trace(br_trace, type, regs[PC], taken); 
                break;
            }
        case BPL:
            {
                taken = 'n';

                condition = states->get_condition() & NEGATIVE;
                if(!condition)
                {
                    regs[PC] += (2 * (int8_t)offset);
                    taken = 'y';
                }
                sprintf(type, "BPL");
                branch_trace(br_trace, type, regs[PC], taken); 
                break;
            }
        case BMI:
            {
                taken = 'n';

                condition = states->get_condition() & NEGATIVE;
                if(condition)
                {
                    regs[PC] += (2 * (int8_t)offset);
                    taken = 'y';
                }
                sprintf(type, "BMI");
                branch_trace(br_trace, type, regs[PC], taken); 
                break;
            }
        case BVC:
            {
                taken = 'n';

                condition = states->get_condition() & V_OVERFLOW;
                if(!condition)
                {
                    regs[PC] += (2 * (int8_t)offset);
                    taken = 'y';
                }
                sprintf(type, "BVC");
                branch_trace(br_trace, type, regs[PC], taken); 
                break;
            }
        case BVS:
            {
                taken = 'n';

                condition = states->get_condition() & V_OVERFLOW;
                if(condition)
                {
                    regs[PC] += (2 * (int8_t)offset);
                    taken = 'y';
                }
                sprintf(type, "BVS");
                branch_trace(br_trace, type, regs[PC], taken); 
                break;
            }
        case BCC:
            {
                taken = 'n';

                condition = states->get_condition() & CARRY;
                if(!condition)
                {
                    regs[PC] += (2 * (int8_t)offset);
                    taken = 'y';
                }
                sprintf(type, "BCC");
                branch_trace(br_trace, type, regs[PC], taken); 
                break;
            }
        case BCS:
            {
                taken = 'n';

                condition = states->get_condition() & CARRY;
                if(condition)
                {
                    regs[PC] += (2 * (int8_t)offset);
                    taken = 'y';
                }
                sprintf(type, "BCS");
                branch_trace(br_trace, type, regs[PC], taken); 
                break;
            }
        case BGE:
            {   //branch if xnor N and V bits
                taken = 'n';
 
                //xor N and V condition bits
                condition = (states->get_condition() & V_OVERFLOW) >> 1;
                condition ^= ((states->get_condition() & NEGATIVE) >> 3);
                if(!condition)
                {
                    regs[PC] += (2 * (int8_t)offset);
                    taken = 'y';
                }
                sprintf(type, "BGE");
                branch_trace(br_trace, type, regs[PC], taken); 
               break;
            }
        case BLT:
            {   //branch if xor N and V bits
                taken = 'n';

                //xor N and V condition bits
                condition = (states->get_condition() & V_OVERFLOW) >> 1;
                condition ^= ((states->get_condition() & NEGATIVE) >> 3);
                if(condition)
                {
                    regs[PC] += (2 * (int8_t)offset);
                    taken = 'y';
                }
                sprintf(type, "BLT");
                branch_trace(br_trace, type, regs[PC], taken); 
                break;
            }
        case BGT:
            {   //branch if xnor N and V bits or Z bit

                taken = 'n';
 
                //xor N and V condition bits
                condition = (states->get_condition() & V_OVERFLOW) >> 1;
                condition ^= ((states->get_condition() & NEGATIVE) >> 3);
                condition |= ((states->get_condition() & ZERO) >> 2);
                if(!condition)
                {
                    regs[PC] += (2 * (int8_t)offset);
                    taken = 'y';
                }
                sprintf(type, "BGT");
                branch_trace(br_trace, type, regs[PC], taken); 
                break;
            }
        case BLE:
            {   //branch if xor N and V bits or Z bits
                taken = 'n';
 
                //xor N and V condition bits
                condition = (states->get_condition() & V_OVERFLOW) >> 1;
                condition ^= ((states->get_condition() & NEGATIVE) >> 3);
                condition |= ((states->get_condition() & ZERO) >> 2);
                if(condition)
                {
                    regs[PC] += (2 * (int8_t)offset);
                    taken = 'y';
                }
                sprintf(type, "BLE");
                branch_trace(br_trace, type, regs[PC], taken); 
                break;
            }
        case BHI:
            {   //branch if C & Z both zero
                taken = 'n';
 
                condition = (states->get_condition() & CARRY);
                condition |= (states->get_condition() & ZERO);
                if(!condition)
                {
                    regs[PC] += (2 * (int8_t)offset);
                    taken = 'y';
                }
                sprintf(type, "BHI");
                branch_trace(br_trace, type, regs[PC], taken); 
                break;
 
                break;
            }
        case BLOS:
            {   //branch if C | Z set
                taken = 'n';
  
                condition = (states->get_condition() & CARRY);
                condition |= (states->get_condition() & ZERO);
                if(condition)
                {
                    regs[PC] += (2 * (int8_t)offset);
                    taken = 'y';
                }
                sprintf(type, "BLOS");
                branch_trace(br_trace, type, regs[PC], taken); 
                break;
            }
        default:
            {
                cout << "nope" << endl;
                break;
            }
    }
    return 0;
}

//display information when -v option set
int branch::fetch_display(uint16_t *regs, CPSR *states)
{
    int condition;
    int i;

   switch(function_code)
    {
        case BR:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BR" << endl;
                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case BNE:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BNE" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case BEQ:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BEQ" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case BPL:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BPL" << endl;
                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case BMI:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BMI" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case BVC:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BVC" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }

        case BVS:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BVS" << endl;
                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case BCC:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BCC" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case BCS:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BCS" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }

        case BGE:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BGE" << endl;
                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case BLT:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BLT" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case BGT:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BGT" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }

        case BLE:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BLE" << endl;
                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case BHI:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BHI" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case BLOS:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BLOS" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }


        default:
            {
                cout << "nope" << endl;
                break;
            }
    }
    return 0;
}

branch::~branch()
{
}


//single operand
single_operand::single_operand(): command(), function_code(0), destination_mode(0), destination(0)
{
}

single_operand::single_operand(int to_data, char to_disposition, char * to_tracefile, int to_function_code, int to_destination_mode, int to_destination): command(to_data, to_disposition, to_tracefile), function_code(to_function_code), destination_mode(to_destination_mode), destination(to_destination)
{
}

void single_operand::disp()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
    cout << "function code: " << function_code << endl;
    cout << "destination_mode: " << destination_mode << endl;
    cout << "destination: " << destination << endl;
}

int single_operand::instruction(uint16_t *regs, CPSR *states, i_cache *program)
{
    int outcome = -1;
    int deferred = 0;   //temp for deferred addressing
    uint16_t index = 0;      //temp for index addressing
    int condition;      //temp for condition codes

    switch(function_code)
    {
        case CLR:
            {
                switch (destination_mode) {
                    case 0: {           //register:
                                        //set register to 0
                        regs[destination] = 0;
                        states->set_condition(4);
                        break;
                    }
                    case 1: {           //register deferred:
                                        //set memory location pointed to by
                                        //register to 0
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        program[regs[destination]].data = 0;
                        program[regs[destination] + 1].data = 0;
                        states->set_condition(4);
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }
                    case 2: {           //autoincrement:
                                        //set memory location pointed to by
                                        //register to 0, then increment register
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        program[regs[destination]].data = 0;
                        program[regs[destination] + 1].data = 0;
                        trace_file(tracefile, 1, regs[destination]);    //write data write to trace file
                        regs[destination] += 2;
                        states->set_condition(4);
                        outcome = 1;
                        break;
                    }
                    case 3:         //autoincrement deferred:
                                    //set memory location pointed to by
                                    //memory at location pointed to by
                                    //register, then increment register
                    {
                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        
                        program[deferred].data = 0;
                        program[deferred + 1].data = 0;
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        states->set_condition(4);
                        outcome = 1;
                        break;
                    }
                    case 4:         //autodecrement:
                                    //decrement register, then
                                    //set memory location pointed to by
                                    //register to 0
                    {
                        regs[destination] -= 2;
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        program[regs[destination]].data = 0;
                        program[regs[destination] + 1].data = 0;
                        states->set_condition(4);
                        trace_file(tracefile, 1, regs[destination]);
                        break;
                    }
                    case 5: {       //autodecrement deferred:
                                    //decrement register, then
                                    //set memory location pointed to by
                                    //memory location pointed to by register
                                    //to 0
                        regs[destination] -= 2;
                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        program[deferred].data = 0;
                        program[deferred + 1].data = 0;
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        states->set_condition(4);
                        outcome = 1;
                        break;
                    }
                    case 6: {       //index:
                                    //set memory pointed to by register plus
                                    //index, which is located just after instruction,
                                    //to 0
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (regs[destination]))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }


                        program[index].data = 0;        
                        program[index + 1].data = 0;    
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 1, index);
                        regs[PC] += 2;
                        states->set_condition(4);
                        outcome = 1;
                        break;
                    }
                    case 7: {       //index deferred:
                                    //set memory pointed to by memory pointed to
                                    //by register plus index, which is located just after
                                    //instruction, to 0
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (program[index].data % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }


                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 0, regs[PC]);
                        program[program[index].data].data = 0;
                        program[program[index].data + 1].data = 0;
                        trace_file(tracefile, 1, program[index].data);
                        regs[PC] += 2;
                        states->set_condition(4);
                        outcome = 1;
                        break;
                    }
                    default:
                        {
                             cout << "invalid destination mode" << endl;
                             break;
                        }
                }


                break;
            }
        case CLRB:
            {
                cout << "CLRB" << endl;
                break;
            }
        case COM:
            {
                switch (destination_mode) {
                    case 0: {           //register:
                                        //flip register bits
                        regs[destination] ^= 0xFFFF;     //flip bits
                        states->set_condition(((regs[destination] >> 15) << 3)  | (!(regs[destination]) << 2) | 1);
                        break;
                    }
                    case 1: {           //register deferred:
                                        //flip bits at memory location 
                                        //pointed to by register
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        program[regs[destination]].data ^= 0xFFFF;   //flip bits
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        states->set_condition(((program[regs[destination]].data >> 15) << 3)  | (!(program[regs[destination]].data) << 2) | 1);
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }
                    case 2: {           //autoincrement:
                                        //flip bits at memory location pointed to by
                                        //register, then increment register
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        program[regs[destination]].data ^= 0xFFFF;
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        trace_file(tracefile, 0, regs[destination]);    //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);    //write data write to trace file
                        regs[destination] += 2;
                        states->set_condition(((program[regs[destination]].data >> 15) << 3)  | (!(program[regs[destination]].data) << 2) | 1);
                        outcome = 1;
                        break;
                    }
                    case 3:         //autoincrement deferred:
                                    //set memory location pointed to by
                                    //memory at location pointed to by
                                    //register, then increment register
                    {
                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        program[deferred].data ^= 0xFFFF;
                        program[deferred + 1].data = program[deferred].data;
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        condition = 0;
                        states->set_condition(((program[deferred].data >> 15) << 3)  | (!(program[deferred].data) << 2) | 1);
                        outcome = 1;
                        break;
                    }
                    case 4:         //autodecrement:
                                    //decrement register, then
                                    //set memory location pointed to by
                                    //register to 0
                    {
                        regs[destination] -= 2;
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        program[regs[destination]].data ^= 0xFFFF;
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        states->set_condition(((program[regs[destination]].data >> 15) << 3)  | (!(program[regs[destination]].data) << 2) | 1);
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 1, regs[destination]);
                        break;
                    }
                    case 5: {       //autodecrement deferred:
                                    //decrement register, then
                                    //set memory location pointed to by
                                    //memory location pointed to by register
                                    //to 0
                        regs[destination] -= 2;
                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        program[deferred].data ^= 0xFFFF;
                        program[deferred + 1].data = program[deferred].data;
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        states->set_condition(((program[deferred].data >> 15) << 3)  | (!(program[deferred].data) << 2) | 1);
                        outcome = 1;
                        break;
                    }
                    case 6: {       //index:
                                    //set memory pointed to by register plus
                                    //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }
                        program[index].data ^= 0xFFFF;
                        program[index + 1].data = program[index].data;
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 1, index);
                        regs[PC] += 2;
                        states->set_condition(((program[index + 1].data >> 15) << 3)  | (!(program[index].data) << 2) | 1);
                        outcome = 1;
                        break;
                    }
                    case 7: {       //index deferred:
                                    //set memory pointed to by memory pointed to
                                    //by register plus index, which is located just after
                                    //instruction, to 0
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (program[index].data % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }
                        program[program[index].data].data ^= 0xFFFF;
                        program[program[index].data + 1].data = program[program[index].data].data;
                    
                        regs[PC] += 2;
                        states->set_condition(((program[program[index].data].data >> 15) << 3)  | (!(program[program[index].data].data) << 2) | 1);
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 0, program[index].data);
                        trace_file(tracefile, 1, program[index].data);

                        outcome = 1;
                        break;
                    }
                    default:
                        {
                             cout << "invalid destination mode" << endl;
                             break;
                        }
                }
                break;
            }
        case COMB:
            {
                cout << "COMB" << endl;
                break;
            }
        case INC:
            {
                switch (destination_mode) {
                    case 0: {           //register:
                        //add 1 to signed value
                        condition = states->get_condition() & 1;
                        if ((regs[destination]) == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        regs[destination] += 1;
                        if (!(regs[destination]))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)regs[destination] < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        break;
                    }
                    case 1: {           //register deferred:
                        //increment at memory location
                        //pointed to by register
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = states->get_condition() & CARRY;    //get CARRY condition bit
                        program[regs[destination]].data += 1;
                        if (program[regs[destination] + 1].data == 0077777)        //increment and check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }
                    case 2: {           //autoincrement:
                        //increment at memory location pointed to by
                        //register, then increment register
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = states->get_condition() & 1;
                        if (program[regs[destination]].data == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[regs[destination]].data += 1;
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 3:         //autoincrement deferred:
                        //set memory location pointed to by
                        //memory at location pointed to by
                        //register, then increment register
                    {
                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = states->get_condition() & 1;
                        if (program[deferred].data == 0077777)        //increment and check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[deferred].data += 1;
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[deferred].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 4:         //autodecrement:
                        //decrement register, then
                        //set memory location pointed to by
                        //register to 0
                    {
                        regs[destination] -= 2;
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        condition = states->get_condition() & 1;
                        if (program[regs[destination]].data == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[regs[destination]].data += 1;
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        break;
                    }
                    case 5: {       //autodecrement deferred:
                        //decrement register, then
                        //set memory location pointed to by
                        //memory location pointed to by register
                        //to 0
                        regs[destination] -= 2;
                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        condition = states->get_condition() & 1;
                        if (program[deferred].data == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[deferred].data += 1;
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[deferred].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        break;
                    }
                    case 6: {       //index:
                        //set memory pointed to by register plus
                        //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }
                        condition = states->get_condition() & 1;
                        if (program[index].data == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[index].data += 1;
                        program[index + 1].data = program[index].data;
                        if (!(program[index].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[index].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 1, index);
                        regs[PC] += 2;
                        outcome = 1;
                        break;
                    }
                    case 7: {       //index deferred:
                        //set memory pointed to by memory pointed to
                        //by register plus index, which is located just after
                        //instruction, to 0
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (program[index].data % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = states->get_condition() & 1;    //increment
                        if (program[program[index].data].data == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= V_OVERFLOW;
                        program[program[index].data].data += 1;     //increment
                        program[program[index].data + 1].data = program[program[index].data].data;
                        if (!(program[program[index].data].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[program[index].data].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 0, program[index].data);
                        trace_file(tracefile, 1, program[index].data);
                        
                        regs[PC] += 2;

                        outcome = 1;
                        break;
                    }
                    default: {
                        cout << "invalid destination mode" << endl;
                        break;
                    }
                }
                break;
            }
        case INCB:
            {
                cout << "INCB" << endl;/*
                switch (destination_mode) {
                    case 0: {           //register:
                        //add 1 to signed value
                        condition = states->get_condition() & 1;
                        if ((regs[destination]) == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        regs[destination] = ((regs[destination] & 0xFF) + 1) & 0xFFFF;
                        if (!(regs[destination]))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)regs[destination] < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        break;
                    }
                    case 1: {           //register deferred:
                        //increment at memory location
                        //pointed to by register
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = states->get_condition() & CARRY;    //get CARRY condition bit
                        program[regs[destination]].data += 1;
                        if (program[regs[destination] + 1].data == 0077777)        //increment and check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }
                    case 2: {           //autoincrement:
                        //increment at memory location pointed to by
                        //register, then increment register
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = states->get_condition() & 1;
                        if (program[regs[destination]].data == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[regs[destination]].data += 1;
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 3:         //autoincrement deferred:
                        //set memory location pointed to by
                        //memory at location pointed to by
                        //register, then increment register
                    {
                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = states->get_condition() & 1;
                        if (program[deferred].data == 0077777)        //increment and check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[deferred].data += 1;
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[deferred].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 4:         //autodecrement:
                        //decrement register, then
                        //set memory location pointed to by
                        //register to 0
                    {
                        regs[destination] -= 2;
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        condition = states->get_condition() & 1;
                        if (program[regs[destination]].data == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[regs[destination]].data += 1;
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        break;
                    }
                    case 5: {       //autodecrement deferred:
                        //decrement register, then
                        //set memory location pointed to by
                        //memory location pointed to by register
                        //to 0
                        regs[destination] -= 2;
                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        condition = states->get_condition() & 1;
                        if (program[deferred].data == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[deferred].data += 1;
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[deferred].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        break;
                    }
                    case 6: {       //index:
                        //set memory pointed to by register plus
                        //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }
                        condition = states->get_condition() & 1;
                        if (program[index].data == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[index].data += 1;
                        program[index + 1].data = program[index].data;
                        if (!(program[index].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[index].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 1, index);
                        regs[PC] += 2;
                        outcome = 1;
                        break;
                    }
                    case 7: {       //index deferred:
                        //set memory pointed to by memory pointed to
                        //by register plus index, which is located just after
                        //instruction, to 0
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (program[index].data % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = states->get_condition() & 1;    //increment
                        if (program[program[index].data].data == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= V_OVERFLOW;
                        program[program[index].data].data += 1;     //increment
                        program[program[index].data + 1].data = program[program[index].data].data;
                        if (!(program[program[index].data].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[program[index].data].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 0, program[index].data);
                        trace_file(tracefile, 1, program[index].data);
                        
                        regs[PC] += 2;

                        outcome = 1;
                        break;
                    }
                    default: {
                        cout << "invalid destination mode" << endl;
                        break;
                    }
                }*/
                break;
            }
        case DEC:
            {
                switch (destination_mode) {
                    case 0: {           //register:
                        //subtract 1 from signed value
                        condition = states->get_condition() & 1;
                        if ((regs[destination]) == 0100000)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        regs[destination] -= 1;
                        if (!(regs[destination]))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)regs[destination] < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        break;
                    }
                    case 1: {           //register deferred:
                        //increment at memory location
                        //pointed to by register
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = states->get_condition() & CARRY;    //get CARRY condition bit
                        if (program[regs[destination]].data == 0100000)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[regs[destination]].data -= 1;
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }
                    case 2: {           //autoincrement:
                        //decrement at memory location pointed to by
                        //register, then increment register
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = states->get_condition() & 1;
                        if (program[regs[destination]].data == 0100000)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[regs[destination]].data -= 1;
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 3:         //autoincrement deferred:
                        //set memory location pointed to by
                        //memory at location pointed to by
                        //register, then increment register
                    {
                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = states->get_condition() & 1;
                        if (program[deferred].data == 0100000)        //increment and check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[deferred].data -= 1;
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[deferred].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 4:         //autodecrement:
                        //decrement register, then
                        //set memory location pointed to by
                        //register
                    {
                        regs[destination] -= 2;
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        condition = states->get_condition() & 1;
                        if (program[regs[destination]].data == 0100000)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[regs[destination]].data -= 1;
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        break;
                    }
                    case 5: {       //autodecrement deferred:
                        //decrement register, then
                        //set memory location pointed to by
                        //memory location pointed to by register
                        //to 0
                        regs[destination] -= 2;
                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        condition = states->get_condition() & 1;
                        if (program[deferred].data == 0100000)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[deferred].data -= 1;
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[deferred].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        break;
                    }
                    case 6: {       //index:
                        //set memory pointed to by register plus
                        //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }
                        condition = states->get_condition() & 1;
                        if (program[index].data == 0100000)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[index].data -= 1;
                        program[index + 1].data = program[index].data;
                        if (!(program[index].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[index].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, (uint16_t)index);
                        trace_file(tracefile, 1, (uint16_t)index);
                        regs[PC] += 2;
                        outcome = 1;
                        break;
                    }
                    case 7: {       //index deferred:
                        //set memory pointed to by memory pointed to
                        //by register plus index, which is located just after
                        //instruction, to 0
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (program[index].data % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = states->get_condition() & 1;    //increment
                        if (program[program[index].data].data == 0100000)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[program[index].data].data -= 1;     //increment
                        program[program[index].data + 1].data = program[program[index].data].data;
                        if (!(program[program[index].data].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[program[index].data].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, program[index].data);
                        trace_file(tracefile, 1, program[index].data);
                        
                        regs[PC] += 2;

                        outcome = 1;
                        break;
                    }
                    default: {
                        cout << "invalid destination mode" << endl;
                        break;
                    }
                }
                break;
            }
        case DECB:
            {
                cout << "DECB" << endl;
                break;
            }
        case NEG:
            {
                switch (destination_mode) {
                    case 0: {           //register:
                        //invert signed value
                        condition = states->get_condition();
                        if ((regs[destination]) == 0100000)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        regs[destination] = ~regs[destination] +1;
                        if (!(regs[destination]))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)regs[destination] < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        if (regs[destination])
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        states->set_condition(condition);       //set conditions
                        break;
                    }
                    case 1: {           //register deferred:
                        //increment at memory location
                        //pointed to by register
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = states->get_condition();    //get condition bit
                        if (program[regs[destination]].data == 0100000)        //overflow if -128
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[regs[destination]].data = ~program[regs[destination]].data + 1;
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;
                        if (program[regs[destination]].data)
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }
                    case 2: {           //autoincrement:
                        //increment at memory location pointed to by
                        //register, then increment register
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = states->get_condition();    //get condition bit
                        if (program[regs[destination]].data == 0100000)        //overflow if -128
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[regs[destination]].data = ~program[regs[destination]].data + 1;
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;
                        if (program[regs[destination]].data)
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 3:         //autoincrement deferred:
                        //set memory location pointed to by
                        //memory at location pointed to by
                        //register, then increment register
                    {
                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = states->get_condition();    //get condition bit
                        if (program[deferred].data == 0100000)        //overflow if -128
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[deferred].data = ~program[deferred].data + 1;
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[deferred].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;
                        if (program[deferred].data)
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 4:         //autodecrement:
                        //decrement register, then
                        //set memory location pointed to by
                        //register to 0
                    {
                        regs[destination] -= 2;
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        condition = states->get_condition();    //get condition bit
                        if (program[regs[destination]].data == 0100000)        //increment and check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[regs[destination]].data = ~program[regs[destination]].data + 1;
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;
                        if (program[regs[destination]].data)
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        break;
                    }
                    case 5: {       //autodecrement deferred:
                        //decrement register, then
                        //set memory location pointed to by
                        //memory location pointed to by register
                        //to 0
                        regs[destination] -= 2;
                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        condition = states->get_condition();    //get condition bit
                        if (program[deferred].data == 0100000)        //overflow if -128
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[deferred].data = ~program[deferred].data + 1;
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[deferred].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;
                        if (program[deferred].data)
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        break;
                    }
                    case 6: {       //index:
                        //set memory pointed to by register plus
                        //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((regs[destination] % 2) || (index % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }
                        condition = states->get_condition();    //get condition bit
                        if (program[index].data == 0100000)        //overflow if -128
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[index].data = ~program[index].data + 1;
                        program[index + 1].data = program[index].data;
                        if (!(program[index].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[index].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;
                        if (program[index].data)
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 1, index);
                        regs[PC] += 2;
                        outcome = 1;
                        break;
                    }
                    case 7: {       //index deferred:
                        //set memory pointed to by memory pointed to
                        //by register plus index, which is located just after
                        //instruction, to 0
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (program[index].data % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = states->get_condition();    //get condition bit
                        if (program[program[index].data].data == 0100000)        //overflow if -128
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[program[index].data].data = ~program[program[index].data].data + 1;
                        program[program[index].data + 1].data = program[program[index].data].data;
                        if (!(program[program[index].data].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[program[index].data].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;
                        if (program[program[index].data].data)
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, program[index].data);
                        trace_file(tracefile, 1, program[index].data);
                        
                        regs[PC] += 2;

                        outcome = 1;
                        break;
                    }
                    default: {
                        cout << "invalid destination mode" << endl;
                        break;
                    }
                }
                break;
            }
        case NEGB:
            {
                cout << "NEGB" << endl;
                break;
            }
        case TST:
            {
                switch (destination_mode) {
                    case 0: {           //register:
                        // signed value
                        condition = states->get_condition() & ~(V_OVERFLOW | CARRY);
                        if (!(regs[destination]))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)regs[destination] < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        break;
                    }
                    case 1: {           //register deferred:
                        //look at memory location
                        //pointed to by register
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = states->get_condition() & ~(V_OVERFLOW | CARRY);    //get condition bit
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }
                    case 2: {           //autoincrement:
                        //increment at memory location pointed to by
                        //register, then increment register
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = states->get_condition() & ~(CARRY | V_OVERFLOW);    //get condition bit
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;

                        trace_file(tracefile, 0, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 3:         //autoincrement deferred:
                        //set memory location pointed to by
                        //memory at location pointed to by
                        //register, then increment register
                    {
                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = states->get_condition() & ~(V_OVERFLOW | CARRY);    //get condition bit
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[deferred].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 4:         //autodecrement:
                        //decrement register, then
                        //set memory location pointed to by
                        //register to 0
                    {
                        regs[destination] -= 2;
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        condition = states->get_condition() & ~(CARRY | V_OVERFLOW);    //get condition bit
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //write data write to trace file
                        break;
                    }
                    case 5: {       //autodecrement deferred:
                        //decrement register, then
                        //set memory location pointed to by
                        //memory location pointed to by register
                        //to 0
                        regs[destination] -= 2;
                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        condition = states->get_condition() & ~(CARRY | V_OVERFLOW);    //get condition bit
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[deferred].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        break;
                    }
                    case 6: {       //index:
                        //get memory pointed to by register plus
                        //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }
                        condition = states->get_condition() & ~(CARRY | V_OVERFLOW);    //get condition bit
                        if (!(program[index].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[index].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        regs[PC] += 2;
                        outcome = 1;
                        break;
                    }
                    case 7: {       //index deferred:
                        //get memory pointed to by memory pointed to
                        //by register plus index, which is located just after
                        //instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (program[index].data % 2) || regs[destination])
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = states->get_condition() & ~(CARRY | V_OVERFLOW);    //get condition bit
                        if (!(program[program[index].data].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[program[index].data].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, program[index].data);
                        
                        regs[PC] += 2;

                        outcome = 1;
                        break;
                    }
                    default: {
                        cout << "invalid destination mode" << endl;
                        break;
                    }
                }
                break;
            }
        case TSTB:
            {
                cout << "TSTB" << endl;
                break;
            }
        case ASR:
            {
                switch (destination_mode) {
                    case 0: {           //register:
                        // arithmetic shift left 16 bit
                        condition = regs[destination] & CARRY;    //move bit 0 to C condition code
                        regs[destination] = regs[destination] >> 1; //shift right
                        regs[destination] |= (((regs[destination] >> 14) & 1) << 15);     //return sign bit to bit 15
                        
                        if (!(regs[destination]))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((regs[destination] >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions
                        break;
                    }
                    case 1: {           //register deferred:
                        //look at memory location
                        //pointed to by register:
                        // arithmetic shift left 16 bit
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = program[regs[destination]].data & CARRY;    //move bit 0 to C condition code
                        program[regs[destination]].data = program[regs[destination]].data >> 1; //shift right
                        program[regs[destination]].data |= (((program[regs[destination]].data >> 14) & 1) << 15);     //return sign bit to bit 15
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }
                    case 2: {           //autoincrement:
                        //shift left at memory location pointed to by
                        //register, then increment register

                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = program[regs[destination]].data & CARRY;    //move bit 0 to C condition code
                        program[regs[destination]].data = program[regs[destination]].data >> 1; //shift right
                        program[regs[destination]].data |= (((program[regs[destination]].data >> 14) & 1) << 15);     //return sign bit to bit 15
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;


                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 3:         //autoincrement deferred:
                        //set memory location pointed to by
                        //memory at location pointed to by
                        //register, then increment register
                    {

                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = program[deferred].data & CARRY;    //move bit 0 to C condition code
                        program[deferred].data = program[deferred].data >> 1; //shift right
                        program[deferred].data |= (((program[deferred].data >> 14) & 1) << 15);     //return sign bit to bit 15
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[deferred].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 4:         //autodecrement:
                        //decrement register, then
                        //set memory location pointed to by
                        //register
                    {
                        regs[destination] -= 2;
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }

                        condition = program[regs[destination]].data & CARRY;    //move bit 0 to C condition code
                        program[regs[destination]].data = program[regs[destination]].data >> 1; //shift right
                        program[regs[destination]].data |= (((program[regs[destination]].data >> 14) & 1) << 15);     //return sign bit to bit 15
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;


                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }

                    case 5: {       //autodecrement deferred:
                        //decrement register, then
                        //set memory location pointed to by
                        //memory location pointed to by register
                        regs[destination] -= 2;

                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        condition = program[deferred].data & CARRY;    //move bit 0 to C condition code
                        program[deferred].data = program[deferred].data >> 1; //shift right
                        program[deferred].data |= (((program[deferred].data >> 14) & 1) << 15);     //return sign bit to bit 15
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[deferred].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions

                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        break;
                    }
                    case 6: {       //index:
                        //set memory pointed to by register plus
                        //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = program[index].data & CARRY;    //move bit 0 to C condition code
                        program[index].data = program[index].data >> 1; //shift right
                        program[index].data |= (((program[index].data >> 14) & 1) << 15);     //return sign bit to bit 15
                        program[index + 1].data = program[index].data;
                        if (!(program[index].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[index].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions

                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 1, index);
                        regs[PC] += 2;
                        outcome = 1;
                        break;
                    }
                    case 7: {       //index deferred:
                        //set memory pointed to by memory pointed to
                        //by register plus index, which is located just after
                        //instruction, to 0
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (program[index].data % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = program[index].data & CARRY;    //move bit 0 to C condition code
                        program[program[index].data].data = program[program[index].data].data >> 1; //shift right
                        program[program[index].data].data |= (((program[program[index].data].data >> 14) & 1) << 15);     //return sign bit to bit 15
                        program[program[index].data + 1].data = program[program[index].data].data;
                        if (!(program[program[index].data].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[program[index].data].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions

                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, program[index].data);
                        trace_file(tracefile, 1, program[index].data);
                        
                        regs[PC] += 2;

                        outcome = 1;
                        break;
                    }
                    default: {
                        cout << "invalid destination mode" << endl;
                        break;
                    }
                }
                break;
            }

        case ASRB:
            {
                cout << "ASRB" << endl;
                break;
            }
        case ASL:
            {
                switch (destination_mode) {
                    case 0: {           //register:
                        // arithmetic shift left 16 bit
                        condition = (regs[destination] >> 15) & CARRY;    //move bit 15 to C condition code
                        regs[destination] = regs[destination] << 1; //shift left
                        
                        if (!(regs[destination]))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((regs[destination] >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions
                        break;
                    }
                    case 1: {           //register deferred:
                        //look at memory location
                        //pointed to by register:
                        // arithmetic shift left 16 bit
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = (program[regs[destination]].data >> 15) & CARRY;    //move bit 15 to C condition code
                        program[regs[destination]].data = program[regs[destination]].data << 1; //shift right
                        program[regs[destination] + 1].data = program[regs[destination]].data;  //store high byte
                        
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }
                    case 2: {           //autoincrement:
                        //increment at memory location pointed to by
                        //register, then increment register

                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = (program[regs[destination]].data >> 15) & CARRY;    //move bit 15 to C condition code
                        program[regs[destination]].data = program[regs[destination]].data >> 1; //shift right
                        program[regs[destination] + 1].data = program[regs[destination]].data;  //store high byte
                        
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;


                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 3:         //autoincrement deferred:
                        //set memory location pointed to by
                        //memory at location pointed to by
                        //register, then increment register
                    {

                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = (program[deferred].data >> 15) & CARRY;    //move bit 15 to C condition code
                        program[deferred].data = program[deferred].data << 1; //shift left
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[deferred].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 4:         //autodecrement:
                        //decrement register, then
                        //set memory location pointed to by
                        //register
                    {
                        regs[destination] -= 2;
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }

                        condition = (program[regs[destination]].data >> 15) & CARRY;    //move bit 15 to C condition code
                        program[regs[destination]].data = program[regs[destination]].data << 1; //shift left
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;


                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }

                    case 5: {       //autodecrement deferred:
                        //decrement register, then
                        //set memory location pointed to by
                        //memory location pointed to by register

                        regs[destination] -= 2;

                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        condition = (program[deferred].data >> 15) & CARRY;    //move bit 0 to C condition code
                        program[deferred].data = program[deferred].data << 1; //shift left
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[deferred].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions

                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        break;
                    }
                    case 6: {       //index:
                        //set memory pointed to by register plus
                        //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = (program[index].data >> 15) & CARRY;    //move bit 0 to C condition code
                        program[index].data = program[index].data << 1; //shift left
                        program[index + 1].data = program[index].data;
                        if (!(program[index].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[index].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions

                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 1, index);
                        regs[PC] += 2;
                        outcome = 1;
                        break;
                    }
                    case 7: {       //index deferred:
                        //set memory pointed to by memory pointed to
                        //by register plus index, which is located just after
                        //instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (program[index].data % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = (program[index].data >> 15) & CARRY;    //move bit 0 to C condition code
                        program[program[index].data].data = program[program[index].data].data << 1; //shift left
                        program[program[index].data + 1].data = program[program[index].data].data;
                        if (!(program[program[index].data].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[program[index].data].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions

                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, program[index].data);
                        trace_file(tracefile, 1, program[index].data);
                        
                        regs[PC] += 2;

                        outcome = 1;
                        break;
                    }
                    default: {
                        cout << "invalid destination mode" << endl;
                        break;
                    }
                }

                break;
            }
        case ASLB:
            {
                cout << "ASLB" << endl;
                break;
            }
        case ROR:
            {
                switch (destination_mode) {
                    case 0: {           //register:
                        // rotate right 1 bit
                        condition = regs[destination] & CARRY;    //move bit 0 to C condition code
                        regs[destination] = regs[destination] >> 1; //shift right
                        regs[destination] |= ((condition & CARRY) << 15);     //move carry bit to bit 15
                        
                        if (!(regs[destination]))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((regs[destination] >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions
                        break;
                    }
                    case 1: {           //register deferred:
                        //look at memory location
                        //pointed to by register:
                        //rotate right 1 bit
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = program[regs[destination]].data & CARRY;    //move bit 0 to C condition code
                        program[regs[destination]].data = program[regs[destination]].data >> 1; //shift right
                        program[regs[destination]].data |= ((condition & CARRY) << 15);     //move carry bit to bit 15
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }
                    case 2: {           //autoincrement:
                        //set memory location pointed to by
                        //register

                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = program[regs[destination]].data & CARRY;    //move bit 0 to C condition code
                        program[regs[destination]].data = program[regs[destination]].data >> 1; //shift right
                        program[regs[deferred]].data |= ((condition & CARRY) << 15);     //move carry bit to bit 15
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;


                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 3:         //autoincrement deferred:
                        //set memory location pointed to by
                        //memory at location pointed to by
                        //register, then increment register
                    {

                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = program[deferred].data & CARRY;    //move bit 0 to C condition code
                        program[deferred].data = program[deferred].data >> 1; //shift right
                        program[regs[deferred]].data |= ((condition & CARRY) << 15);     //move carry bit to bit 15
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[deferred].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 4:         //autodecrement:
                        //decrement register, then
                        //set memory location pointed to by
                        //register
                    {
                        regs[destination] -= 2;
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }

                        condition = program[regs[destination]].data & CARRY;    //move bit 0 to C condition code
                        program[regs[destination]].data = program[regs[destination]].data >> 1; //shift right
                        program[regs[destination]].data |= ((condition & CARRY) << 15);     //move carry bit to bit 15
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;


                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }

                    case 5: {       //autodecrement deferred:
                        //decrement register, then
                        //set memory location pointed to by
                        //memory location pointed to by register
                
                        regs[destination] -= 2;

                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        condition = program[deferred].data & CARRY;    //move bit 0 to C condition code
                        program[deferred].data = program[deferred].data >> 1; //shift right
                        program[deferred].data |= ((condition & CARRY) << 15);     //move carry bit to bit 15
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[deferred].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions

                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        break;
                    }
                    case 6: {       //index:
                        //set memory pointed to by register plus
                        //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = program[index].data & CARRY;    //move bit 0 to C condition code
                        program[index].data = program[index].data >> 1; //shift right
                        program[index].data |= ((condition & CARRY) << 15);     //move carry bit to bit 15
                        program[index + 1].data = program[index].data;
                        if (!(program[index].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[index].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions

                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 1, index);
                        regs[PC] += 2;
                        outcome = 1;
                        break;
                    }
                    case 7: {       //index deferred:
                        //set memory pointed to by memory pointed to
                        //by register plus index, which is located just after
                        //instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (program[index].data % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = program[index].data & CARRY;    //move bit 0 to C condition code
                        program[program[index].data].data = program[program[index].data].data >> 1; //shift right
                        program[program[index].data].data |= (condition & CARRY) << 15;     //move carry bit to bit 15
                        program[program[index].data + 1].data = program[program[index].data].data;
                        if (!(program[program[index].data].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[program[index].data].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions

                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, program[index].data);
                        trace_file(tracefile, 1, program[index].data);
                        
                        regs[PC] += 2;

                        outcome = 1;
                        break;
                    }
                    default: {
                        cout << "invalid destination mode" << endl;
                        break;
                    }
                }
                break;
            }

        case RORB:
            {
                cout << "RORB" << endl;
                break;
            }
        case ROL:
            {
                switch (destination_mode) {
                    case 0: {           //register:
                        // rotate left 1 bit
                        condition = (regs[destination] >> 15) & CARRY;    //move bit 15 to C condition code
                        regs[destination] = regs[destination] << 1; //shift left
                        regs[destination] |= (condition & CARRY); //move carry bit to bit 0
                        
                        if (!(regs[destination]))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((regs[destination] >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions
                        break;
                    }
                    case 1: {           //register deferred:
                        //look at memory location
                        //pointed to by register:
                        // arithmetic shift left 16 bit
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = (program[regs[destination]].data >> 15) & CARRY;    //move bit 15 to C condition code
                        program[regs[destination]].data = program[regs[destination]].data << 1; //shift right
                        program[regs[destination]].data |= (condition & CARRY);   //move carry bit to bit 0
                        program[regs[destination] + 1].data = program[regs[destination]].data;  //store high byte
                        
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //write data write to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }
                    case 2: {           //autoincrement:
                        //increment at memory location pointed to by
                        //register, then increment register

                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = (program[regs[destination]].data >> 15) & CARRY;    //move bit 15 to C condition code
                        program[regs[destination]].data = program[regs[destination]].data >> 1; //shift right
                        program[regs[destination]].data |= (condition & CARRY);   //move carry bit to bit 0
                        program[regs[destination] + 1].data = program[regs[destination]].data;  //store high byte
                        
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;


                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 3:         //autoincrement deferred:
                        //set memory location pointed to by
                        //memory at location pointed to by
                        //register, 
                    {

                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            break;
                        }
                        condition = (program[deferred].data >> 15) & CARRY;    //move bit 15 to C condition code
                        program[deferred].data = program[deferred].data << 1; //shift left
                        program[deferred].data |= (condition & CARRY);   //move carry bit to bit 0
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[deferred].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }
                    case 4:         //autodecrement:
                        //decrement register, then
                        //set memory location pointed to by
                        //register
                    {
                        regs[destination] -= 2;
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }

                        condition = (program[regs[destination]].data >> 15) & CARRY;    //move bit 15 to C condition code
                        program[regs[destination]].data = program[regs[destination]].data << 1; //shift left
                        program[regs[destination]].data |= (condition & CARRY);   //move carry bit to bit 0
                        program[regs[destination] + 1].data = program[regs[destination]].data;
                        
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;


                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }

                    case 5: {       //autodecrement deferred:
                        //decrement register, then
                        //set memory location pointed to by
                        //memory location pointed to by register
                    
                        regs[destination] -= 2;

                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }
                        condition = (program[deferred].data >> 15) & CARRY;    //move bit 0 to C condition code
                        program[deferred].data = program[deferred].data << 1; //shift left
                        program[deferred].data |= (condition & CARRY);   //move carry bit to bit 0
                        program[deferred + 1].data = program[deferred].data;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[deferred].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions

                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        break;
                    }
                    case 6: {       //index:
                        //set memory pointed to by register plus
                        //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = (program[index].data >> 15) & CARRY;    //move bit 0 to C condition code
                        program[index].data = program[index].data << 1; //shift left
                        program[index].data |= (condition & CARRY);   //move carry bit to bit 0
                        program[index + 1].data = program[index].data;
                        if (!(program[index].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[index].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions

                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 1, index);
                        regs[PC] += 2;
                        outcome = 1;
                        break;
                    }
                    case 7: {       //index deferred:
                        //set memory pointed to by memory pointed to
                        //by register plus index, which is located just after
                        //instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (program[index].data % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = (program[index].data >> 15) & CARRY;    //move bit 0 to C condition code
                        program[program[index].data].data = program[program[index].data].data << 1; //shift left
                        program[program[index].data].data |= (condition & CARRY);   //move carry bit to bit 0
                        program[program[index].data + 1].data = program[program[index].data].data;
                        if (!(program[program[index].data].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[program[index].data].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        if((condition & NEGATIVE) ^ (condition & CARRY))    //XOR C and N condition codes for overflow condition
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;

                        states->set_condition(condition);       //set conditions

                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, program[index].data);
                        trace_file(tracefile, 1, program[index].data);
                        
                        regs[PC] += 2;

                        outcome = 1;
                        break;
                    }
                    default: {
                        cout << "invalid destination mode" << endl;
                        break;
                    }
                }

                break;
            }

        case ROLB:
            {
                cout << "ROLB" << endl;
                break;
            }
        case SWAB:
            {
                switch (destination_mode) {
                    case 0: {           //register:
                        // swap bytes
                        condition = (regs[destination] << 8) & 0xFF00;    //use condition as swap variable to store low order byte
                        regs[destination] = ((regs[destination] >> 8) & 0xFF) | condition; //swap bytes 

                        condition = 0;                          //reset condition variable
                        
                        if (!(regs[destination]))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((regs[destination] >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        break;
                    }

                    case 1: {           //register deferred:
                        //look at memory location
                        //pointed to by register:
                        //swap bytes
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = (program[regs[destination]].data << 8) & 0xFF00;    //use condition as swap variable to store low order byte
                        program[regs[destination]].data = ((program[regs[destination]].data >> 8) & 0xFF) | condition; //swap bytes 
                        program[regs[destination + 1]].data = program[regs[destination]].data;

                        condition = 0;
                        
                        if (!(program[regs[destination]].data))           //check if low byte zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }

                    case 2: {           //autoincrement:
                        //increment at memory location pointed to by
                        //register, then
                        //swap bytes
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = (program[regs[destination]].data << 8) & 0xFF00;    //use condition as swap variable to store low order byte
                        program[regs[destination]].data = ((program[regs[destination]].data >> 8) & 0xFF) | condition; //swap bytes 
                        program[regs[destination + 1]].data = program[regs[destination]].data;

                        condition = 0;
                        
                        if (!(program[regs[destination]].data))           //check if low byte zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }

                    case 3: {        //autoincrement deferred:
                        //set memory location pointed to by
                        //memory at location pointed to by
                        //register, then
                        //swap bytes
                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = (program[deferred].data << 8) & 0xFF00;    //use condition as swap variable to store low order byte
                        program[deferred].data = ((program[deferred].data >> 8) & 0xFF) | condition; //swap bytes 
                        program[deferred + 1].data = program[deferred].data;

                        condition = 0;
                        
                        if (!(program[deferred].data))           //check if low byte zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[deferred].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //write data write to trace file
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }

                    case 4: {        //autodecrement:
                        //decrement register, then
                        //swap bytes at memory location
                        //swap bytes
                        regs[destination] -= 2;
                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }

                        condition = (program[regs[destination]].data << 8) & 0xFF00;    //use condition as swap variable to store low order byte
                        program[regs[destination]].data = ((program[regs[destination]].data >> 8) & 0xFF) | condition; //swap bytes 
                        program[regs[destination + 1]].data = program[regs[destination]].data;

                        condition = 0;
                        
                        if (!(program[regs[destination]].data))           //check if low byte zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }


                    case 5: {       //autodecrement deferred:
                        //decrement register, then
                        //set memory location pointed to by
                        //memory location pointed to by register
                        //swap bytes
                        regs[destination] -= 2;

                        deferred = program[regs[destination]].data;
                        if((deferred % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }

                        condition = (program[deferred].data << 8) & 0xFF00;    //use condition as swap variable to store low order byte
                        program[deferred].data = ((program[deferred].data >> 8) & 0xFF) | condition; //swap bytes 
                        program[deferred + 1].data = program[deferred].data;

                        condition = 0;
                        
                        if (!(program[deferred].data))           //check if low byte zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[deferred].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //write data write to trace file
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        outcome = 1;
                        break;
                    }

                    case 6: {       //index:
                        //set memory pointed to by register plus
                        //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = (program[index].data << 8) & 0xFF00;    //use condition as swap variable to store low order byte
                        program[index].data = ((program[index].data >> 8) & 0xFF) | condition; //swap bytes 
                        program[index + 1].data = program[index].data;

                        condition = 0;
                        
                        if (!(program[index].data))           //check if low byte zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[index].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //write data write to trace file
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 1, index);
                        outcome = 1;
                        regs[PC] += 2;
                        break;
                    }


                    case 7: {       //index deferred:
                        //set memory pointed to by memory pointed to
                        //by register plus index, which is located just after
                        //instruction, to 0
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (program[index].data % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = (program[program[index].data].data << 8) & 0xFF00;    //use condition as swap variable to store low order byte
                        program[program[index].data].data = ((program[program[index].data].data >> 8) & 0xFF) | condition; //swap bytes 
                        program[program[index].data + 1].data = program[program[index].data].data;

                        condition = 0;
                        
                        if (!(program[program[index].data].data))           //check if low byte zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[program[index].data].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //write data write to trace file
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 0, program[index].data);
                        trace_file(tracefile, 1, program[index].data);
                        outcome = 1;
                        regs[PC] += 2;
                        break;
                    }

                        
                    default: {
                        cout << "invalid destination mode" << endl;
                        break;
                    }
                }

                break;
            }

        case ADC:
            {
                switch (destination_mode) {
                    case 0: {           //register:
                        // add carry

                        condition = states->get_condition() & CARRY;    //get carry bit
 
                        if((regs[destination] == 0xFFFF) && (condition))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }

                       if((regs[destination] == 0x7FFF) && condition)
                        {
                            regs[destination] += (condition);
                            condition |= V_OVERFLOW;
                        } else {
                            regs[destination] += (condition);
                            condition &= ~V_OVERFLOW;
                        }

                        if (!(regs[destination]))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((regs[destination] >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        break;
                    }

                    case 1: {           //register deferred:
                        //look at memory location
                        //pointed to by register:
                        //add carry

                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = states->get_condition() & CARRY;    //get carry bit
 
                        if((program[regs[destination]].data == 0xFFFF) && (condition))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }

                       if((program[regs[destination]].data == 0x7FFF) && condition)
                        {
                            program[regs[destination]].data += (condition);
                            condition |= V_OVERFLOW;
                        } else {
                            program[regs[destination]].data += (condition);
                            condition &= ~V_OVERFLOW;
                        }
                        program[regs[destination] + 1].data = program[regs[destination]].data;

                        if (!program[regs[destination]].data)           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        break;
                    }

                    case 2: {           //autoincrement:
                        //increment at memory location pointed to by
                        //register, then
                        //add carry

                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = states->get_condition() & CARRY;    //get carry bit
 
                        if((program[regs[destination]].data == 0xFFFF) && (condition))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }

                       if((program[regs[destination]].data == 0x7FFF) && condition)
                        {
                            program[regs[destination]].data += condition;
                            condition |= V_OVERFLOW;
                        } else {
                            program[regs[destination]].data += condition;
                            condition &= ~V_OVERFLOW;
                        }
                        program[regs[destination] + 1].data = program[regs[destination]].data;

                        if (!program[regs[destination]].data)           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }

                    case 3: {        //autoincrement deferred:
                        //set memory location pointed to by
                        //memory at location pointed to by
                        //register, then
                        //add carry
                        deferred = program[regs[destination]].data;

                        if((regs[destination] % 2) || (deferred % 2))
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = states->get_condition() & CARRY;    //get carry bit
 
                        if((program[deferred].data == 0xFFFF) && (condition))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }

                       if((program[deferred].data == 0x7FFF) && condition)
                        {
                            program[deferred].data += (condition);
                            condition |= V_OVERFLOW;
                        } else {
                            program[deferred].data += (condition);
                            condition &= ~V_OVERFLOW;
                        }
                        program[deferred + 1].data = program[deferred].data;

                        if (!program[deferred].data)           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[deferred].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //write data write to trace file
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }


                    case 4: {        //autodecrement:
                        //decrement register, then
                        //add carry
                        regs[destination] -= 2;

                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }

                        condition = states->get_condition() & CARRY;    //get carry bit
 
                        if((program[regs[destination]].data == 0xFFFF) && (condition))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }

                       if((program[regs[destination]].data == 0x7FFF) && condition)
                        {
                            program[regs[destination]].data += (condition);
                            condition |= V_OVERFLOW;
                        } else {
                            program[regs[destination]].data += (condition);
                            condition &= ~V_OVERFLOW;
                        }
                        program[regs[destination] + 1].data = program[regs[destination]].data;

                        if (!program[regs[destination]].data)           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }


                    case 5: {       //autodecrement deferred:
                        //decrement register, then
                        //set memory location pointed to by
                        //memory location pointed to by register
                        //swap bytes
                        regs[destination] -= 2;

                        deferred = program[regs[destination]].data;
 
                        if((regs[destination] % 2) || (deferred % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }

                        condition = states->get_condition() & CARRY;    //get carry bit
 
                        if((program[deferred].data == 0xFFFF) && (condition))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }

                       if((program[deferred].data == 0x7FFF) && condition)
                        {
                            program[deferred].data += (condition);
                            condition |= V_OVERFLOW;
                        } else {
                            program[deferred].data += (condition);
                            condition &= ~V_OVERFLOW;
                        }
                        program[deferred + 1].data = program[deferred].data;

                        if (!program[deferred].data)           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[deferred].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //write data write to trace file
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        outcome = 1;
                        break;
                    }



                    case 6: {       //index:
                        //set memory pointed to by register plus
                        //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }
                        condition = states->get_condition() & CARRY;    //get carry bit

                        if((program[index].data == 0xFFFF) && (condition))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }


                        if((program[index].data == 0x7FFF) && condition)
                        {
                            program[index].data += (condition);
                            condition |= V_OVERFLOW;
                        } else {
                            program[index].data += (condition);
                            condition &= ~V_OVERFLOW;
                        }
                        program[index + 1].data = program[index].data;

                        if (!program[index].data)           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[index].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[PC]);   //write data write to trace file
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 1, index);
                        outcome = 1;
                        regs[PC] += 2;
                        break;
                    }


                    case 7: {       //index deferred:
                        //set memory pointed to by memory pointed to
                        //by register plus index, which is located just after
                        //instruction, to 0
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (program[index].data % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = states->get_condition() & CARRY;    //get carry bit
                        if((program[program[index].data].data == 0xFFFF) && (condition))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }

 
                        if((program[program[index].data].data == 0x7FFF) && condition)
                        {
                            program[program[index].data].data += (condition);
                            condition |= V_OVERFLOW;
                        } else {
                            program[program[index].data].data += (condition);
                            condition &= ~V_OVERFLOW;
                        }

                       program[program[index].data + 1].data = program[program[index].data].data;

                        if (!program[program[index].data].data)           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[program[index].data].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[PC]);   //write data write to trace file
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 0, program[index].data);
                        trace_file(tracefile, 1, program[index].data);
                        outcome = 1;
                        regs[PC] += 2;
                        break;
                    }


                    default: {
                        cout << "invalid destination mode" << endl;
                        break;
                    }
                }

                break;
            }

        case ADCB:
            {
                cout << "ADCB" << endl;
                break;
            }
        case SBC:
            {
    
                switch (destination_mode) {
                    case 0: {           //register:
                        // subtract carry

                        condition = states->get_condition() & CARRY;    //get carry bit

                        if(!(regs[destination]) && (condition))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }

                        if((regs[destination] == 0x8000))
                        {
                            regs[destination] -= condition;
                            condition |= V_OVERFLOW;
                        } else {
                            regs[destination] -= condition;
                            condition &= ~V_OVERFLOW;
                        }

                        if (!(regs[destination]))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((regs[destination] >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        break;
                    }

                    case 1: {           //register deferred:
                        //look at memory location
                        //pointed to by register:
                        //add carry

                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = states->get_condition() & CARRY;    //get carry bit
                        if(!(program[regs[destination]].data) && (condition & CARRY))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }

                        if((program[regs[destination]].data == 0x8000))
                        {
                            program[regs[destination]].data -= condition;
                            condition |= V_OVERFLOW;
                        } else {
                            program[regs[destination]].data -= condition;
                            condition &= ~V_OVERFLOW;
                        }

                        program[regs[destination] + 1].data = program[regs[destination]].data;


                        if (!program[regs[destination]].data)           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        break;
                    }

                    case 2: {           //autoincrement:
                        //increment at memory location pointed to by
                        //register, then
                        //add carry

                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = states->get_condition() & CARRY;    //get carry bit
                        if(!(program[regs[destination]].data) && (condition & CARRY))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }

                        if((program[regs[destination]].data == 0x8000))
                        {
                            program[regs[destination]].data -= condition;
                            condition |= V_OVERFLOW;
                        } else {
                            program[regs[destination]].data -= condition;
                            condition &= ~V_OVERFLOW;
                        }

                        program[regs[destination] + 1].data = program[regs[destination]].data;

                        if (!program[regs[destination]].data)           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }

                    case 3: {        //autoincrement deferred:
                        //set memory location pointed to by
                        //memory at location pointed to by
                        //register, then
                        //add carry
                        deferred = program[regs[destination]].data;

                        if((regs[destination] % 2) || (deferred % 2))
                        {
                            cout << "unaligned reference\n";
                            break;
                        }

                        condition = states->get_condition() & CARRY;    //get carry bit
                        
                        if(!(program[deferred].data) && (condition & CARRY))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }

                        if((program[deferred].data == 0x7FFF) && condition)
                        {
                            program[deferred].data -= condition;
                            condition |= V_OVERFLOW;
                        } else {
                            program[deferred].data -= condition;
                            condition &= ~V_OVERFLOW;
                        }

                        program[deferred + 1].data = program[deferred].data;

                        if (!program[deferred].data)           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[deferred].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //write data write to trace file
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        break;
                    }


                    case 4: {        //autodecrement:
                        //decrement register, then
                        //add carry
                        regs[destination] -= 2;

                        if(regs[destination] % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }

                        condition = states->get_condition() & CARRY;    //get carry bit
                        if(!(program[regs[destination]].data) && (condition & CARRY))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }

                        if((program[regs[destination]].data == 0x8000))
                        {
                            program[regs[destination]].data -= condition;
                            condition |= V_OVERFLOW;
                        } else {
                            program[regs[destination]].data -= condition;
                            condition &= ~V_OVERFLOW;
                        }

                        program[regs[destination] + 1].data = program[regs[destination]].data;

                        if (!program[regs[destination]].data)           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[regs[destination]].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //data read to trace file
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }


                    case 5: {       //autodecrement deferred:
                        //decrement register, then
                        //set memory location pointed to by
                        //memory location pointed to by register
                        //swap bytes
                        regs[destination] -= 2;

                        deferred = program[regs[destination]].data;
 
                        if((regs[destination] % 2) || (deferred % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[destination] += 2;
                            break;
                        }

                        condition = states->get_condition() & CARRY;    //get carry bit

                        if(!(program[deferred].data) && (condition & CARRY))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }


                        if((program[deferred].data == 0x8000))
                        {
                            program[deferred].data -= condition;
                            condition |= V_OVERFLOW;
                        } else {
                            program[deferred].data -= condition;
                            condition &= ~V_OVERFLOW;
                        }

                        program[deferred + 1].data = program[deferred].data;

                        if (!program[deferred].data)           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[deferred].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);   //write data write to trace file
                        trace_file(tracefile, 0, (uint16_t)deferred);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        outcome = 1;
                        break;
                    }



                    case 6: {       //index:
                        //set memory pointed to by register plus
                        //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = states->get_condition() & CARRY;    //get carry bit

                        if(!(program[index].data) && (condition & CARRY))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }

                        if(!(program[index].data))
                        {
                            program[index].data -= condition;
                            condition |= V_OVERFLOW;
                        } else {
                            program[index].data -= condition;
                            condition &= ~V_OVERFLOW;
                        }

                        program[index + 1].data = program[index].data;

                        if (!program[index].data)           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[index].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[PC]);   //write data write to trace file
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 1, index);
                        outcome = 1;
                        regs[PC] += 2;
                        break;
                    }


                    case 7: {       //index deferred:
                        //set memory pointed to by memory pointed to
                        //by register plus index, which is located just after
                        //instruction, to 0
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if((index % 2) || (program[index].data % 2) || (regs[destination] % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = states->get_condition() & CARRY;    //get carry bit

                        if(!(program[program[index].data].data) && (condition & CARRY))
                        {
                            condition |= CARRY;
                        } else {
                            condition &= ~CARRY;
                        }

                        if((program[program[index].data].data == 0x8000))
                        {
                            program[program[index].data].data -= condition;
                            condition |= V_OVERFLOW;
                        } else {
                            program[program[index].data].data -= condition;
                            condition &= ~V_OVERFLOW;
                        }

                        program[program[index].data + 1].data = program[program[index].data].data;

                        if (!program[program[index].data].data)           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((program[program[index].data].data >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[PC]);   //write data write to trace file
                        trace_file(tracefile, 0, index);
                        trace_file(tracefile, 0, program[index].data);
                        trace_file(tracefile, 1, program[index].data);
                        outcome = 1;
                        regs[PC] += 2;
                        break;
                    }


                    default: {
                        cout << "invalid destination mode" << endl;
                        break;
                    }
                }

                break;
            }

        case SBCB:
            {
                cout << "SBCB" << endl;
                break;
            }
        case SXT:
            {

                switch(destination_mode) {
                    case 0: {       //register: set to N flag

                                condition = states->get_condition();

                                if(!(condition & NEGATIVE))
                                {
                                    regs[destination] = 0;
                                    condition |= ZERO;
                                    condition &= ~NEGATIVE;
                                } else {
                                    regs[destination] = 0xFFFF;
                                    condition &= ~ZERO;
                                    condition |= NEGATIVE;
                                }

                                states->set_condition(condition);
                                outcome = regs[destination];
                                break;
                            }

                    case 1: {   //set memory location to N flag

                                if(regs[destination] % 2)
                                {
                                    cout << "unaligned reference\n";
                                    break;
                                }

                                condition = states->get_condition();

                                if(!(condition & NEGATIVE))
                                {
                                    program[regs[destination]].data = 0;
                                    condition |= ZERO;
                                    condition &= ~NEGATIVE;
                                } else {
                                    program[regs[destination]].data = 0xFFFF;
                                    condition &= ~ZERO;
                                    condition |= NEGATIVE;
                                }
                                
                                program[regs[destination] + 1].data = program[regs[destination]].data;
                                states->set_condition(condition);

                                trace_file(tracefile, 1, regs[destination]);
                                outcome = program[regs[destination]].data;
                                break;
                            }

                    case 2: {   //set memory location to N flag
                                //increment register
                                if(regs[destination] % 2)
                                {
                                    cout << "unaligned reference\n";
                                    break;
                                }

                                condition = states->get_condition();

                                if(!(condition & NEGATIVE))
                                {
                                    program[regs[destination]].data = 0;
                                    condition |= ZERO;
                                    condition &= ~NEGATIVE;
                                } else {
                                    program[regs[destination]].data = 0xFFFF;
                                    condition &= ~ZERO;
                                    condition |= NEGATIVE;
                                }

                                program[regs[destination] + 1].data = program[regs[destination]].data;
                                
                                states->set_condition(condition);
                                trace_file(tracefile, 1, regs[destination]);

                                regs[destination] += 2;
                                outcome = program[regs[destination]].data;
                                break;
                            }

                    case 3: {   //set memory location to N flag
                                //increment register
                                //
                                deferred = program[regs[destination]].data;

                                if((regs[destination] % 2) || (deferred % 2))
                                {
                                    cout << "unaligned reference\n";
                                    break;
                                }

                                condition = states->get_condition();

                                if(!(condition & NEGATIVE))
                                {
                                    program[deferred].data = 0;
                                    condition |= ZERO;
                                    condition &= ~NEGATIVE;
                                } else {
                                    program[deferred].data = 0xFFFF;
                                    condition &= ~ZERO;
                                    condition |= NEGATIVE;
                                }

                                program[deferred + 1].data = program[deferred].data;
                                
                                states->set_condition(condition);
                                trace_file(tracefile, 0, regs[destination]);
                                trace_file(tracefile, 1, deferred);

                                regs[destination] += 2;
                                outcome = program[deferred].data;
                                break;
                            }

                    case 4: {   //decrement register
                                //set memory location to N flag
                                regs[destination] -= 2;

                                if(regs[destination] % 2)
                                {
                                    cout << "unaligned reference\n";
                                    regs[destination] += 2;
                                    break;
                                }

                                condition = states->get_condition();

                                if(!(condition & NEGATIVE))
                                {
                                    program[regs[destination]].data = 0;
                                    condition |= ZERO;
                                    condition &= ~NEGATIVE;
                                } else {
                                    program[regs[destination]].data = 0xFFFF;
                                    condition &= ~ZERO;
                                    condition |= NEGATIVE;
                                }

                                program[regs[destination] + 1].data = program[regs[destination]].data;
                                
                                states->set_condition(condition);
                                trace_file(tracefile, 1, regs[destination]);
                                outcome = program[regs[destination]].data;
                                break;
                            }

                    case 5: {   //decrement register
                                //set memory location to N flag
                                //
                                regs[destination] -= 2;
                                deferred = program[regs[destination]].data;

                                if((regs[destination] % 2) || (deferred % 2))
                                {
                                    cout << "unaligned reference\n";
                                    regs[destination] += 2;
                                    break;
                                }

                                condition = states->get_condition();

                                if(!(condition & NEGATIVE))
                                {
                                    program[deferred].data = 0;
                                    condition |= ZERO;
                                    condition &= ~NEGATIVE;
                                } else {
                                    program[deferred].data = 0xFFFF;
                                    condition &= ~ZERO;
                                    condition |= NEGATIVE;
                                }

                                program[deferred + 1].data = program[deferred].data;
                                
                                states->set_condition(condition);
                                trace_file(tracefile, 0, regs[destination]);
                                trace_file(tracefile, 1, deferred);

                                outcome = program[deferred].data;
                                break;
                            }

                    case 6: {   //use index after instruction
                                //set memory location to N flag
                                //
                                if(destination == PC)
                                    index = regs[PC] + 2 + program[regs[PC]].data;
                                else index = regs[destination] + program[regs[PC]].data;

                                if((regs[destination] % 2) || (index % 2))
                                {
                                    cout << "unaligned reference\n";
                                    regs[PC] += 2;
                                    break;
                                }

                                condition = states->get_condition();

                                if(!(condition & NEGATIVE))
                                {
                                    program[index].data = 0;
                                    condition |= ZERO;
                                    condition &= ~NEGATIVE;
                                } else {
                                    program[index].data = 0xFFFF;
                                    condition &= ~ZERO;
                                    condition |= NEGATIVE;
                                }

                                program[index + 1].data = program[index].data;
                                
                                states->set_condition(condition);
                                trace_file(tracefile, 0, regs[PC]);
                                trace_file(tracefile, 1, index);

                                regs[PC] += 2;
                                outcome = program[index].data;
                                break;
                            }

                    case 7: {   //use index after instruction
                                //set memory location to N flag
                                //
                                if(destination == PC)
                                    index = regs[PC] + 2 + program[regs[PC]].data;
                                else index = regs[destination] + program[regs[PC]].data;

                                if((regs[destination] % 2) || (index % 2) || (program[index].data % 2))
                                {
                                    cout << "unaligned reference\n";
                                    regs[PC] += 2;
                                    break;
                                }

                                condition = states->get_condition();

                                if(!(condition & NEGATIVE))
                                {
                                    program[program[index].data].data = 0;
                                    condition |= ZERO;
                                    condition &= ~NEGATIVE;
                                } else {
                                    program[program[index].data].data = 0xFFFF;
                                    condition &= ~ZERO;
                                    condition |= NEGATIVE;
                                }

                                program[program[index].data + 1].data = program[program[index].data].data;
                                
                                states->set_condition(condition);
                                trace_file(tracefile, 0, regs[PC]);
                                trace_file(tracefile, 0, index);
                                trace_file(tracefile, 1, program[index].data);

                                regs[PC] += 2;
                                outcome = program[program[index].data].data;
                                break;
                            }

                    default: {
                        cout << "nope" << endl;
                        break;
                    }
                }

                break;
            }
        default:
            {
                cout << "nope" << endl;
                break;
            }
    }

    return outcome;
}

//display information when -v option set
int single_operand::fetch_display(uint16_t *regs, CPSR *states)
{
    int condition;
    int i;

   switch(function_code)
    {
        case CLR:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "CLR" << endl;
                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case COM:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "COM" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case INC:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "INC" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case DEC:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "DEC" << endl;
                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case NEG:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "NEG" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case TST:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "TST" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }

        case ASR:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "ASR" << endl;
                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case ASL:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "ASL" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case ROR:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "ROR" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }

        case ROL:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "ROL" << endl;
                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case SWAB:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "SWAB" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case ADC:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "ADC" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }

        case SBC:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "SBC" << endl;
                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case SXT:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "SXT" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }

        default:
            {
                cout << "nope" << endl;
                break;
            }
    }
    return 0;
}


single_operand::~single_operand()
{
}


