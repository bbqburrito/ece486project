//file to contain all double operand instructions
//double_operand

#include "pdp11.h"

using namespace std;

double_operand::double_operand(): command(), function_code(0), source_mode(0), source(0), destination_mode(0), destination(0)
{
}

double_operand::double_operand(int to_data, char to_disposition, char * tracefile, int to_function_code, int to_source_mode, int to_source, int to_destination_mode, int to_destination): command(to_data, to_disposition, tracefile), function_code(to_function_code), source_mode(to_source_mode), source(to_source), destination_mode(to_destination_mode), destination(to_destination)
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


//bic
int double_operand::bic(uint16_t *regs, CPSR *states, i_cache *program)
{
    uint16_t index = 0;
    int source_op = make_source(regs, program);
    int condition = 0;
    uint16_t result;

    if(0x10000 < source_op)
        return source_op;

    //perform action for different register modes
    switch(destination_mode)
    {
        case 0:
            {   //register
                result = ~(uint16_t)source_op & regs[destination];

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;

                regs[destination] = result;

                states->set_condition(condition);
 
                return (int) regs[destination];
                break;
            }

        case 1:
            {   //register deferred
                if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = ~(uint16_t)source_op & program[regs[destination]].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;

                program[regs[destination]].data = result;
                program[regs[destination] + 1].data = result;

                states->set_condition(condition);
 
                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 1, regs[destination]);
                return (int) program[regs[destination]].data;
                break;
            }


        case 2:
            {   //autoincrement:
                 if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = ~(uint16_t)source_op & program[regs[destination]].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;


                program[regs[destination]].data = result;
                program[regs[destination] + 1].data = result;
                states->set_condition(condition);

                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 1, regs[destination]);
                regs[destination] += 2;
 
                return (int) program[regs[destination] - 2].data;
                break;
            }



       case 3: {   //autoincrement deferred
                if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = ~(uint16_t)source_op & program[program[regs[destination]].data].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;

                program[program[regs[destination]].data].data = result;
                program[program[regs[destination]].data + 1].data = result;

                states->set_condition(condition);

                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 0, program[regs[destination]].data);
                trace_file(tracefile, 1, program[regs[destination]].data);
                regs[destination] += 2;
 
                return (int) program[regs[destination] - 2].data;
                break;
            }


        case 4: {   //autodecrement

                regs[destination] -= 2;
                  if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = ~(uint16_t)source_op & program[regs[destination]].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;


                program[regs[destination]].data = result;
                program[regs[destination] + 1].data = result;
                states->set_condition(condition);

                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 1, regs[destination]);
 
                return (int) program[regs[destination]].data;
                break;
            }


        case 5: {   //autodecrement deferred
                regs[destination] -= 2;
                 if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = ~(uint16_t)source_op & program[program[regs[destination]].data].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;

                program[program[regs[destination]].data].data = result;
                program[program[regs[destination]].data + 1].data = result;

                states->set_condition(condition);

                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 0, program[regs[destination]].data);
                trace_file(tracefile, 1, program[regs[destination]].data);
 
                return (int) program[regs[destination]].data;
                break;
            }



        case 6: {   //index
            if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2)) {
                cout << "boundary error\n";
                return 0x10000;
            }
                result = ~(uint16_t)source_op & program[index].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;


                program[index].data = result;
                program[index + 1].data = result;
                states->set_condition(condition);

                trace_file(tracefile, 0, regs[PC]);
                trace_file(tracefile, 0, index);
                trace_file(tracefile, 1, index);

                regs[PC] += 2;
 
                return (int) program[index].data;
                break;
            }


        case 7: {   //register deferred
             if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2) || (program[index].data % 2)) 
            {
                cout << "boundary error\n";
                return 0x10000;
            }
                result = ~(uint16_t)source_op & program[program[index].data].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;


                program[program[index].data].data = result;
                program[program[index].data + 1].data = result;
                states->set_condition(condition);

                trace_file(tracefile, 0, regs[PC]);
                trace_file(tracefile, 0, index);
                trace_file(tracefile, 0, program[index].data);
                trace_file(tracefile, 1, program[index].data);
 

                regs[PC] += 2;
 
                return (int) program[program[index].data].data;
                break;
            }

        default: {
            cout << "invalid destination mode\n";
            return 0x10000;
            break;
        }
    }

    return 0x10000;
}

