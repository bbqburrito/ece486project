<<<<<<< HEAD
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
        start = strtol(answer, nullptr, 10);
        if(start > prog_length)
        {
            cout << "program only " << prog_length << " lines. Start set to 0\n";
            start = 0;
        }
    }
    else {
        cout << "find '*' to start at or start at 0." << endl;
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

//writes to file specified by filename. writes address parameter
//as an octal. returns an int to indicate outcome.
//takes a char to indicate filename, an int to indicate
//the type write, and an int to indicate the address write
int trace_file(char *filename, int type, int address) {
    char make_address[100];
    ofstream outfile;
    int i;
    int outcome;

    //initialize make_address to all NULL
    for(i = 0; i < 100; ++i)
    {
        make_address[i] = '\0';
    }

    outfile.open(filename, ios::app | ios::out);

    if(!outfile.is_open())
    {
        cout << "file error\n";
        return -1;
    }

    outcome = sprintf(make_address, "%d\t\t%o\n", type, address);

    outfile << make_address;

    return outcome;
}

//takes int as a command to parse, an int pointer to pass firstbit, and a command 
//pointer to assign to command class object by upcasting. returns an int representing
//the function code of passed command. parses command, then creates an object of 
//class type and stores in new_command pointer
int interpreter(int to_interpret, int * firstbit, command *& new_command, char * tracefile)
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
}


//command
command::command(): data(0), disposition(0), tracefile(nullptr)
{
}

command::command(int to_data, char to_disposition, char * to_tracefile): data(to_data), disposition(to_disposition),
                                                                      tracefile(to_tracefile)
{
}

void command::disp()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
}


int command::instruction(int *regs, CPSR *states, i_cache *program)
{
    return 0;
}

int command::instructionB(int *regs, CPSR *states, i_cache * program)
{
    return 0;
}

command::~command() {

}

//extended
extended::extended(): command(), function_code(0), destination(0), source_mode(0), source(0)
{
}

extended::extended(int to_data, char disposition, char * to_tracefile, int to_function_code,
                   int to_destination, int to_source_mode,
                   int to_source): command(to_data, disposition, to_tracefile), function_code(to_function_code),
                                   destination(to_destination), source_mode(to_source_mode), source(to_source)
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

