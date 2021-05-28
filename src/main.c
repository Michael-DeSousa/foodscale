#define F_CPU 8000000UL

#include <avr/io.h>
#include "io.c"
#include "buzzer.c"
#include "ledMatrix.c"
#include "timer.h"
#include "loadCell.c"

typedef struct _task
{    
    signed 	 char state; 		//Task's current state
    unsigned long period; 		//Task period
    unsigned long elapsedTime; 	//Time elapsed since last task tick
    int (*TickFct)(int); 		//Task tick function
} task;

//Global Variables
//Button Inputs
unsigned char tempA = 0x00;

//Power for the whole system. We don't really shut down components, we just block their output
enum Power_States { Off, On };
unsigned char power = Off;

//Char Array that holds weight for LCD 
char weightBuffer[10];

//Alarm for Matrix/Buzzer
long currLoad = 0;

//Raw Weight value from scale
long weight = 0;

//Tare weight offset
long offset = 0;

//Measurement options for scale
enum Unit_Types {Grams, Ounces, Pounds};
unsigned char unitFactor = Grams;

//Global Variables END


enum PowerButton_States { PowerButton_Init, PowerButton_Idle, PowerButton_Release, PowerButton_Press } PowerButton_State;

int PowerButtonTick(int PowerButton_State)
{
    //State Transitions
    switch(PowerButton_State)
    {
        case PowerButton_Init:
        power = Off;
        PowerButton_State = PowerButton_Idle;
        break;
        
        case PowerButton_Idle:
        if (tempA == 0x01)
        {
            PowerButton_State = PowerButton_Press;
        }
        break;
        
        case PowerButton_Release:
        PowerButton_State = PowerButton_Idle;
        break;
        
        case PowerButton_Press:
        if (tempA == 0x00)
        {
            PowerButton_State = PowerButton_Release;
        }
        break;
    }
    
    //State Actions
    switch(PowerButton_State)
    {
        case PowerButton_Init:
        break;
        
        case PowerButton_Idle:
        break;
        
        case PowerButton_Release:
        power = !power;
        break;
        
        case PowerButton_Press:
        break;
    }
    
    return PowerButton_State;
}

enum TareButton_States { TareButton_Init, TareButton_Idle, TareButton_Release, TareButton_Press } TareButton_State;

int TareButtonTick(int TareButton_State)
{
    //State Transitions
    switch(TareButton_State)
    {
        case TareButton_Init:
        offset = 0;
        if(power == On)
        {
            TareButton_State = TareButton_Idle;
        }            
        break;
        
        case TareButton_Idle:
        if (power == Off)
        {
            TareButton_State = TareButton_Init;
        }
        else if (tempA == 0x02 && power == On)
        {
            TareButton_State = TareButton_Press;
        }
        break;
        
        case TareButton_Release:
        TareButton_State = TareButton_Idle;
        break;
        
        case TareButton_Press:
        if (tempA == 0x00)
        {
            TareButton_State = TareButton_Release;
        }
        break;
    }
    
    //State Actions
    switch(TareButton_State)
    {
        case TareButton_Init:
        break;
        
        case TareButton_Idle:
        break;
        
        case TareButton_Release:
        offset = weight;
        break;
        
        case TareButton_Press:
        break;
    }
    
    return TareButton_State;
}

enum UnitButton_States { UnitButton_Init, UnitButton_Idle, UnitButton_Release, UnitButton_Press } UnitButton_State;

int UnitButtonTick(int UnitButton_State)
{
    //State Transitions
    switch(UnitButton_State)
    {
        case UnitButton_Init:
        unitFactor = Grams;
        if (power == On)
        {
        UnitButton_State = UnitButton_Idle;
        }        
        break;
        
        case UnitButton_Idle:
        if (power == Off)
        {
            UnitButton_State = UnitButton_Init;
        }
        if (tempA == 0x04 && power == On)
        {
            UnitButton_State = UnitButton_Press;
        }                
        break;
        
        case UnitButton_Release:
        UnitButton_State = UnitButton_Idle;
        break;
        
        case UnitButton_Press:
        if (tempA == 0x00)
        {
            UnitButton_State = UnitButton_Release;
        }
        break;
    }
    
    //State Actions
    switch(UnitButton_State)
    {
        case UnitButton_Init:
        break;
        
        case UnitButton_Idle:
        break;
        
        case UnitButton_Release:
        switch(unitFactor)
        {
            case Grams:
            unitFactor = Ounces;
            break;
            
            case Ounces:
            unitFactor = Pounds;
            break;
            
            case Pounds:
            unitFactor = Grams;
            break;
        }
        break;
        
        case UnitButton_Press:
        break;
    }
    
    return UnitButton_State;
}