//bit
int double_operand::bit(uint16_t *regs, CPSR *states, i_cache *program)
{
    uint16_t index = 0;
    int source_op = make_source(regs, program);
    int condition = 0;
    uint16_t result;

    if(0x10000 < source_op)
        return source_op;

    //perform action for different register modes
    switch(destination_mode)
    {
        case 0:
            {   //register
                result = (uint16_t)source_op & regs[destination];

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;

                states->set_condition(condition);
 
                return (int) result;
                break;
            }

        case 1:
            {   //register deferred
                if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = (uint16_t)source_op & program[regs[destination]].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;

                states->set_condition(condition);
 
                trace_file(tracefile, 0, regs[destination]);
                return (int) result;
                break;
            }


        case 2:
            {   //autoincrement:
                 if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = (uint16_t)source_op & program[regs[destination]].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;


                states->set_condition(condition);

                trace_file(tracefile, 0, regs[destination]);
                regs[destination] += 2;
 
                return (int) result;
                break;
            }



       case 3: {   //autoincrement deferred
                if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = (uint16_t)source_op & program[program[regs[destination]].data].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;


                states->set_condition(condition);

                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 0, program[regs[destination]].data);
                regs[destination] += 2;
 
                return (int) result;
                break;
            }


        case 4: {   //autodecrement

                regs[destination] -= 2;
                  if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = (uint16_t)source_op & program[regs[destination]].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;


                states->set_condition(condition);

                trace_file(tracefile, 0, regs[destination]);
 
                return (int) result;
                break;
            }


        case 5: {   //autodecrement deferred
                regs[destination] -= 2;
                 if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = (uint16_t)source_op & program[program[regs[destination]].data].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;


                states->set_condition(condition);

                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 0, program[regs[destination]].data);
 
                return (int) result;
                break;
            }

        case 6: {   //index
            if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2)) {
                cout << "boundary error\n";
                return 0x10000;
            }
                result = (uint16_t)source_op & program[index].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;


                states->set_condition(condition);

                trace_file(tracefile, 0, regs[PC]);
                trace_file(tracefile, 0, index);

                regs[PC] += 2;
 
                return (int) result;
                break;
            }


        case 7: {   //register deferred
             if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2) || (program[index].data % 2)) 
            {
                cout << "boundary error\n";
                return 0x10000;
            }
                result = (uint16_t)source_op & program[program[index].data].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;


                states->set_condition(condition);

                trace_file(tracefile, 0, regs[PC]);
                trace_file(tracefile, 0, index);
                trace_file(tracefile, 0, program[index].data);

                regs[PC] += 2;
 
                return (int) result;
                break;
            }

        default: {
            cout << "invalid destination mode\n";
            return 0x10000;
            break;
        }
    }

    return 0x10000;
}

//bis
int double_operand::bis(uint16_t *regs, CPSR *states, i_cache *program)
{
    uint16_t index = 0;
    int source_op = make_source(regs, program);
    int condition = 0;
    uint16_t result;

    if(0x10000 < source_op)
        return source_op;

    //perform action for different register modes
    switch(destination_mode)
    {
        case 0:
            {   //register
                result = (uint16_t)source_op | regs[destination];

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;

                regs[destination] = result;

                states->set_condition(condition);
 
                return (int) regs[destination];
                break;
            }

        case 1:
            {   //register deferred
                if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = (uint16_t)source_op | program[regs[destination]].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;

                program[regs[destination]].data = result;
                program[regs[destination] + 1].data = result;

                states->set_condition(condition);
 
                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 1, regs[destination]);
                return (int) program[regs[destination]].data;
                break;
            }


        case 2:
            {   //autoincrement:
                 if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = (uint16_t)source_op | program[regs[destination]].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;


                program[regs[destination]].data = result;
                program[regs[destination] + 1].data = result;
                states->set_condition(condition);

                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 1, regs[destination]);
                regs[destination] += 2;
 
                return (int) program[regs[destination] - 2].data;
                break;
            }



       case 3: {   //autoincrement deferred
                if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = (uint16_t)source_op | program[program[regs[destination]].data].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;

                program[program[regs[destination]].data].data = result;
                program[program[regs[destination]].data + 1].data = result;

                states->set_condition(condition);

                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 0, program[regs[destination]].data);
                trace_file(tracefile, 1, program[regs[destination]].data);
                regs[destination] += 2;
 
                return (int) program[regs[destination] - 2].data;
                break;
            }


        case 4: {   //autodecrement

                regs[destination] -= 2;
                  if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = (uint16_t)source_op | program[regs[destination]].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;


                program[regs[destination]].data = result;
                program[regs[destination] + 1].data = result;
                states->set_condition(condition);

                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 1, regs[destination]);
 
                return (int) program[regs[destination]].data;
                break;
            }


        case 5: {   //autodecrement deferred
                regs[destination] -= 2;
                 if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = (uint16_t)source_op | program[program[regs[destination]].data].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;

                program[program[regs[destination]].data].data = result;
                program[program[regs[destination]].data + 1].data = result;

                states->set_condition(condition);

                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 0, program[regs[destination]].data);
                trace_file(tracefile, 1, program[regs[destination]].data);
 
                return (int) program[regs[destination]].data;
                break;
            }



        case 6: {   //index
            if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2)) {
                cout << "boundary error\n";
                return 0x10000;
            }
                result = (uint16_t)source_op | program[index].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;


                program[index].data = result;
                program[index + 1].data = result;
                states->set_condition(condition);

                trace_file(tracefile, 0, regs[PC]);
                trace_file(tracefile, 0, index);
                trace_file(tracefile, 1, index);
 

                regs[PC] += 2;
 
                return (int) program[index].data;
                break;
            }


        case 7: {   //register deferred
             if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2) || (program[index].data % 2)) 
            {
                cout << "boundary error\n";
                return 0x10000;
            }
                result = (uint16_t)source_op | program[program[index].data].data;

                condition = states->get_condition() & CARRY;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;


                program[program[index].data].data = result;
                program[program[index].data + 1].data = result;
                states->set_condition(condition);

                trace_file(tracefile, 0, regs[PC]);
                trace_file(tracefile, 0, index);
                trace_file(tracefile, 0, program[index].data);
                trace_file(tracefile, 1, program[index].data);
 

                regs[PC] += 2;
 
                return (int) program[program[index].data].data;
                break;
            }

        default: {
            cout << "invalid destination mode\n";
            return 0x10000;
            break;
        }
    }

    return 0x10000;
}