int extended::instruction(int *regs, CPSR *states, i_cache * program)
{
    cout << "function_code: " << function_code << ": ";
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

//trapIntMiscCond
trapIntMiscCond::trapIntMiscCond(): command(), function_code(0), trap_code(0)
{
}


trapIntMiscCond::trapIntMiscCond(int to_data, char to_disposition, char * to_tracefile, int to_function_code,
                                 int to_trap_code): command(to_data, to_disposition, to_tracefile),
                                                    function_code(to_function_code), trap_code(to_trap_code)
{
}

void trapIntMiscCond::disp()
{
    cout << "data: " << data << endl;
    cout << "dispostion: " << disposition << endl;
    cout << "function code: " << function_code << endl;
    cout << "trap code: " << trap_code << endl;
}

int trapIntMiscCond::instruction(int *regs, CPSR *states, i_cache *program)
{
    int outcome = -1;

    cout << "function_code: " << function_code << ": ";
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
                cout << "HALT" << endl;
                break;
            }
        case WAIT:
            {
                cout << "WAIT" << endl;
                break;
            }
        case RESET:
            {
                cout << "RESET" << endl;
                break;
            }
        case CLC:
            {
                cout << "CLC" << endl;
                states->set_condition(states->get_condition() & 0xFE);
                break;
            }
        case CLV:
            {
                cout << "CLV" << endl;
                states->set_condition(states->get_condition() & 0xFD);
                break;
            }
        case CLZ:
            {
                cout << "CLZ" << endl;
                states->set_condition(states->get_condition() & 0xFB);
                break;
            }
        case CLN:
            {
                cout << "CLN" << endl;
                states->set_condition(states->get_condition() & 0xF7);
                break;
            }
        case CCC:
            {
                cout << "CCC" << endl;
                states->set_condition(states->get_condition() & 0xF0);
                break;
            }
        case SEC:
            {
                cout << "SEC" << endl;
                states->set_condition(states->get_condition() | 1);
                break;
            }
        case SEV:
            {
                cout << "SEV" << endl;
                states->set_condition(states->get_condition() | 2);
                break;
            }
        case SEZ:
            {
                cout << "SEZ" << endl;
                states->set_condition(states->get_condition() | 4);
                break;
            }
        case SEN:
            {
                cout << "SEN" << endl;
                states->set_condition(states->get_condition() | 8);
                break;
            }
        case SCC:
            {
                cout << "SCC" << endl;
                states->set_condition(states->get_condition() | 0xF);
                break;
            }
        case NOP:
            {
                cout << "NOP" << endl;
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

//jump_sub
jump_sub::jump_sub(): command(), function_code(0), linkage_reg(0), destination_code(0), destination(0), parameters(0)
{
}

jump_sub::jump_sub(int to_data, char to_disposition, char * to_tracefile, int to_function_code, int to_linkage_reg,
                   int to_destination_code, int to_destination,
                   int to_parameters): command(to_data, to_disposition, to_tracefile), function_code(to_function_code),
                                       linkage_reg(to_linkage_reg), destination_code(to_destination_code),
                                       destination(to_destination), parameters(to_parameters)
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

int jump_sub::instruction(int *regs, CPSR *states, i_cache *program)
{

    cout << "function_code: " << function_code << ": ";
   switch(function_code)
    {
        case JMP:
            {
                
                cout << "JMP" << endl;
                break;
            }
        case JSR:
            {
                cout << "JSR" << endl;
                break;
            }
        case RTS:
            {
                cout << "RTS" << endl;
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

int branch::instruction(int *regs, CPSR *states, i_cache *program)
{

    cout << "function_code: " << function_code << ": ";
   switch(function_code)
    {
        case BR:
            {
                cout << "BR" << endl;

                break;
            }
        case BNE:
            {
                cout << "BNE" << endl;
                break;
            }
        case BEQ:
            {
                cout << "BEQ" << endl;
                break;
            }
        case BPL:
            {
                cout << "BPL" << endl;
                break;
            }
        case BMI:
            {
                cout << "BMI" << endl;
                break;
            }
        case BVC:
            {
                cout << "BVC" << endl;
                break;
            }
        case BVS:
            {
                cout << "BVS" << endl;
                break;
            }
        case BCC:
            {
                cout << "BCC" << endl;
                break;
            }
        case BCS:
            {
                cout << "BCS" << endl;
                break;
            }
        case BGE:
            {
                cout << "BGE" << endl;
                break;
            }
        case BLT:
            {
                cout << "BLT" << endl;
                break;
            }
        case BGT:
            {
                cout << "BGT" << endl;
                break;
            }
        case BLE:
            {
                cout << "BLE" << endl;
                break;
            }
        case BHI:
            {
                cout << "BHI" << endl;
                break;
            }
        case BLOS:
            {
                cout << "BLOS" << endl;
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

//single operand
single_operand::single_operand(): command(), function_code(0), destination_mode(0), destination(0)
{
}

single_operand::single_operand(int to_data, char to_disposition, char * to_tracefile, int to_function_code,
                               int to_destination_mode,
                               int to_destination): command(to_data, to_disposition, to_tracefile), function_code(to_function_code),
                                                    destination_mode(to_destination_mode), destination(to_destination)
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

int single_operand::instruction(int *regs, CPSR *states, i_cache *program)
{
    int outcome = -1;

    cout << "function_code: " << function_code << ": ";

    switch(function_code)
    {
        case CLR:
            {
                cout << "CLR" << endl;
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
                        program[regs[destination] / 2].data = 0;
                        states->set_condition(4);
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        break;
                    }
                    case 2: {           //autoincrement:
                                        //set memory location pointed to by
                                        //register to 0, then increment register
                        program[regs[destination] / 2].data = 0;
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
                        program[program[regs[destination]/2].data/2].data = 0;
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 1, program[regs[destination]/2].data);
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
                        program[regs[destination]/2].data = 0;
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
                        program[program[regs[destination]/2].data/2].data = 0;
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 1, program[regs[destination]/2].data);
                        states->set_condition(4);
                        outcome = 1;
                        break;
                    }
                    case 6: {       //index:
                                    //set memory pointed to by register plus
                                    //index, which is located just after instruction,
                                    //to 0
                        program[(regs[destination] + program[regs[7]/2 + 1].data)/2].data = 0;
                        trace_file(tracefile, 1, regs[destination] + program[regs[7]/2 + 1].data);
                        trace_file(tracefile, 0, program[regs[7]/2].data + 1);
                        regs[7] += 4;
                        states->set_condition(4);
                        outcome = 1;
                        break;
                    }
                    case 7: {       //index deferred:
                                    //set memory pointed to by memory pointed to
                                    //by register plus index, which is located just after
                                    //instruction, to 0
                        program[program[(regs[destination] + program[regs[7]/2 + 1].data)/2].data/2].data = 0;
                        trace_file(tracefile, 1, program[(regs[destination] + program[regs[7]/2 + 1].data)/2].data);
                        trace_file(tracefile, 0, regs[destination] + program[regs[7]/2 + 1].data);
                        trace_file(tracefile, 0, program[regs[7]/2].data + 1);
                        regs[7] += 4;
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
                cout << "COM" << endl;
                break;
            }
        case COMB:
            {
                cout << "COMB" << endl;
                break;
            }
        case INC:
            {
                cout << "INC" << endl;
                break;
            }
        case INCB:
            {
                cout << "INCB" << endl;
                break;
            }
        case DEC:
            {
                cout << "DEC" << endl;
                break;
            }
        case DECB:
            {
                cout << "DECB" << endl;
                break;
            }
        case NEG:
            {
                cout << "NEG" << endl;
                break;
            }
        case NEGB:
            {
                cout << "NEGB" << endl;
                break;
            }
        case TST:
            {
                cout << "TST" << endl;
                break;
            }
        case TSTB:
            {
                cout << "TSTB" << endl;
                break;
            }
        case ASR:
            {
                cout << "ASR" << endl;
                break;
            }
        case ASRB:
            {
                cout << "ASRB" << endl;
                break;
            }
        case ASL:
            {
                cout << "ASL" << endl;
                break;
            }
        case ASLB:
            {
                cout << "ASLB" << endl;
                break;
            }
        case ROR:
            {
                cout << "ROR" << endl;
                break;
            }
        case RORB:
            {
                cout << "RORB" << endl;
                break;
            }
        case ROL:
            {
                cout << "ROL" << endl;
                break;
            }
        case ROLB:
            {
                cout << "ROLB" << endl;
                break;
            }
        case SWAB:
            {
                cout << "SWAB" << endl;
                break;
            }
        case ADC:
            {
                cout << "ADC" << endl;
                break;
            }
        case ADCB:
            {
                cout << "ADCB" << endl;
                break;
            }
        case SBC:
            {
                cout << "SBC" << endl;
                break;
            }
        case SBCB:
            {
                cout << "SBCB" << endl;
                break;
            }
        case SXT:
            {
                cout << "SXT" << endl;
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


//double_operand
double_operand::double_operand(): command(), function_code(0), source(0), source_mode(0), destination(0),
                                  destination_mode(0)
{
}

double_operand::double_operand(int to_data, char to_disposition, char * to_tracefile, int to_function_code,
                               int to_source_mode, int to_source, int to_destination_mode,
                               int to_destination): command(to_data, to_disposition, to_tracefile),
                                                    function_code(to_function_code), source(to_source),
                                                    source_mode(to_source_mode), destination(to_destination),
                                                    destination_mode(to_destination_mode)
{
}

void double_operand::disp()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
    cout << "function_code: " << function_code << endl;
    cout << "source_mode: " << source_mode << endl;
    cout << "source: " << source << endl;
    cout << "destination_mode: " << destination_mode << endl;
    cout << "destination: " << destination << endl;
}

int double_operand::instruction(int *regs, CPSR *states, i_cache *program)
{
    cout << "function_code: " << function_code << ": ";

    switch(function_code)
    {
        case MOV:
            {
                cout << "MOV" << endl;
                break;
            }
        case MOVB:
            {
                cout << "MOVB" << endl;
                break;
            }
        case CMP: 
            {
                cout << "CMP" << endl;
                break;
            }
        case CMPB:
            {
                cout << "CMPB" << endl;
                break;
            }
        case ADD:
            {
                cout << "ADD" << endl;
                break;
            }
        case SUB:
            {
                cout << "SUB" << endl;
                break;
            }
        case BIT:
            {
                cout << "BIT" << endl;
                break;
            }
        case BITB:
            {
                cout << "BITB" << endl;
                break;
            }
        case BIC:
            {
                cout << "BIC" << endl;
                break;
            }
        case BICB:
            {
                cout << "BICB" << endl;
                break;
            }
        case BIS:
            {
                cout << "BIS" << endl;
                break;
            }
        case BISB:
            {
                cout << "BISB" << endl;
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

=======
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
        start = strtol(answer, nullptr, 10);
        if(start > prog_length)
        {
            cout << "program only " << prog_length << " lines. Start set to 0\n";
            start = 0;
        }
    }
    else {
        cout << "find '*' to start at or start at 0." << endl;
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

//writes to file specified by filename. writes address parameter
//as an octal. returns an int to indicate outcome.
//takes a char to indicate filename, an int to indicate
//the type write, and an int to indicate the address write
int trace_file(char *filename, int type, int address) {
    char make_address[100];
    ofstream outfile;
    int i;
    int outcome;

    //initialize make_address to all NULL
    for(i = 0; i < 100; ++i)
    {
        make_address[i] = '\0';
    }

    outfile.open(filename, ios::app | ios::out);

    if(!outfile.is_open())
    {
        cout << "file error\n";
        return -1;
    }

    outcome = sprintf(make_address, "%d\t\t%o\n", type, address);

    outfile << make_address;

    return outcome;
}

//takes int as a command to parse, an int pointer to pass firstbit, and a command 
//pointer to assign to command class object by upcasting. returns an int representing
//the function code of passed command. parses command, then creates an object of 
//class type and stores in new_command pointer
int interpreter(int to_interpret, int * firstbit, command *& new_command, char * tracefile)
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

//gp_register
gp_register::gp_register(): contents(0)
{
}

gp_register::~gp_register()
= default;

gp_register::gp_register(const gp_register &to_copy)
{
    contents = to_copy.get_gp();
}


void gp_register::display()
{
    cout << "contents: " << contents << endl;
}

int gp_register::set_gp(int to_set)
{
    contents = to_set;

    return contents;
}

int gp_register::get_gp() const
{
    return contents;
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
}


//command
command::command(): data(0), disposition(0), tracefile(nullptr)
{
}

command::command(int to_data, char to_disposition, char * to_tracefile): data(to_data), disposition(to_disposition),
                                                                      tracefile(to_tracefile)
{
}

void command::disp()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
}


int command::instruction(int *regs, CPSR *states, i_cache *program, int position, int * mem)
{
    return 0;
}

int command::instructionB(int *regs, CPSR *states, i_cache * program, int position, int * mem)
{
    return 0;
}

//extended
extended::extended(): command(), function_code(0), destination(0), source_mode(0), source(0)
{
}

extended::extended(int to_data, char disposition, char * to_tracefile, int to_function_code, int to_destination,
                   int to_source_mode, int to_source): command(to_data, disposition, to_tracefile), function_code(to_function_code),
                                                       destination(to_destination), source_mode(to_source_mode), source(to_source)
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

int extended::instruction(int *regs, CPSR *states, i_cache * program, int position, int * mem)
{
    cout << "function_code: " << function_code << ": ";
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

//trapIntMiscCond
trapIntMiscCond::trapIntMiscCond(): command(), function_code(0), trap_code(0)
{
}


trapIntMiscCond::trapIntMiscCond(int to_data, char to_disposition, char * to_tracefile, int to_function_code,
                                 int to_trap_code): command(to_data, to_disposition, to_tracefile),
                                                    function_code(to_function_code), trap_code(to_trap_code)
{
}

void trapIntMiscCond::disp()
{
    cout << "data: " << data << endl;
    cout << "dispostion: " << disposition << endl;
    cout << "function code: " << function_code << endl;
    cout << "trap code: " << trap_code << endl;
}

int trapIntMiscCond::instruction(int *regs, CPSR *states, i_cache *program, int position, int * mem)
{
    int outcome = -1;

    cout << "function_code: " << function_code << ": ";
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
                cout << "HALT" << endl;
                break;
            }
        case WAIT:
            {
                cout << "WAIT" << endl;
                break;
            }
        case RESET:
            {
                cout << "RESET" << endl;
                break;
            }
        case CLC:
            {
                cout << "CLC" << endl;
                states->set_condition(states->get_condition() & 0xFE);
                break;
            }
        case CLV:
            {
                cout << "CLV" << endl;
                states->set_condition(states->get_condition() & 0xFD);
                break;
            }
        case CLZ:
            {
                cout << "CLZ" << endl;
                states->set_condition(states->get_condition() & 0xFB);
                break;
            }
        case CLN:
            {
                cout << "CLN" << endl;
                states->set_condition(states->get_condition() & 0xF7);
                break;
            }
        case CCC:
            {
                cout << "CCC" << endl;
                states->set_condition(states->get_condition() & 0xF0);
                break;
            }
        case SEC:
            {
                cout << "SEC" << endl;
                states->set_condition(states->get_condition() | 1);
                break;
            }
        case SEV:
            {
                cout << "SEV" << endl;
                states->set_condition(states->get_condition() | 2);
                break;
            }
        case SEZ:
            {
                cout << "SEZ" << endl;
                states->set_condition(states->get_condition() | 4);
                break;
            }
        case SEN:
            {
                cout << "SEN" << endl;
                states->set_condition(states->get_condition() | 8);
                break;
            }
        case SCC:
            {
                cout << "SCC" << endl;
                states->set_condition(states->get_condition() | 0xF);
                break;
            }
        case NOP:
            {
                cout << "NOP" << endl;
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

//jump_sub
jump_sub::jump_sub(): command(), function_code(0), linkage_reg(0), destination_code(0), destination(0), parameters(0)
{
}

jump_sub::jump_sub(int to_data, char to_disposition, char * to_tracefile, int to_function_code, int to_linkage_reg,
                   int to_destination_code, int to_destination,
                   int to_parameters): command(to_data, to_disposition, to_tracefile), function_code(to_function_code),
                                       linkage_reg(to_linkage_reg), destination_code(to_destination_code),
                                       destination(to_destination), parameters(to_parameters)
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

int jump_sub::instruction(int *regs, CPSR *states, i_cache *program, int position, int * mem)
{

    cout << "function_code: " << function_code << ": ";
   switch(function_code)
    {
        case JMP:
            {
                cout << "JMP" << endl;
                break;
            }
        case JSR:
            {
                cout << "JSR" << endl;
                break;
            }
        case RTS:
            {
                cout << "RTS" << endl;
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

int branch::instruction(int *regs, CPSR *states, i_cache *program, int position, int * mem)
{

    cout << "function_code: " << function_code << ": ";
   switch(function_code)
    {
        case BR:
            {
                cout << "BR" << endl;

                break;
            }
        case BNE:
            {
                cout << "BNE" << endl;
                break;
            }
        case BEQ:
            {
                cout << "BEQ" << endl;
                break;
            }
        case BPL:
            {
                cout << "BPL" << endl;
                break;
            }
        case BMI:
            {
                cout << "BMI" << endl;
                break;
            }
        case BVC:
            {
                cout << "BVC" << endl;
                break;
            }
        case BVS:
            {
                cout << "BVS" << endl;
                break;
            }
        case BCC:
            {
                cout << "BCC" << endl;
                break;
            }
        case BCS:
            {
                cout << "BCS" << endl;
                break;
            }
        case BGE:
            {
                cout << "BGE" << endl;
                break;
            }
        case BLT:
            {
                cout << "BLT" << endl;
                break;
            }
        case BGT:
            {
                cout << "BGT" << endl;
                break;
            }
        case BLE:
            {
                cout << "BLE" << endl;
                break;
            }
        case BHI:
            {
                cout << "BHI" << endl;
                break;
            }
        case BLOS:
            {
                cout << "BLOS" << endl;
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

//single operand
single_operand::single_operand(): command(), function_code(0), destination_mode(0), destination(0)
{
}

single_operand::single_operand(int to_data, char to_disposition, char * to_tracefile, int to_function_code,
                               int to_destination_mode,
                               int to_destination): command(to_data, to_disposition, to_tracefile), function_code(to_function_code),
                                                    destination_mode(to_destination_mode), destination(to_destination)
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

int single_operand::instruction(int *regs, CPSR *states, i_cache *program, int position, int * mem)
{
    int outcome = -1;

    cout << "function_code: " << function_code << ": ";

    switch(function_code)
    {
        case CLR:
            {
                cout << "CLR" << endl;
                switch (destination_mode) {
                    case 0: 
                        {
                            regs[destination] = 0;
                            states->set_condition(4);
                            break;
                        }
                    case 1:
                        {
                            mem[regs[destination]] = 0;
                            states->set_condition(4);
                            trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                            outcome = 1;
                            break;
                        }
                    case 2:
                        {
                            mem[regs[destination]] = 0;
                            regs[destination] += 1;
                            states->set_condition(4);
                            trace_file(tracefile, 1, regs[destination] - 1);   //write data write to trace file
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
                cout << "COM" << endl;
                break;
            }
        case COMB:
            {
                cout << "COMB" << endl;
                break;
            }
        case INC:
            {
                cout << "INC" << endl;
                break;
            }
        case INCB:
            {
                cout << "INCB" << endl;
                break;
            }
        case DEC:
            {
                cout << "DEC" << endl;
                break;
            }
        case DECB:
            {
                cout << "DECB" << endl;
                break;
            }
        case NEG:
            {
                cout << "NEG" << endl;
                break;
            }
        case NEGB:
            {
                cout << "NEGB" << endl;
                break;
            }
        case TST:
            {
                cout << "TST" << endl;
                break;
            }
        case TSTB:
            {
                cout << "TSTB" << endl;
                break;
            }
        case ASR:
            {
                cout << "ASR" << endl;
                break;
            }
        case ASRB:
            {
                cout << "ASRB" << endl;
                break;
            }
        case ASL:
            {
                cout << "ASL" << endl;
                break;
            }
        case ASLB:
            {
                cout << "ASLB" << endl;
                break;
            }
        case ROR:
            {
                cout << "ROR" << endl;
                break;
            }
        case RORB:
            {
                cout << "RORB" << endl;
                break;
            }
        case ROL:
            {
                cout << "ROL" << endl;
                break;
            }
        case ROLB:
            {
                cout << "ROLB" << endl;
                break;
            }
        case SWAB:
            {
                cout << "SWAB" << endl;
                break;
            }
        case ADC:
            {
                cout << "ADC" << endl;
                break;
            }
        case ADCB:
            {
                cout << "ADCB" << endl;
                break;
            }
        case SBC:
            {
                cout << "SBC" << endl;
                break;
            }
        case SBCB:
            {
                cout << "SBCB" << endl;
                break;
            }
        case SXT:
            {
                cout << "SXT" << endl;
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


//double_operand
double_operand::double_operand(): command(), function_code(0), source(0), source_mode(0), destination(0),
                                  destination_mode(0)
{
}

double_operand::double_operand(int to_data, char to_disposition, char * to_tracefile, int to_function_code,
                               int to_source_mode, int to_source, int to_destination_mode,
                               int to_destination): command(to_data, to_disposition, to_tracefile),
                                                    function_code(to_function_code), source(to_source),
                                                    source_mode(to_source_mode), destination(to_destination),
                                                    destination_mode(to_destination_mode)
{
}

void double_operand::disp()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
    cout << "function_code: " << function_code << endl;
    cout << "source_mode: " << source_mode << endl;
    cout << "source: " << source << endl;
    cout << "destination_mode: " << destination_mode << endl;
    cout << "destination: " << destination << endl;
}

int double_operand::instruction(int *regs, CPSR *states, i_cache *program, int position, int * mem)
{
    cout << "function_code: " << function_code << ": ";

    switch(function_code)
    {
        case MOV:
            {
                cout << "MOV" << endl;
                break;
            }
        case MOVB:
            {
                cout << "MOVB" << endl;
                break;
            }
        case CMP: 
            {
                cout << "CMP" << endl;
                break;
            }
        case CMPB:
            {
                cout << "CMPB" << endl;
                break;
            }
        case ADD:
            {
                cout << "ADD" << endl;
                break;
            }
        case SUB:
            {
                cout << "SUB" << endl;
                break;
            }
        case BIT:
            {
                cout << "BIT" << endl;
                break;
            }
        case BITB:
            {
                cout << "BITB" << endl;
                break;
            }
        case BIC:
            {
                cout << "BIC" << endl;
                break;
            }
        case BICB:
            {
                cout << "BICB" << endl;
                break;
            }
        case BIS:
            {
                cout << "BIS" << endl;
                break;
            }
        case BISB:
            {
                cout << "BISB" << endl;
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

>>>>>>> master