enum Buzzer_States { Buzzer_Init, Buzzer_Idle, Buzzer_Buzz } Buzzer_State;

int BuzzerTick(int Buzzer_State)
{
    //State Transitions
    switch(Buzzer_State) 
    {   
        case Buzzer_Init:
        PWM_on();
        Buzzer_State = Buzzer_Idle;
        break;
        
        case Buzzer_Idle:                    
        if(currLoad > 80.0 && power == On)
        {
            Buzzer_State = Buzzer_Buzz;
        }
        break;
        
        case Buzzer_Buzz:
        Buzzer_State = Buzzer_Idle;
        break;
    }
    
    //State Actions
    switch(Buzzer_State)
    {        
        case Buzzer_Init:
        break;
        
        case Buzzer_Idle:
        set_PWM(0);
        break;
        
        case Buzzer_Buzz:
        set_PWM(1054.94);
        break;        
    }
    
    return Buzzer_State;
}

enum Matrix_States { Matrix_Init, Matrix_Idle, Matrix_On } Matrix_State;
int MatrixTick(int Matrix_State)
{
    //State Transitions
    switch(Matrix_State)
    {
        case Matrix_Init:
        SPIsetup();
        initializeMatrix();
        clear();
        Matrix_State = Matrix_Idle;
        break;
        
        case Matrix_Idle:
        clear();
        if(power == On)
        {
            Matrix_State = Matrix_On;
        }
        break;        
        
        case Matrix_On:
        if(power == Off)
        {
            Matrix_State = Matrix_Idle;
        }
        else
        {
            Matrix_State = Matrix_On;
        }
        break;       
    }
    
    //State Actions
    switch(Matrix_State)
    {
        case Matrix_Init:
        break;
        
        case Matrix_Idle:
        break;
        
        case Matrix_On:
        if (currLoad >= 0 && currLoad < 12)
        {
            display(0b00000000);
        }
        else if (currLoad >= 12 && currLoad < 25)
        {
            display(0b00000001);
        }
        else if (currLoad >= 25 && currLoad < 37)
        {
            display(0b00000011);
        }
        else if (currLoad >= 37 && currLoad < 50)
        {
            display(0b00000111);
        }
        else if (currLoad >= 50 && currLoad < 62)
        {
            display(0b00001111);
        }
        else if (currLoad >= 62 && currLoad < 75)
        {
            display(0b00011111);
        }
        else if (currLoad >= 75 && currLoad < 87)
        {
            display(0b00111111);
        }
        else if (currLoad >= 87 && currLoad < 100)
        {
            display(0b01111111);
        }
        else if (currLoad >= 100)
        {
            display(0b11111111);
        }
        break;
    }
    
    return Matrix_State;
}

enum LCD_States { LCD_Start, LCD_Idle, LCD_On } LCD_State;
int LCD_Tick(int LCD_State)
{
    //State Transitions
    switch(LCD_State)
    {
        case LCD_Start:
        LCD_init();
        LCD_State = LCD_Idle;
        break;
        
        case LCD_Idle:
        if(power == On)
        {
            LCD_State = LCD_On;
        }
        break;
        
        case LCD_On:
        if(power == Off)
        {
            LCD_State = LCD_Idle;
        }
        break;
    }
    
    //State Actions
    switch(LCD_State)
    {
        case LCD_Start:
        break;
        
        case LCD_Idle:
        LCD_ClearScreen();
        break;
        
        case LCD_On:
        switch(unitFactor)
        {
            case Grams:
            LCD_ClearScreen();
            LCD_DisplayString(1, "GM: ");
            LCD_DisplayString(5, weightBuffer);
            break;
            
            case Ounces:
            LCD_ClearScreen();
            LCD_DisplayString(1, "OZ: ");
            LCD_DisplayString(5, weightBuffer);
            break;
            
            case Pounds:
            LCD_ClearScreen();
            LCD_DisplayString(1, "LB: ");
            LCD_DisplayString(5, weightBuffer);
            break;
        }
        break;
    }
    
    return LCD_State;
}

enum Scale_States { Scale_Init, Scale_Idle, Scale_On} Scale_State;