//subtract
int double_operand::sub(uint16_t *regs, CPSR *states, i_cache *program)
{
    uint16_t index = 0;
    int source_op = make_source(regs, program);
    int sign_source = (uint16_t)source_op >> 15;
    int condition = 0;
    uint16_t result;

    if(0xFFFF < source_op)
        return source_op;

    //perform action for different register modes
    switch(destination_mode)
    {
        case 0:
            {   //register
                result = regs[destination] + ~(uint16_t)source_op + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (regs[destination] >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)regs[destination] < (int16_t)source_op)
                    condition |= CARRY;

                regs[destination] = result;

                states->set_condition(condition);
 
                return (int) regs[destination];
                break;
            }

        case 1:
            {   //register deferred
                if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }

                result = ~(uint16_t)source_op + program[regs[destination]].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[regs[destination]].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)program[regs[destination]].data < (int16_t)source_op)
                    condition |= CARRY;

                program[regs[destination]].data = result;
                program[regs[destination] + 1].data = result;

                states->set_condition(condition);

                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 1, regs[destination]);
 
                return (int) program[regs[destination]].data;
                break;
            }


        case 2:
            {   //autoincrement:
                 if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }

                result = ~(uint16_t)source_op + program[regs[destination]].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[regs[destination]].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)program[regs[destination]].data < (int16_t)source_op)
                    condition |= CARRY;

                program[regs[destination]].data = result;
                program[regs[destination] + 1].data = result;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 1, regs[destination]);
                regs[destination] += 2;
 
                return (int) program[regs[destination]].data;
                break;
            }


       case 3: {   //autoincrement deferred
                if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
 
                result = ~(uint16_t)source_op + program[program[regs[destination]].data].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[program[regs[destination]].data].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)program[regs[destination]].data < (int16_t)source_op)
                    condition |= CARRY;

                program[program[regs[destination]].data].data = result;
                program[program[regs[destination]].data + 1].data = result;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 0, program[regs[destination]].data);
                trace_file(tracefile, 1, program[regs[destination]].data);
                regs[destination] += 2;
 
                return (int) program[program[regs[destination]].data].data;
                break;
            }


        case 4: {   //autodecrement

                regs[destination] -= 2;
                  if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }

                result = ~(uint16_t)source_op + program[regs[destination]].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[regs[destination]].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)program[regs[destination]].data < (int16_t)source_op)
                    condition |= CARRY;
                
                program[regs[destination]].data = result;
                program[regs[destination] + 1].data = result;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 1, regs[destination]);
 
                return (int) program[regs[destination]].data;
                break;
            }


        case 5: {   //autodecrement deferred
                regs[destination] -= 2;
                 if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
 
                result = ~(uint16_t)source_op + program[program[regs[destination]].data].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[program[regs[destination]].data].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)program[regs[destination]].data < (int16_t)source_op)
                    condition |= CARRY;

                program[program[regs[destination]].data].data = result;
                program[program[regs[destination]].data + 1].data = result;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 0, program[regs[destination]].data);
                trace_file(tracefile, 1, program[regs[destination]].data);
 
                return (int) program[program[regs[destination]].data].data;
                break;
            }



        case 6: {   //index
            if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2)) {
                cout << "boundary error\n";
                return 0x10000;
            }

                result = ~(uint16_t)source_op + program[index].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[regs[destination]].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;

                if((int16_t)program[index].data < (int16_t)source_op)
                    condition |= CARRY;

                states->set_condition(condition);

                program[index].data = result;
                program[index + 1].data = result;

                trace_file(tracefile, 0, regs[PC]);
                trace_file(tracefile, 0, index);
                trace_file(tracefile, 1, index);
                regs[PC] += 2;
 
                return (int) program[index].data;
                break;
            }


        case 7: {   //register deferred
             if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2) || (program[index].data % 2)) 
            {
                cout << "boundary error\n";
                return 0x10000;
            }

                result = ~(uint16_t)source_op + program[program[index].data].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[program[index].data].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;

                if((int16_t)program[program[index].data].data < (int16_t)source_op)
                    condition |= CARRY;

                states->set_condition(condition);

                program[program[index].data].data = result;
                program[program[index].data + 1].data = result;

                trace_file(tracefile, 0, regs[PC]);
                trace_file(tracefile, 0, index);
                trace_file(tracefile, 0, program[index].data);
                trace_file(tracefile, 1, program[index].data);
                regs[PC] += 2;
 
                return (int) program[program[index].data].data;
                break;
            }


        default: {
            cout << "invalid destination mode\n";
            return 0x10000;
            break;
        }
    }

    return 0x10000;
}





