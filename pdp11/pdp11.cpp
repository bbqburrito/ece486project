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


int command::instruction(uint16_t *regs, CPSR *states, i_cache *program)
{
    return 0;
}

int command::instructionB(uint16_t *regs, CPSR *states, i_cache * program)
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

int extended::instruction(uint16_t *regs, CPSR *states, i_cache * program)
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

int trapIntMiscCond::instruction(uint16_t *regs, CPSR *states, i_cache *program)
{
    char answer[25];

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

                cin.sync();
                cout << "End program?(Y/n) ";
                cin.getline(answer, 25);
                cin.sync();

                if(toupper(answer[0]) != 'N')
                    regs[PC] = 0xFFFF;  //set to 65535, so next fetch
                                        //ends execution

                break;
            }
        case WAIT:
            {
                cout << "WAIT" << endl;
 
                cin.sync();
                cout << "End program?(y/N) ";
                cin.getline(answer, 25);
                cin.sync();

                if(toupper(answer[0]) != 'Y')
                    regs[PC] = 0xFFFF;  //set to 65535, so next fetch
                                        //ends execution

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

int jump_sub::instruction(uint16_t *regs, CPSR *states, i_cache *program)
{
    int i;
    int index;
    int deferred;

    cout << "function_code: " << function_code << ": ";
   switch(function_code)
    {
        case JMP:
            {
                
                cout << "JMP" << endl;

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
                                for(i = 0; i < 8; ++i)
                                {
                                    cout << regs[i] << ' ';
                                }
                                cout << endl;
                                break;
                            }
                    case 2: {   //autoincrement:
                                //jump to memory location pointed to 
                                //by memory location in register then 
                                //increment register
                                if((program[regs[destination]].data % 2) || (regs[destination] % 2))
                                {
                                    cout << "boundary error\n";
                                    break;
                                }
                                regs[PC] = program[regs[destination]].data;
                                trace_file(tracefile, 0, regs[destination]);
                                regs[destination] += 2;
                                for(i = 0; i < 8; ++i)
                                {
                                    cout << regs[i] << ' ';
                                }
                                cout << endl;
                                break;
                            }
                    case 3: {   //autoincrement deferred:
                                //jump to memory location pointed to by
                                //memory at location pointed to by register,
                                //then increment register
                                deferred = program[regs[destination]].data;
                                if((deferred % 2) || (regs[destination] % 2) || (program[deferred].data % 2))
                                {
                                    cout << "unaligned reference\n";
                                    break;
                                }

                                regs[PC] = program[deferred].data;
                                trace_file(tracefile, 0, regs[destination]);
                                trace_file(tracefile, 0, deferred);
                                regs[destination] += 2;
                                for(i = 0; i < 8; ++i)
                                {
                                    cout << regs[i] << ' ';
                                }
                                cout << endl;
                                break;
                            }
                    case 4: {   //autodecrement:
                                //decrement register, then jump
                                //to memory location pointed to 
                                //by register
                                if((program[regs[destination]].data % 2) || (regs[destination] % 2))
                                {
                                    cout << "boundary error\n";
                                    break;
                                }
                                regs[destination] -= 2;
                                regs[PC] = program[regs[destination]].data;
                                trace_file(tracefile, 0, regs[destination]);
                                for(i = 0; i < 8; ++i)
                                {
                                    cout << regs[i] << ' ';
                                }
                                cout << endl;
                                break;
                            }
                    case 5: {   //autodecrement deferred:
                                //decrement register, then jump to
                                //memory location pointed to by memory
                                //location pointed to by register
                                regs[destination] -= 2;
                                deferred = program[regs[destination]].data;
                                if((deferred % 2) || (regs[destination] % 2) || (program[deferred].data % 2))
                                {
                                    cout << "boundary error\n";
                                    break;
                                }

                                regs[PC] = program[deferred].data;
                                trace_file(tracefile, 0, regs[destination]);
                                trace_file(tracefile, 0, deferred);
                                for(i = 0; i < 8; ++i)
                                {
                                    cout << regs[i] << ' ';
                                }
                                cout << endl;
                                break;
                            }
                    case 6: {   //index: jump to 
                                //memory location pointed to by register 
                                //plus index, which is located just after
                                //instruction
                                if(destination == PC)
                                    index = regs[PC] + 2 + program[regs[PC]].data;
                                else index = regs[destination] + program[regs[PC]].data;
                                if(index % 2)
                                {
                                    cout << "boundary error\n";
                                    regs[PC] += 2;
                                    break;
                                }

                                regs[PC] = program[index].data;
                                trace_file(tracefile, 0, regs[PC]);
                                trace_file(tracefile, 0, index);
                                for (i = 0; i < 8; ++i)
                                {
                                    cout << regs[i] << ' ';
                                }
                                cout << endl;
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
                                if((index % 2) || (program[index].data % 2))
                                {
                                    cout << "boundary error\n";
                                    regs[PC] += 2;
                                    break;
                                }

                                regs[PC] = program[program[index].data].data;
                                trace_file(tracefile, 0, regs[PC]);
                                trace_file(tracefile, 0, index);
                                trace_file(tracefile, 0, program[index].data);
                                for (i = 0; i < 8; ++i)
                                {
                                    cout << regs[i] << ' ';
                                }
                                cout << endl;
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
                cout << "JSR" << endl;
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
                                program[regs[SP]].data = regs[linkage_reg];
                                regs[SP] += 2;
                                regs[linkage_reg] = regs[PC];
                                regs[PC] = regs[destination];
                                trace_file(tracefile, 0, regs[SP]);
                                for(i = 0; i < 8; ++i)
                                {
                                    cout << regs[i] << ' ';
                                }
                                cout << endl;
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
                                program[regs[SP]].data = regs[linkage_reg];
                                regs[SP] += 2;
                                regs[linkage_reg] = regs[PC];
                                regs[PC] = regs[destination];
                                trace_file(tracefile, 0, regs[SP]);
                                regs[destination] += 2;
                                for(i = 0; i < 8; ++i)
                                {
                                    cout << regs[i] << ' ';
                                }
                                cout << endl;
                                break;
                            }
                    case 3: {   //autoincrement deferred:
                                //push linkage register contents to stack,
                                //replace with location following the 
                                //instruction, jump to address pointed to
                                //by address pointed to by destination
                                //register, then increment register
                                deferred = program[regs[destination]].data;
                                if((deferred % 2) || (regs[destination] % 2))
                                {
                                    cout << "boundary error\n";
                                    break;
                                }
                                program[regs[SP]].data = regs[linkage_reg];
                                regs[SP] += 2;
                                regs[linkage_reg] = regs[PC];
                                regs[PC] = program[regs[destination]].data;
                                trace_file(tracefile, 0, regs[SP]);
                                trace_file(tracefile, 0, regs[destination]);
                                regs[destination] += 2;
                                for(i = 0; i < 8; ++i)
                                {
                                    cout << regs[i] << ' ';
                                }
                                cout << endl;
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
                                program[regs[SP]].data = regs[linkage_reg];
                                regs[SP] += 2;
                                regs[linkage_reg] = regs[PC];
                                regs[PC] = regs[destination];
                                trace_file(tracefile, 0, regs[SP]);
                                for(i = 0; i < 8; ++i)
                                {
                                    cout << regs[i] << ' ';
                                }
                                cout << endl;
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
                                if((deferred % 2) || (regs[destination] % 2))
                                {
                                    cout << "boundary error\n";
                                    break;
                                }
                                regs[destination] -= 2;
                                program[regs[SP]].data = regs[linkage_reg];
                                regs[SP] += 2;
                                regs[linkage_reg] = regs[PC];
                                regs[PC] = program[regs[destination]].data;
                                trace_file(tracefile, 0, regs[SP]);
                                trace_file(tracefile, 0, regs[destination]);
                                for(i = 0; i < 8; ++i)
                                {
                                    cout << regs[i] << ' ';
                                }
                                cout << endl;
                                break;
                            }
                    case 6: {   //index: push linkage register contents to 
                                //stack, replace with location following
                                //location following PC, jump to address
                                //pointed to by register plus index
                                if(destination == PC)
                                    index = regs[PC] + 2 + program[regs[PC]].data;
                                else index = regs[destination] + program[regs[PC]].data;
                                if(index % 2)
                                {
                                    cout << "boundary error\n";
                                    regs[PC] += 2;
                                    break;
                                }

                                program[regs[SP]].data = regs[linkage_reg];
                                regs[SP] += 2;
                                regs[linkage_reg] = regs[PC];
                                regs[PC] = program[index].data;
                                trace_file(tracefile, 0, regs[PC]);
                                trace_file(tracefile, 0, (regs[SP] - 2));
                                trace_file(tracefile, 0, index);
                                for(i = 0; i < 8; ++i)
                                {
                                    cout << regs[i] << ' ';
                                }
                                cout << endl;
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
                                if(index % 2)
                                {
                                    cout << "boundary error\n";
                                    regs[PC] += 2;
                                    break;
                                }

                                program[regs[SP]].data = regs[linkage_reg];
                                regs[SP] += 2;
                                regs[linkage_reg] = regs[PC];
                                regs[PC] = program[program[index].data].data;
                                trace_file(tracefile, 0, regs[PC]);
                                trace_file(tracefile, 0, (regs[SP] - 2));
                                trace_file(tracefile, 0, index);
                                trace_file(tracefile, 0, program[index].data);
                                for(i = 0; i < 8; ++i)
                                {
                                    cout << regs[i] << ' ';
                                }
                                cout << endl;
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
                cout << "RTS" << endl;

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
                regs[SP] += 2;
                trace_file(tracefile, 0, regs[SP]);
                for (i = 0; i < 8; ++i)
                {
                    cout << regs[i] << ' ';
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
    int condition = 0;

    cout << "function_code: " << function_code << ": ";
   switch(function_code)
    {
        case BR:
            {
                cout << "BR" << endl;

                regs[PC] += (2 * (int8_t)offset - 2);

                break;
            }
        case BNE:
            {
                cout << "BNE" << endl;

                condition = states->get_condition() & ZERO;
                if(!condition)
                {
                    regs[PC] += (2 * (int8_t)offset - 2);
                }
                break;
            }
        case BEQ:
            {
                cout << "BEQ" << endl;

                condition = states->get_condition() & ZERO;
                if(condition)
                {
                    regs[PC] += (2 * (int8_t)offset - 2);
                }
                break;
            }
        case BPL:
            {
                cout << "BPL" << endl;

                condition = states->get_condition() & NEGATIVE;
                if(!condition)
                {
                    regs[PC] += (2 * (int8_t)offset - 2);
                }
                break;
            }
        case BMI:
            {
                cout << "BMI" << endl;

                condition = states->get_condition() & NEGATIVE;
                if(condition)
                {
                    regs[PC] += (2 * (int8_t)offset - 2);
                }
                break;
            }
        case BVC:
            {
                cout << "BVC" << endl;

                condition = states->get_condition() & V_OVERFLOW;
                if(!condition)
                {
                    regs[PC] += (2 * (int8_t)offset - 2);
                }
                break;
            }
        case BVS:
            {
                cout << "BVS" << endl;

                condition = states->get_condition() & V_OVERFLOW;
                if(condition)
                {
                    regs[PC] += (2 * (int8_t)offset - 2);
                }
                break;
            }
        case BCC:
            {
                cout << "BCC" << endl;
                break;

                condition = states->get_condition() & CARRY;
                if(!condition)
                {
                    regs[PC] += (2 * (int8_t)offset - 2);
                }
            }
        case BCS:
            {
                cout << "BCS" << endl;

                condition = states->get_condition() & CARRY;
                if(condition)
                {
                    regs[PC] += (2 * (int8_t)offset - 2);
                }
                break;
            }
        case BGE:
            {   //branch if xnor N and V bits
                cout << "BGE" << endl;
 
                //xor N and V condition bits
                condition = states->get_condition() & V_OVERFLOW;
                condition ^= (states->get_condition() & NEGATIVE);
                if(!condition)
                {
                    regs[PC] += (2 * (int8_t)offset - 2);
                }
               break;
            }
        case BLT:
            {   //branch if xor N and V bits
                cout << "BLT" << endl;

                //xor N and V condition bits
                condition = states->get_condition() & V_OVERFLOW;
                condition ^= (states->get_condition() & NEGATIVE);
                if(condition)
                {
                    regs[PC] += (2 * (int8_t)offset - 2);
                }
                break;
            }
        case BGT:
            {   //branch if xnor N and V bits or Z bit

                cout << "BGT" << endl;
 
                //xor N and V condition bits
                condition = states->get_condition() & V_OVERFLOW;
                condition ^= (states->get_condition() & NEGATIVE);
                condition |= (states->get_condition() & ZERO);
                if(!condition)
                {
                    regs[PC] += (2 * (int8_t)offset - 2);
                }
                break;
            }
        case BLE:
            {
                cout << "BLE" << endl;
 
                //xor N and V condition bits
                condition = states->get_condition() & V_OVERFLOW;
                condition ^= (states->get_condition() & NEGATIVE);
                condition |= (states->get_condition() & ZERO);
                if(condition)
                {
                    regs[PC] += (2 * (int8_t)offset - 2);
                }
                break;
            }
        case BHI:
            {   //branch if C & Z both zero
                cout << "BHI" << endl;
 
                condition = (states->get_condition() & CARRY);
                condition |= (states->get_condition() & ZERO);
                if(!condition)
                {
                    regs[PC] += (2 * (int8_t)offset - 2);
                }
                break;
 
                break;
            }
        case BLOS:
            {   //branch if C | Z set
                cout << "BLOS" << endl;
  
                condition = (states->get_condition() & CARRY);
                condition |= (states->get_condition() & ZERO);
                if(condition)
                {
                    regs[PC] += (2 * (int8_t)offset - 2);
                }
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

int single_operand::instruction(uint16_t *regs, CPSR *states, i_cache *program)
{
    int i;
    int outcome = -1;
    int deferred = 0;   //temp for deferred addressing
    int index = 0;      //temp for index addressing
    int condition;      //temp for condition codes

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
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, destination);
                        states->set_condition(4);
                        outcome = 1;
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
                        break;
                    }
                    case 6: {       //index:
                                    //set memory pointed to by register plus
                                    //index, which is located just after instruction,
                                    //to 0
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if(index % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }


                        program[index].data = 0;        
                        program[index + 1].data = 0;    
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 1, (uint16_t)index);
                        regs[PC] += 2;
                        states->set_condition(4);
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        if((index % 2) || (program[index].data % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }


                        trace_file(tracefile, 0, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        program[program[index].data].data = 0;
                        program[program[index].data + 1].data = 0;
                        trace_file(tracefile, 1, program[index].data);
                        regs[PC] += 2;
                        states->set_condition(4);
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                switch (destination_mode) {
                    case 0: {           //register:
                                        //flip register bits
                        regs[destination] ^= 0xFFFF;     //flip bits
                        states->set_condition(((regs[destination] >> 15) << 3)  | (!(regs[destination]) << 2) | 1);
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[regs[destination] + 1].data ^= 0xFFFF;   //flip bits
                        states->set_condition(((program[regs[destination]].data >> 15) << 3)  | (!(program[regs[destination]].data) << 2) | 1);
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[regs[destination] + 1].data ^= 0xFFFF;
                        trace_file(tracefile, 1, regs[destination]);    //write data write to trace file
                        regs[destination] += 2;
                        states->set_condition(((program[regs[destination]].data >> 15) << 3)  | (!(program[regs[destination]].data) << 2) | 1);
                        outcome = 1;
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[deferred + 1].data ^= 0xFFFF;
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        states->set_condition(((program[deferred].data >> 15) << 3)  | (!(program[deferred].data) << 2) | 1);
                        outcome = 1;
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[regs[destination] + 1].data ^= 0xFFFF;
                        states->set_condition(((program[regs[destination]].data >> 15) << 3)  | (!(program[regs[destination]].data) << 2) | 1);
                        trace_file(tracefile, 1, regs[destination]);
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[deferred + 1].data ^= 0xFFFF;
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        states->set_condition(((program[deferred].data >> 15) << 3)  | (!(program[deferred].data) << 2) | 1);
                        outcome = 1;
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
                        break;
                    }
                    case 6: {       //index:
                                    //set memory pointed to by register plus
                                    //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if(index % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }
                        program[index].data ^= 0xFFFF;
                        program[index + 1].data ^= 0xFFFF;
                        trace_file(tracefile, 1, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        regs[PC] += 2;
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        if((index % 2) || (program[index].data % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }
                        program[program[index].data].data ^= 0xFFFF;
                        program[program[index].data + 1].data ^= 0xFFFF;
                    
                        regs[PC] += 2;
                        states->set_condition(((program[program[index].data].data >> 15) << 3)  | (!(program[program[index].data].data) << 2) | 1);
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 0, (uint16_t)index);
                        trace_file(tracefile, 1, program[index].data);
                        for(i = 0; i < 8; ++i)
                        {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;

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
                cout << "INC" << endl;
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
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[regs[destination] + 1].data += 1;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[deferred + 1].data += 1;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[regs[destination] + 1].data += 1;
                        if (program[regs[destination]].data == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[regs[destination]].data += 1;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[deferred + 1].data += 1;
                        if (program[deferred].data == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[deferred].data += 1;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
                        break;
                    }
                    case 6: {       //index:
                        //set memory pointed to by register plus
                        //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if(index % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }
                        condition = states->get_condition() & 1;
                        program[index + 1].data += 1;
                        if (program[index].data == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[index].data += 1;
                        if (!(program[index].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[index].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 1, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        regs[PC] += 2;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        if((index % 2) || (program[index].data % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = states->get_condition() & 1;    //increment
                        program[program[index].data + 1].data += 1;
                        if (program[program[index].data].data == 0077777)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= V_OVERFLOW;
                        program[program[index].data].data += 1;     //increment
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
                        trace_file(tracefile, 1, program[index].data);
                        
                        regs[PC] += 2;

                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;

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
                cout << "INCB" << endl;
                break;
            }
        case DEC:
            {
                cout << "DEC" << endl;
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
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[regs[destination]].data -= 1;
                        if (program[regs[destination] + 1].data == 0100000)        //increment and check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[regs[destination] + 1].data -= 1;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[regs[destination] + 1].data -= 1;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[deferred + 1].data -= 1;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[regs[destination] + 1].data -= 1;
                        if (program[regs[destination]].data == 0100000)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[regs[destination]].data -= 1;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[deferred + 1].data -= 1;
                        if (program[deferred].data == 0100000)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[deferred].data -= 1;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
                        break;
                    }
                    case 6: {       //index:
                        //set memory pointed to by register plus
                        //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if(index % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }
                        condition = states->get_condition() & 1;
                        program[index + 1].data -= 1;
                        if (program[index].data == 0100000)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[index].data -= 1;
                        if (!(program[index].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[index].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 1, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        regs[PC] += 2;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        if((index % 2) || (program[index].data % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = states->get_condition() & 1;    //increment
                        program[program[index].data + 1].data -= 1;
                        if (program[program[index].data].data == 0100000)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        program[program[index].data].data -= 1;     //increment
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
                        trace_file(tracefile, 1, program[index].data);
                        
                        regs[PC] += 2;

                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;

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
                cout << "NEG" << endl;
                switch (destination_mode) {
                    case 0: {           //register:
                        //invert signed value
                        condition = states->get_condition();
                        regs[destination] &= 0xFFFF;
                        regs[destination] += 1;
                        if ((regs[destination]) == 0100000)        //check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        if (!(regs[destination]))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)regs[destination] < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;
                        if (!regs[destination])
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        states->set_condition(condition);       //set conditions
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[regs[destination]].data ^= 0xFFFF;
                        program[regs[destination]].data += 1;
                        program[regs[destination] + 1].data ^= 0xFFFF;
                        program[regs[destination] + 1].data += 1;
                        if (program[regs[destination]].data == 0100000)        //increment and check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;
                        if (!(program[regs[destination]].data))
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[regs[destination]].data ^= 0xFFFF;
                        program[regs[destination]].data += 1;
                        program[regs[destination] + 1].data ^= 0xFFFF;
                        program[regs[destination] + 1].data += 1;
                        if (program[regs[destination]].data == 0100000)        //increment and check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;
                        if (!(program[regs[destination]].data))
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[deferred].data ^= 0xFFFF;
                        program[deferred].data += 1;
                        program[deferred + 1].data ^= 0xFFFF;
                        program[deferred + 1].data += 1;
                        if (program[deferred].data == 0100000)        //increment and check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[deferred].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;
                        if (!(program[deferred].data))
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[regs[destination]].data ^= 0xFFFF;
                        program[regs[destination]].data += 1;
                        program[regs[destination] + 1].data ^= 0xFFFF;
                        program[regs[destination] + 1].data += 1;
                        if (program[regs[destination]].data == 0100000)        //increment and check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        if (!(program[regs[destination]].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[regs[destination]].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;
                        if (!(program[regs[destination]].data))
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        program[deferred].data ^= 0xFFFF;
                        program[deferred].data += 1;
                        program[deferred + 1].data ^= 0xFFFF;
                        program[deferred + 1].data += 1;
                        if (program[deferred].data == 0100000)        //increment and check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        if (!(program[deferred].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[deferred].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;
                        if (!(program[deferred].data))
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, regs[destination]);
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
                        break;
                    }
                    case 6: {       //index:
                        //set memory pointed to by register plus
                        //index, which is located just after instruction
                        if (destination == PC)
                            index = regs[PC] + 2 + program[regs[PC]].data;
                        else index = regs[destination] + program[regs[PC]].data;
                        if(index % 2)
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }
                        condition = states->get_condition();    //get condition bit
                        program[index].data ^= 0xFFFF;
                        program[index].data += 1;
                        program[index + 1].data ^= 0xFFFF;
                        program[index + 1].data += 1;
                        if (program[index].data == 0100000)        //increment and check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        if (!(program[index].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[index].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;
                        if (!(program[index].data))
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 1, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        regs[PC] += 2;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        if((index % 2) || (program[index].data % 2))
                        {
                            cout << "unaligned reference\n";
                            regs[PC] += 2;
                            break;
                        }

                        condition = states->get_condition();    //get condition bit
                        program[program[index].data].data ^= 0xFFFF;
                        program[program[index].data].data += 1;
                        program[program[index].data + 1].data ^= 0xFFFF;
                        program[program[index].data + 1].data += 1;
                        if (program[program[index].data].data == 0100000)        //increment and check for overflow
                        {
                            condition |= V_OVERFLOW;
                        } else condition &= ~V_OVERFLOW;
                        if (!(program[program[index].data].data))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;
                        if ((int16_t)program[program[index].data].data < 0)          //check for negative
                        {
                            condition |= NEGATIVE;
                        }else condition &= ~NEGATIVE;
                        if (!(program[program[index].data].data))
                        {
                            condition |= CARRY;
                        } else condition &= ~CARRY;

                        states->set_condition(condition);       //set conditions
                        trace_file(tracefile, 0, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 1, program[index].data);
                        
                        regs[PC] += 2;

                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;

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
                cout << "TST" << endl;
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
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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

                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;

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
                cout << "ASR" << endl;

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
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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


                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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


                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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

                        trace_file(tracefile, 1, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        regs[PC] += 2;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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

                        trace_file(tracefile, 0, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 1, program[index].data);
                        
                        regs[PC] += 2;

                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;

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
 
               cout << "ASL" << endl;

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
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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


                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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


                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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

                        trace_file(tracefile, 1, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        regs[PC] += 2;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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

                        trace_file(tracefile, 0, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 1, program[index].data);
                        
                        regs[PC] += 2;

                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;

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
                cout << "ROR" << endl;
 
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
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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


                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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


                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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

                        trace_file(tracefile, 1, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        regs[PC] += 2;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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

                        trace_file(tracefile, 0, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 1, program[index].data);
                        
                        regs[PC] += 2;

                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;

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
                cout << "ROL" << endl;
 
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
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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


                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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


                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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

                        trace_file(tracefile, 1, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        regs[PC] += 2;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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

                        trace_file(tracefile, 0, (uint16_t)index);
                        trace_file(tracefile, 0, regs[PC]);
                        trace_file(tracefile, 1, program[index].data);
                        
                        regs[PC] += 2;

                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;

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
                cout << "SWAB" << endl;
  
                switch (destination_mode) {
                    case 0: {           //register:
                        // swap bytes
                        condition = (regs[destination] << 8) & 0xFF00;    //use condition as swap variable to store low order byte
                        regs[destination] = ((regs[destination] >> 8) & 0xFF) | condition; //swap bytes 
                        regs[destination + 1] = regs[destination];

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
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)index);
                        outcome = 1;
                        regs[PC] += 2;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 0, (uint16_t)index);
                        trace_file(tracefile, 1, program[index].data);
                        outcome = 1;
                        regs[PC] += 2;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                cout << "ADC" << endl;
   
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
                        regs[destination + 1] = regs[destination];

                        if (!(regs[destination]))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((regs[destination] >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)index);
                        outcome = 1;
                        regs[PC] += 2;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 0, (uint16_t)index);
                        trace_file(tracefile, 1, program[index].data);
                        outcome = 1;
                        regs[PC] += 2;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                cout << "SBC" << endl;
    
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
                        regs[destination + 1] = regs[destination];

                        if (!(regs[destination]))           //check for zero
                        {
                            condition |= ZERO;
                        } else condition &= ~ZERO;

                        if ((regs[destination] >> 15) & 1)          //check for negative
                        {
                            condition |= NEGATIVE;
                        } else condition &= ~NEGATIVE;

                        states->set_condition(condition);       //set conditions
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        regs[destination] += 2;
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)deferred);
                        outcome = 1;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 1, (uint16_t)index);
                        outcome = 1;
                        regs[PC] += 2;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                        trace_file(tracefile, 0, (uint16_t)index);
                        trace_file(tracefile, 1, program[index].data);
                        outcome = 1;
                        regs[PC] += 2;
                        for (i = 0; i < 8; ++i) {
                            cout << regs[i] << ' ';
                        }
                        cout << endl;
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
                cout << "SXT" << endl;

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

                                for(i = 0; i < 8; ++i)
                                    cout << regs[i] << ' ';

                                cout << endl;
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

                                for(i = 0; i < 8; ++i)
                                    cout << regs[i] << ' ';

                                cout << endl;
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
                                for(i = 0; i < 8; ++i)
                                    cout << regs[i] << ' ';

                                cout << endl;
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
                                for(i = 0; i < 8; ++i)
                                    cout << regs[i] << ' ';

                                cout << endl;
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

                                for(i = 0; i < 8; ++i)
                                    cout << regs[i] << ' ';

                                cout << endl;
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

                                for(i = 0; i < 8; ++i)
                                    cout << regs[i] << ' ';

                                cout << endl;
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
                                for(i = 0; i < 8; ++i)
                                    cout << regs[i] << ' ';

                                cout << endl;
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
                                for(i = 0; i < 8; ++i)
                                    cout << regs[i] << ' ';

                                cout << endl;
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