int ScaleTick(int Scale_State)
{
    long convertedWeight = 0;   
    switch(Scale_State)
    {
        case Scale_Init:
        initialize_scale();
        Scale_State = Scale_Idle;          
        break;
        
        case Scale_Idle:
        if (power == On)
        {
            Scale_State = Scale_On;
        }
        break;
        
        case Scale_On:
        if (power == Off)
        {
            Scale_State = Scale_Idle;
        }
        break;
        
        default:
        break;
    }
    
    switch(Scale_State)
    {
        case Scale_Init:
        break;
        
        case Scale_Idle:
        weight = 0;
        currLoad = 0;
        break;
        
        case Scale_On:
        currLoad = 0;
        weight = getAverage(10);       
        switch(unitFactor)
        {
            case Grams:
            convertedWeight = weight/407;
            currLoad = (convertedWeight/3628.0) * 100.0;
            convertedWeight -= (offset/407);
            break;
            
            case Ounces:
            convertedWeight = weight/11538;
            currLoad = (convertedWeight/128.0) * 100.0;
            convertedWeight -= (offset/11538);
            break;
            
            case Pounds:
            convertedWeight = weight/184778;
            currLoad = (convertedWeight/8.0) * 100.0;
            convertedWeight -= (offset/184778);
            break; 
            
            default:
            break;           
        }
        ltoa(convertedWeight, weightBuffer, 10);        
        break;
        
        default:
        break;
    }
    
    return Scale_State;
}

int main(void)
{
    DDRA = 0xF0; PORTA = 0x0F; //A0 - A3 are reserved for input buttons, A4 - A7 are reserved for LCD control lines
    DDRB = 0xFF; //Port B is for output only
    DDRD = 0xFF; PORTD = 0x00; //Port D is reserved LCD Data Lines
    DDRC = 0xFF; PORTC = 0x00; //Port C is for scale readings
        
    static task task1, task2, task3, task4, task5, task6, task7;
    task *tasks[] = { &task1, &task2, &task3, &task4, &task5, &task6, &task7 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    TimerOn();
    TimerSet(10);
    
    unsigned long PowerButtonTick_period = 10;
    unsigned long TareButtonTick_period = 10;
    unsigned long UnitButtonTick_period = 10;
    unsigned long BuzzerTick_period = 10;
    unsigned long MatrixTick_period = 10;
    unsigned long LCD_Tick_period = 10;
    unsigned long ScaleTick_period = 200;
    
    task1.state = Buzzer_Init;
    task1.period = BuzzerTick_period;
    task1.elapsedTime = BuzzerTick_period;
    task1.TickFct = &BuzzerTick;
    
    task2.state = Matrix_Init;
    task2.period = MatrixTick_period;
    task2.elapsedTime = MatrixTick_period;
    task2.TickFct = &MatrixTick;
    
    task3.state = PowerButton_Init;
    task3.period = PowerButtonTick_period;
    task3.elapsedTime = PowerButtonTick_period;
    task3.TickFct = &PowerButtonTick;
    
    task4.state = TareButton_Init;
    task4.period = TareButtonTick_period;
    task4.elapsedTime = TareButtonTick_period;
    task4.TickFct = &TareButtonTick;
    
    task5.state = UnitButton_Init;
    task5.period = UnitButtonTick_period;
    task5.elapsedTime = UnitButtonTick_period;
    task5.TickFct = &UnitButtonTick;
    
    task6.state = LCD_Start;
    task6.period = LCD_Tick_period;
    task6.elapsedTime = LCD_Tick_period;
    task6.TickFct = &LCD_Tick;
    
    task7.state = Scale_Init;
    task7.period = ScaleTick_period;
    task7.elapsedTime = ScaleTick_period;
    task7.TickFct = &ScaleTick;
            
    while (1) 
    {
        tempA = ~PINA & 0x07;
                  
        // Scheduler code
        for ( int i = 0; i < numTasks; i++ )
        {
            // Task is ready to tick
            if ( tasks[i]->elapsedTime == tasks[i]->period )
            {
                // Setting next state for task
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                // Reset the elapsed time for next tick.
                tasks[i]->elapsedTime = 0;
            }
            
            tasks[i]->elapsedTime += 1;
        }
        
        while(!TimerFlag)
        {
            //Countdown for next tick
        }
        TimerFlag = 0;
    }

   
    return 0;
}