//add function
int double_operand::add(uint16_t *regs, CPSR *states, i_cache *program)
{
    uint16_t index = 0;
    int source_op = make_source(regs, program);
    int condition = 0;
    int result = 0;
    int sign = 0;

    if(0xFFFF < source_op)
        return source_op;
    
    switch(destination_mode)
    {
        case 0:
            {   //register
                result = regs[destination] + (uint16_t)source_op;
                sign = (uint16_t)result >> 15;

                if(result < 0x10000)     //set carry bit if result greater than 0xFFFF
                    condition = 0;
                else condition = CARRY;

                condition |= states->get_condition();
                if(!(uint16_t)result)
                {
                    condition |= ZERO;
                }
                if(sign)
                    condition |= NEGATIVE;
                
                if(!(((uint16_t)source_op >> 15) ^ (regs[destination] >> 15)) && ((regs[destination] >> 15) != sign))
                    condition |= V_OVERFLOW;

                regs[destination] += ((uint16_t)source_op);

                states->set_condition(condition);
 
                return (int) regs[destination];
                break;
            }

        case 1:
            {   //register deferred
                if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = program[regs[destination]].data + (uint16_t)source_op;
                sign = (uint16_t)result >> 15;

                if(result < 0x10000)     //set carry bit if result greater than 0xFFFF
                    condition = 0;
                else condition = CARRY;

                condition |= states->get_condition();
                if(!(uint16_t)result)
                {
                    condition |= ZERO;
                }
                if(sign)
                    condition |= NEGATIVE;
                
                if(!(((uint16_t)source_op >> 15) ^ (program[regs[destination]].data >> 15)) && ((program[regs[destination]].data >> 15) != sign))
                    condition |= V_OVERFLOW;

                program[regs[destination]].data += ((uint16_t)source_op);
                program[regs[destination] + 1].data = program[regs[destination]].data;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 1, regs[destination]);
 
                return (int) program[regs[destination]].data;
                break;
            }


        case 2:
            {   //autoincrement
                if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = program[regs[destination]].data + (uint16_t)source_op;
                sign = (uint16_t)result >> 15;

                if(result < 0x10000)     //set carry bit if result greater than 0xFFFF
                    condition = 0;
                else condition = CARRY;

                condition |= states->get_condition();
                if(!(uint16_t)result)
                {
                    condition |= ZERO;
                }
                if(sign)
                    condition |= NEGATIVE;
                
                if(!(((uint16_t)source_op >> 15) ^ (program[regs[destination]].data >> 15)) && ((program[regs[destination]].data >> 15) != sign))
                    condition |= V_OVERFLOW;

                program[regs[destination]].data += ((uint16_t)source_op);
                program[regs[destination] + 1].data = program[regs[destination]].data;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 1, regs[destination]);

                regs[destination] += 2;
 
                return (int) program[regs[destination]].data;
                break;
            }



       case 3: {   //autoincrement deferred
                if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                result = program[program[regs[destination]].data].data + (uint16_t)source_op;
                sign = (uint16_t)result >> 15;

                if(result < 0x10000)     //set carry bit if result greater than 0xFFFF
                    condition = 0;
                else condition = CARRY;

                condition |= states->get_condition();
                if(!(uint16_t)result)
                {
                    condition |= ZERO;
                }
                if(sign)
                    condition |= NEGATIVE;
                
                if(!(((uint16_t)source_op >> 15) ^ (program[program[regs[destination]].data].data >> 15)) && ((program[program[regs[destination]].data].data >> 15) != sign))
                    condition |= V_OVERFLOW;

                program[program[regs[destination]].data].data += ((uint16_t)source_op);
                program[program[regs[destination]].data + 1].data = program[program[regs[destination]].data].data;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 0, program[regs[destination]].data);
                trace_file(tracefile, 1, program[regs[destination]].data);

                regs[destination] += 2;
 
                return (int) program[regs[destination]].data;
                break;
            }

        case 4: {   //autodecrement
                if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                regs[destination] -= 2;
                result = program[regs[destination]].data + (uint16_t)source_op;
                sign = (uint16_t)result >> 15;

                if(result < 0x10000)     //set carry bit if result greater than 0xFFFF
                    condition = 0;
                else condition = CARRY;

                condition |= states->get_condition();
                if(!(uint16_t)result)
                {
                    condition |= ZERO;
                }
                if(sign)
                    condition |= NEGATIVE;
                
                if(!(((uint16_t)source_op >> 15) ^ (program[regs[destination]].data >> 15)) && ((program[regs[destination]].data >> 15) != sign))
                    condition |= V_OVERFLOW;

                program[regs[destination]].data += ((uint16_t)source_op);
                program[regs[destination] + 1].data = program[regs[destination]].data;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 1, regs[destination]);
 
                return (int) program[regs[destination]].data;
                break;
            }

        case 5: {   //autodecrement deferred
                if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                regs[destination] -= 2;
                result = program[program[regs[destination]].data].data + (uint16_t)source_op;
                sign = (uint16_t)result >> 15;

                if(result < 0x10000)     //set carry bit if result greater than 0xFFFF
                    condition = 0;
                else condition = CARRY;

                condition |= states->get_condition();
                if(!(uint16_t)result)
                {
                    condition |= ZERO;
                }
                if(sign)
                    condition |= NEGATIVE;
                
                if(!(((uint16_t)source_op >> 15) ^ (program[program[regs[destination]].data].data >> 15)) && ((program[program[regs[destination]].data].data >> 15) != sign))
                    condition |= V_OVERFLOW;

                program[program[regs[destination]].data].data += ((uint16_t)source_op);
                program[program[regs[destination]].data + 1].data = program[program[regs[destination]].data].data;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 0, program[regs[destination]].data);
                trace_file(tracefile, 1, program[regs[destination]].data);
 
                return (int) program[regs[destination]].data;
                break;
            }

        case 6: {   //index
            if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2)) {
                cout << "boundary error\n";
                return 0x10000;
            }

                result = program[index].data + (uint16_t)source_op;
                sign = (uint16_t)result >> 15;

                if(result < 0x10000)     //set carry bit if result greater than 0xFFFF
                    condition = 0;
                else condition = CARRY;

                condition |= states->get_condition();
                if(!(uint16_t)result)
                {
                    condition |= ZERO;
                }
                if(sign)
                    condition |= NEGATIVE;
                
                if(!(((uint16_t)source_op >> 15) ^ (program[index].data >> 15)) && ((program[index].data >> 15) != sign))
                    condition |= V_OVERFLOW;

                program[index].data += ((uint16_t)source_op);
                program[index + 1].data = program[index].data; 

                states->set_condition(condition);

            trace_file(tracefile, 0, regs[PC]);
            trace_file(tracefile, 0, index);
            trace_file(tracefile, 1, index);
            regs[PC] += 2;
 
               return (int) program[index].data;
                break;
            }

        case 7: {   //index deferred
            if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2) || (program[index].data % 2)) {
                cout << "boundary error\n";
                return 0x10000;
            }

                result = program[program[index].data].data + (uint16_t)source_op;
                sign = (uint16_t)result >> 15;

                if(result < 0x10000)     //set carry bit if result greater than 0xFFFF
                    condition = 0;
                else condition = CARRY;

                condition |= states->get_condition();
                if(!(uint16_t)result)
                {
                    condition |= ZERO;
                }
                if(sign)
                    condition |= NEGATIVE;
                
                if(!(((uint16_t)source_op >> 15) ^ (program[program[index].data].data >> 15)) && ((program[program[index].data].data >> 15) != sign))
                    condition |= V_OVERFLOW;

                program[program[index].data].data += ((uint16_t)source_op);
                program[program[index].data + 1].data = program[program[index].data].data;

                states->set_condition(condition);

            trace_file(tracefile, 0, regs[PC]);
            trace_file(tracefile, 0, index);
            trace_file(tracefile, 0, program[index].data);
            trace_file(tracefile, 1, program[index].data);
            regs[PC] += 2;
 
                return (int) program[program[index].data].data;
                break;
            }


        default: {
            cout << "invalid destination mode\n";
            return 0x10000;
            break;
        }
    }

    return 0x10000;
}



