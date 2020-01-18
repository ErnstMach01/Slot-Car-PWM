/*
*
* Name: Ernst Mach
* Project: Pulse Width Modulation
* Date: February 28, 2018
* Description: Basic program dealing with PWM that is a simulation of a car. Has 1 button for user control, allowing the user to increase the rpm of the engine to make the light more brighter when the button is pressed. When the button is relased, the rpm will slowly decrease along with the brightness of the light. Also includes heat and fuel systems. Fuel will be slowly consumed over time and when fuel is completly depleated, the car will no longer run and the program will exit. When the engine is at certain rpm levels, heat will be generated. The amount of heat generated will be based on the amount of rpms. Heat will slowly decrease due to coolent system in the car. However if the car reaches critical heat levels, the car will overheat and exits the program.
*
*/

#include <unistd.h>
#include <stdio.h>	
#include </usr/include/sys/io.h> //IO library
#define BASEPORT 0x378 /* parallel or lp1 port address */

float Hertz = 60;		//Starting hertz value
int ImportValue = 0;	//Variable to store what input values are coming from the import bases
float OnTimePercentage = 0.20;  //Percentage of the cycle that the light will be on
float OffTimePercentage = 0.80; //Percentage of the cycle that the light will be off
float OnSpeed = 0;  //How long the light will be on
float OffSpeed = 0; //How long the light will be off
float Cycle = 0;  //Length of one cycle 

int Heat = 0; //The amount of heat in the car
int Fuel = 100000;  //The amount of fuel in the car

//Function used to calculate the duty cycle of a given hertz value
float DutyCycle(float HertzValue) {
float dutyCycle;
dutyCycle = (1000000/HertzValue);
return dutyCycle;
}

//Function used to calculate either on or off time based on length of cycle and percentage of on time or off time
float OnOffTime(float CycleTime, float OnOffTime) {
float DisplayTime;
DisplayTime = CycleTime*OnOffTime;
return DisplayTime;
}

int main()
{
	ioperm(BASEPORT, 3, 1); // Get permission to access the ports
		
	while(1)
	{
    system("clear");  //Clear the screen and update the text on the screen
    printf("Fuel: %i    Heat: %i\n", Fuel, Heat);
		ImportValue = inb(BASEPORT+1);	//Constantly check for input from the baseport
		Cycle = DutyCycle(Hertz); //Find the duty cycle of the given hertz value
		OnSpeed = OnOffTime(Cycle, OnTimePercentage); //Find the time length that the light wil be on
		OffSpeed = OnOffTime(Cycle, OffTimePercentage); //Find the time length that the light will be off

		if( ImportValue == 0x58 && OffTimePercentage > 0.01)  //Check if the pedal is pressed and if the off time percentage does not exceed 1%
		{
		OnTimePercentage += 0.01; //Increase on time percentage by 1%
		OffTimePercentage -= 0.01;  //Decrease off time percentage by 1%
		}

		if(ImportValue == 0x78 && OnTimePercentage > 0.01) {  //Check if the pedal is released and if the on time percentage does not exceed 1%
		OffTimePercentage += 0.01;  //Increase off time percentage by 1%
		OnTimePercentage -= 0.01; //Decrease on time percentage by 1%
		}	
		
		if(OnTimePercentage > 0.25 && OnTimePercentage <= 0.50) { //Checks if the car's 'rpm' is low
		  Heat++; //Increase the heat
		} else if(OnTimePercentage > 0.50 && OnTimePercentage <= 0.75) {  //Checks if the car's 'rpm' is normal
		  Heat += 2;  //Increase the heat by 2
		} else if(OnTimePercentage > 0.75) {  //Checks if the car's 'rpm' is high
		  Heat += 4;  //Increase the heat by 4
		}

    if(Heat == 100000) {  //Checks if the heat has reaced the max amount
      printf("Car has overheated!");  //Prints message to user
      return 0; //Exits program
    }
    
    if(Fuel == 0) { //Checks if the car runs out of fuel
      printf("Out of fuel!"); //Prints message to user
      return 0; //Exits program
    }
    
		outb(0x1, BASEPORT); // Sends a signal to turn on the first light
		usleep(OnSpeed); // Wait a given amount of time based on the on time calculation
		outb(0x0, BASEPORT); // Sends a signal to turn off the light
		usleep(OffSpeed); // Wait a given amount of time  based on the off time calculation
		Fuel--; //Decrase fuel constantly
		Heat--; //Decrease heat constantly
	}

	ioperm(BASEPORT, 3, 0); // We don't need the ports anymore
	// release ports for other programs however as loop is infinite we will never get here

	return 0;
}