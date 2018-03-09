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

//compare
int double_operand::compare(uint16_t *regs, CPSR *states, i_cache *program)
{
    int index = 0;
    int source_op = make_source(regs, program);
    int sign_source = (uint16_t)source_op >> 15;
    int condition = 0;
    uint16_t result;

    //perform action for different register modes
    switch(destination_mode)
    {
        case 0:
            {   //register
                result = (uint16_t)source_op + ~regs[destination] + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (regs[destination] >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)source_op < (int16_t)regs[destination])
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
                    return -1;
                }

                result = (uint16_t)source_op + ~program[regs[destination]].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[regs[destination]].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)source_op < (int16_t)program[regs[destination]].data)
                    condition |= CARRY;

                states->set_condition(condition);

                return (int) regs[destination];
                break;
            }


        case 2:
            {   //autoincrement:
                 if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return -1;
                }

                result = (uint16_t)source_op + ~program[regs[destination]].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[regs[destination]].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)source_op < (int16_t)program[regs[destination]].data)
                    condition |= CARRY;

                states->set_condition(condition);
                regs[destination] += 2;

                return (int) program[regs[destination]].data;
                break;
            }


       case 3: {   //autoincrement deferred
                if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return -1;
                }
 
                result = (uint16_t)source_op + ~program[program[regs[destination]].data].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[program[regs[destination]].data].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)source_op < (int16_t)program[regs[destination]].data)
                    condition |= CARRY;

                states->set_condition(condition);
                regs[destination] += 2;

                return (int) program[program[regs[destination]].data].data;
                break;
            }


        case 4: {   //autodecrement

                regs[destination] -= 2;
                  if(regs[destination] % 2)
                {
                    cout << "boundary error\n";
                    return -1;
                }

                result = (uint16_t)source_op + ~program[regs[destination]].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[regs[destination]].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)source_op < (int16_t)program[regs[destination]].data)
                    condition |= CARRY;

                states->set_condition(condition);

                return (int) program[regs[destination]].data;
                break;
            }


        case 5: {   //autodecrement deferred
                regs[destination] -= 2;
                 if((regs[destination] % 2) || (program[regs[destination]].data % 2))
                {
                    cout << "boundary error\n";
                    return -1;
                }
 
                result = (uint16_t)source_op + ~program[program[regs[destination]].data].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[program[regs[destination]].data].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;
                if((int16_t)source_op < (int16_t)program[regs[destination]].data)
                    condition |= CARRY;

                states->set_condition(condition);

                return (int) program[program[regs[destination]].data].data;
                break;
            }



        case 6: {   //index
            if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2)) {
                cout << "boundary error\n";
                return -1;
            }

                result = (uint16_t)source_op + ~program[index].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[regs[destination]].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;

                if((int16_t)source_op < (int16_t)program[index].data)
                    condition |= CARRY;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[PC]);
                trace_file(tracefile, 0, (uint16_t) index);
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
                return -1;
            }

                result = (uint16_t)source_op + ~program[program[index].data].data + 1;

                if(!result)
                {
                    condition |= ZERO;
                }
                if((int16_t)result < 0)
                    condition |= NEGATIVE;
                if(!((sign_source) ^ (program[program[index].data].data >> 15)) && (sign_source != (result >> 15)))
                        condition |= V_OVERFLOW;

                if((int16_t)source_op < (int16_t)program[program[index].data].data)
                    condition |= CARRY;

                states->set_condition(condition);
                trace_file(tracefile, 0, regs[PC]);
                trace_file(tracefile, 0, (uint16_t) index);
                trace_file(tracefile, 0, program[index].data);
                regs[PC] += 2;

                return (int) program[program[index].data].data;
                break;
            }


        default: {
            cout << "invalid destination mode\n";
            return -1;
            break;
        }
    }

    return -1;
}



//move function
int double_operand::move(uint16_t *regs, CPSR *states, i_cache *program)
{
    int index = 0;
    int source_op = make_source(regs, program);
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
                    return -1;
                }
                program[regs[destination]].data = source_op;
 
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
                    return -1;
                }
                program[regs[destination]].data = source_op;
  
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
                    return -1;
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
                    return -1;
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
                    return -1;
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
                return -1;
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
            trace_file(tracefile, 1, (uint16_t) index);
            regs[PC] += 2;
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
                return -1;
            }
            index = program[index].data;

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
            trace_file(tracefile, 0, (uint16_t) index);
            trace_file(tracefile, 1, program[index].data);
            regs[PC] += 2;

            return (int) program[program[index].data].data;
            break;
        }
        default: {
            cout << "invalid destination mode\n";
            return -1;
            break;
        }
    }

    return -1;
}


//decode source mode
int double_operand::make_source(uint16_t *regs, i_cache *program)
{
    int index = 0;

    switch(source_mode)
    {
        case 0:
            {   //register
                return (int) source;
                break;
            }

        case 1:
            {   //register deferred
                //memory read for BIT and CMP
                //memory write for all others
                trace_file(tracefile, 0, regs[source]);
                return (int)program[regs[source]].data;
                break;
            }


        case 2:
            {   //register deferred
                //memory read for BIT and CMP
                //memory write for all others
                trace_file(tracefile, 0, regs[source]);
                return (int)program[regs[source]].data;
                break;
            }


        case 3: {   //register deferred
            //memory read for BIT and CMP
            //memory write for all others
            trace_file(tracefile, 0, regs[source]);
            trace_file(tracefile, 0, program[regs[source]].data);
            return (int) program[program[regs[source]].data].data;
            break;
        }


        case 4: {   //register deferred
                    //memory read for BIT and CMP
                    //memory write for all others
            trace_file(tracefile, 0, regs[source]);
            return (int) program[regs[source]].data;
            break;
        }
        case 5: {   //register deferred
                    //memory read for BIT and CMP
                    //memory write for all others
            trace_file(tracefile, 0, regs[source]);
            trace_file(tracefile, 0, program[regs[source]].data);
            return (int) program[program[regs[source]].data].data;
            break;
        }

        case 6: {   //register deferred
                        //memory read for BIT and CMP
                        //memory write for all others
            if (source == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[source] + program[regs[PC]].data;
            if ((regs[source] % 2) || (index % 2)) {
                cout << "boundary error\n";
                return -1;
            }

            trace_file(tracefile, 0, regs[PC]);
            trace_file(tracefile, 0, (uint16_t) index);
            regs[PC] += 2;
            return (int) program[index].data;
            break;
        }

        case 7: {   //register deferred
                        //memory read for BIT and CMP
                        //memory write for all others
            if (source == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[source] + program[regs[PC]].data;
            if ((regs[source] % 2) || (index % 2) || (program[index].data % 2)) {
                cout << "boundary error\n";
                return -1;
            }
            index = program[index].data;
            trace_file(tracefile, 0, regs[PC]);
            trace_file(tracefile, 0, (uint16_t) index);
            trace_file(tracefile, 0, program[index].data);
            regs[PC] += 2;

            return (int) program[index].data;
            break;
        }
        default: {
            cout << "invalid source mode\n";
            return -1;
            break;
        }
    }

    return -1;
}




int double_operand::instruction(uint16_t *regs, CPSR *states, i_cache *program)
{
    int result = -1;
    cout << "function_code: " << function_code << ": ";

    switch(function_code)
    {
        case MOV:
            {
                cout << "MOV" << endl;
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
    return result;
}