//compare
int double_operand::compare(uint16_t *regs, CPSR *states, i_cache *program)
{
    uint16_t index = 0;
    int source_op = make_source(regs, program);
    int sign_source = (uint16_t)source_op >> 15;
    int condition = 0;
    uint16_t result;

    if(0x10000 < source_op)
        return source_op;

    //perform action for different register modes
    switch(destination_mode)
    {
        case 0:
            {   //register
                result = ~(uint16_t)source_op + regs[destination] + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (regs[destination] >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)regs[destination] < (int16_t)source_op)
                    condition |= CARRY;

                states->set_condition(condition);
 
                return (int) regs[destination];
                break;
            }

        case 1:
            {   //register deferred
                if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }

                result = ~(uint16_t)source_op + program[regs[destination]].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[regs[destination]].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)program[regs[destination]].data < (int16_t)source_op)
                    condition |= CARRY;

                states->set_condition(condition);

                trace_file(tracefile, 0, regs[destination]);
 
                return (int) program[regs[destination]].data;
                break;
            }


        case 2:
            {   //autoincrement:
                 if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }

                result = ~(uint16_t)source_op + program[regs[destination]].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[regs[destination]].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)program[regs[destination]].data < (int16_t)source_op)
                    condition |= CARRY;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[destination]);
                regs[destination] += 2;
 
                return (int) program[regs[destination]].data;
                break;
            }


       case 3: {   //autoincrement deferred
                if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
 
                result = ~(uint16_t)source_op + program[program[regs[destination]].data].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[program[regs[destination]].data].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)program[regs[destination]].data < (int16_t)source_op)
                    condition |= CARRY;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 0, program[regs[destination]].data);
                regs[destination] += 2;
 
                return (int) program[program[regs[destination]].data].data;
                break;
            }


        case 4: {   //autodecrement

                regs[destination] -= 2;
                  if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }

                result = ~(uint16_t)source_op + program[regs[destination]].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[regs[destination]].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)program[regs[destination]].data < (int16_t)source_op)
                    condition |= CARRY;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[destination]);
 
                return (int) program[regs[destination]].data;
                break;
            }


        case 5: {   //autodecrement deferred
                regs[destination] -= 2;
                 if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
 
                result = ~(uint16_t)source_op + program[program[regs[destination]].data].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[program[regs[destination]].data].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)program[regs[destination]].data < (int16_t)source_op)
                    condition |= CARRY;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 0, program[regs[destination]].data);
 
                return (int) program[program[regs[destination]].data].data;
                break;
            }



        case 6: {   //index
            if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2)) {
                cout << "boundary error\n";
                return 0x10000;
            }

                result = ~(uint16_t)source_op + program[index].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[regs[destination]].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;

                if((int16_t)program[index].data <(int16_t)source_op)
                    condition |= CARRY;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[PC]);
                trace_file(tracefile, 0, index);
                regs[PC] += 2;
 
                return (int) program[index].data;
                break;
            }


        case 7: {   //register deferred
             if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2) || (program[index].data % 2)) 
            {
                cout << "boundary error\n";
                return 0x10000;
            }

                result = ~(uint16_t)source_op + program[program[index].data].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[program[index].data].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;

                if((int16_t)program[program[index].data].data < (int16_t)source_op)
                    condition |= CARRY;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[PC]);
                trace_file(tracefile, 0, index);
                trace_file(tracefile, 0, program[index].data);
                regs[PC] += 2;
 
                return (int) program[program[index].data].data;
                break;
            }


        default: {
            cout << "invalid destination mode\n";
            return 0x10000;
            break;
        }
    }

    return 0x10000;
}



//move function
int double_operand::move(uint16_t *regs, CPSR *states, i_cache *program)
{
    uint16_t index = 0;
    int source_op = make_source(regs, program);
    if(0xFFFF < source_op)
        return source_op;

    int condition = 0;

    switch(destination_mode)
    {
        case 0:
            {   //register
                regs[destination] = source_op;

                condition = states->get_condition() & CARRY;
                if(!regs[destination])
                {
                    condition |= ZERO;
                }
                if(((int) regs[destination]) < 0)
                    condition |= NEGATIVE;
                condition &= ~V_OVERFLOW;
                states->set_condition(condition);
 
                return (int) regs[destination];
                break;
            }

        case 1:
            {   //register deferred
                if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                program[regs[destination]].data = source_op;
                program[regs[destination] + 1].data = source_op;
 
                condition = states->get_condition() & CARRY;
                if(!program[regs[destination]].data)
                {
                    condition |= ZERO;
                }
                if(((int16_t) program[regs[destination]].data) < 0)
                    condition |= NEGATIVE;
                condition &= ~V_OVERFLOW;
                states->set_condition(condition);

                program[regs[destination] + 1].data = source_op;
                trace_file(tracefile, 1, regs[destination]);
 
                return (int)program[regs[destination]].data;
                break;
            }


        case 2:
            {   //autoincrement
                if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                program[regs[destination]].data = source_op;
                program[regs[destination] + 1].data = source_op;
  
                condition = states->get_condition() & CARRY;
                if(!program[regs[destination]].data)
                {
                    condition |= ZERO;
                }
                if(((int16_t) program[regs[destination]].data) < 0)
                    condition |= NEGATIVE;
                condition &= ~V_OVERFLOW;
                states->set_condition(condition);

                program[regs[destination] + 1].data = source_op;
                trace_file(tracefile, 1, regs[destination]);
                regs[destination] += 2;
 
                return (int)program[regs[destination]].data;
                break;
       }


       case 3: {   //autoincrement deferred
                if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                program[program[regs[destination]].data].data = source_op;
                program[program[regs[destination]].data + 1].data = source_op; 

                condition = states->get_condition() & CARRY;
                if(!program[program[regs[destination]].data].data)
                {
                    condition |= ZERO;
                }
                if(((int16_t) program[program[regs[destination]].data].data) < 0)
                    condition |= NEGATIVE;
                condition &= ~V_OVERFLOW;
                states->set_condition(condition);

 
                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 1, program[regs[destination]].data);
                regs[destination] += 2;
 
                return (int)program[program[regs[destination]].data].data;
                break;
        }

        case 4: {   //autodecrement
                if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                regs[destination] -= 2;
                program[regs[destination]].data = source_op;
                program[regs[destination] + 1].data = source_op; 

                condition = states->get_condition() & CARRY;
                if(!program[regs[destination]].data)
                {
                    condition |= ZERO;
                }
                if(((int16_t) program[regs[destination]].data) < 0)
                    condition |= NEGATIVE;
                condition &= ~V_OVERFLOW;
                states->set_condition(condition);
 
                trace_file(tracefile, 1, regs[destination]);
                return (int)program[regs[destination]].data;
                break;
        }
        case 5: {   //autodecrement deferred
                if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return 0x10000;
                }
                regs[destination] -= 2;
                program[program[regs[destination]].data].data = source_op;
                program[program[regs[destination]].data + 1].data = source_op;

                condition = states->get_condition() & CARRY;
                if(!program[program[regs[destination]].data].data)
                {
                    condition |= ZERO;
                }
                if(((int16_t) program[program[regs[destination]].data].data) < 0)
                    condition |= NEGATIVE;
                condition &= ~V_OVERFLOW;
                states->set_condition(condition);

                trace_file(tracefile, 0, regs[destination]);
                trace_file(tracefile, 1, program[regs[destination]].data);
                return (int)program[program[regs[destination]].data].data;
                break;
        }

        case 6: {   //register deferred
                        //memory read for BIT and CMP
                        //memory write for all others
            if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2)) {
                cout << "boundary error\n";
                return 0x10000;
            }

            program[index].data = source_op;
            program[index + 1].data = source_op;
 
            condition = states->get_condition() & CARRY;
            if(!program[index].data)
            {
                condition |= ZERO;
            }
            if(((int16_t) program[index].data) < 0)
                condition |= NEGATIVE;
            condition &= ~V_OVERFLOW;
            states->set_condition(condition);

            trace_file(tracefile, 0, regs[PC]);
            trace_file(tracefile, 1, index);
 
            regs[PC] += 2;
            
            return program[index].data;
            break;
        }

        case 7: {   //register deferred
                        //memory read for BIT and CMP
                        //memory write for all others
            if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2) || (program[index].data % 2)) {
                cout << "boundary error\n";
                return 0x10000;
            }

            program[program[index].data].data = source_op;
            program[program[index].data + 1].data = source_op;
  
            condition = states->get_condition() & CARRY;
            if(!program[program[index].data].data)
            {
condition |= ZERO;
            }
            if(((int16_t) program[program[index].data].data) < 0)
                condition |= NEGATIVE;
            condition &= ~V_OVERFLOW;
            states->set_condition(condition);

            trace_file(tracefile, 0, regs[PC]);
            trace_file(tracefile, 0, index);
            trace_file(tracefile, 1, program[index].data);
            
            regs[PC] += 2;
 
            return (int) program[program[index].data].data;
            break;
        }
        default: {
            cout << "invalid destination mode\n";
            return 0x10000;
            break;
        }
    }

    return 0x10000;
}


//decode source mode
int double_operand::make_source(uint16_t *regs, i_cache *program)
{
    uint16_t index = 0;

    switch(source_mode)
    {
        case 0:
            {   //register
                return (int) regs[source];
                break;
            }

        case 1:
            {   //register deferred
                trace_file(tracefile, 0, regs[source]);
                return (int)program[regs[source]].data;
                break;
            }


        case 2:
            {   //autoincrement
                trace_file(tracefile, 0, regs[source]);
                regs[source] += 2;
                return (int)program[regs[source] - 2].data;
                break;
            }


        case 3: {   //autoincrement deferred
            trace_file(tracefile, 0, regs[source]);
            trace_file(tracefile, 0, program[regs[source]].data);
            regs[source] += 2;
            return (int) program[program[regs[source] - 2].data].data;
            break;
        }


        case 4: {   //autodecrement
            regs[source] -= 2;
            trace_file(tracefile, 0, regs[source]);
            return (int) program[regs[source]].data;
            break;
        }
        case 5: {   //autodecrement deferred
            regs[source] -= 2;
            trace_file(tracefile, 0, regs[source]);
            trace_file(tracefile, 0, program[regs[source]].data);
            return (int) program[program[regs[source]].data].data;
            break;
        }

        case 6: {   //index
            if (source == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[source] + program[regs[PC]].data;
            if ((regs[source] % 2) || (index % 2)) {
                cout << "boundary error\n";
                return 0x10000;
            }

            trace_file(tracefile, 0, regs[PC]);
            trace_file(tracefile, 0, index);
            regs[PC] += 2;
            return (int) program[index].data;
            break;
        }

        case 7: {   //index deferred
            if (source == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[source] + program[regs[PC]].data;
            if ((regs[source] % 2) || (index % 2) || (program[index].data % 2)) {
                cout << "boundary error\n";
                return 0x10000;
            }
            trace_file(tracefile, 0, regs[PC]);
            trace_file(tracefile, 0, index);
            trace_file(tracefile, 0, program[index].data);
            regs[PC] += 2;

            return (int) program[program[index].data].data;
            break;
        }
        default: {
            cout << "invalid source mode\n";
            return 0x10000;
            break;
        }
    }

    return 0x10000;
}




int double_operand::instruction(uint16_t *regs, CPSR *states, i_cache *program)
{
    int result = -1;

    switch(function_code)
    {
        case MOV:
            {
                result = move(regs, states, program);
                break;
            }
        case MOVB:
            {
                cout << "MOVB" << endl;
                break;
            }
        case CMP: 
            {
                result = compare(regs, states, program);
                break;
            }
        case CMPB:
            {
                cout << "CMPB" << endl;
                break;
            }
        case ADD:
            {
                result = add(regs, states, program);
                break;
            }
        case SUB:
            {
                result = sub(regs, states, program);
                break;
            }
        case BIT:
            {
                result = bit(regs, states, program);
                break;
            }
        case BITB:
            {
                cout << "BITB" << endl;
                break;
            }
        case BIC:
            {
                result = bic(regs, states, program);
                break;
            }
        case BICB:
            {
                cout << "BICB" << endl;
                break;
            }
        case BIS:
            {
                result = bis(regs, states, program);
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
    return result;
}




//function to display information when -v option set
int double_operand::fetch_display(uint16_t *regs, CPSR *states)
{
    int condition;
    int i;

   switch(function_code)
    {
        case MOV:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "MOV" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout  << oct << "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case CMP:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "CMP" << endl;

                cout << "flags: \t\t" << "N\t" << "Z\t" << "V\t" << "C" << endl;
                cout << "\t\t" << ((condition >> 3) & 1) << "\t";
                cout << ((condition >> 2) & 1) << "\t" << ((condition >> 1) & 1);
                cout << "\t" << (condition & 1) << endl;
                for(i = 0; i < 8; ++i)
                {
                    cout << oct <<  "register " << i << ": " << regs[i] << "\t";
                }
                cout << endl;
                break;
            }
        case ADD:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "ADD " << endl;

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
        case SUB:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "SUB" << endl;

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
        case BIT:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BIT" << endl;

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

        case BIC:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BIC" << endl;
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
        case BIS:
            {
                condition = states->get_condition();
                cout << "Current instruction: ";  
                cout << "BIS" << endl;

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



double_operand::~double_operand()
{
}
