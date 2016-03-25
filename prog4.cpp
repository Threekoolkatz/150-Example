/*************************************************************************//**
 * @file 
 *
 * @mainpage Program 4 - Harp Stuff
 * 
 * @section course_section Course Information 
 *
 * @author Mackenzie Smith
 * 
 * @date December 7th
 * 
 * @par Professor: 
 *         Roger Schrader
 * 
 * @par Course: 
 *         CSC 150 - section 4 -  3:00pm mon, tues, wed
 * 
 * @par Location: 
 *         McLaury - 310
 *
 * @section program_section Program Information 
 * 
 * @details 
 I'm not quite sure what exactly it does, but it takes input from a file ( a series of numbers that determine: 1. when the note is to be played
 2. The actual note itself and 3. the volume to play the note at.  It stores these to an array i think and then calculates the length of a ring buffer
 for that specific note and outputs to a file the 441 samples per second of that note as its plucked.  Theoretically this temporary file is then fed
 to a program to convert it to a .wav file which can be played in windows media player.
 *
 * @section compile_section Compiling and Usage 
 *
 * @par Compiling Instructions: 
 *      /stack:10000000 and linked in the library wave.lib
		I dont know if its just on my Visual Studios, but you need to clean the solution and then rebuild it for it to work correctly
 * 
 * @par Usage: 
   @verbatim  
   c:\> prog4.exe midinotefile [tempo]
   d:\> c:\bin\prog4.exe midinotefile [tempo]
   [tempo] is optional, default is 1.0
   @endverbatim 
 *
 * @section todo_bugs_modification_section Todo, Bugs, and Modifications
 * 
 * @bug <List anything that is not working in your program>
 * @bug I couldnt figure out how to name the rename the output wave file, so i just hardcoded it as c5.wav
 * @bug When ran from the command prompt it says "convert_to_wave: Input file "c5.tmp" is empty or does not contain only numbers"  not really sure how to fix that
 * 
 * 
 * @par Modifications and Development Timeline: 
   @verbatim 
   Date          Modification 
   ------------  -------------------------------------------------------------- 
   Nov  29th, 2012  Started the program
   Dec  3rd,  2012  No apparent progress
   Dec  5th,  2012  Got the karplus algorithm working 
   Dec  6th,  2012  The get_note and the pluck string function 
   Dec  7th,  2012  Got all the functions working together

                
   @endverbatim
 *
 *****************************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>


#include "wave.h"

using namespace std;

/**
 * @brief The number of samples per second
 **/
const int SAMPLE_RATE=44100;
/**
 * @brief The maximum number of notes 
 **/
const int NUM_NOTES=120; 
/**
 * @brief The number of samples needed per Millisecond
 **/
const int SMPLS_PER_MS=(SAMPLE_RATE/100);
/**
 * @brief The maximum number of seconds in a note file
 **/
const int MAX_SECONDS=600;
/**
 * @brief The largest ring buffer that will be needed.
 **/
const int BASE_SIZE=SAMPLE_RATE/16;

//-----------PROTOTYPES-------------

double get_note(double notes[],int &position, int array_size);
void pluck_string(double notes[],int array_size, float vol);



/**************************************************************************//** 
 * @author Mackenzie Smith
 * 
 * @par Description: 
 * A detailed description of the function.  It can
 * extend to multiple lines
 * 
 * @param[in]      argc - The number of command line arguments
 * @param[in]      argv - a 2d array of characters.  Each row represents a
 *                        token that was supplied at the command prompt.
 * 
 * @returns 0 program ran successful.
 * @returns 1 The midi note file was not supplied.
 * @return  2 An invalid tempo was given.  Must be between .25 and 4.0
 * 
 *****************************************************************************/
int main ( int argc, char **argv )
{
    double notes[NUM_NOTES][BASE_SIZE] = {0};
    int position[NUM_NOTES] = {0};
	double filenotes [2][3] = {0};
	char filename[100];
	int array_size[NUM_NOTES];
	float tempo;
	int i = 0;
	int j = 0;
	int curtt; 
	double exp;
	double next_time;
	int next_note;
	double sum;
	float vol;
	double value;

	ifstream fin;
	fin.open(argv[1]);
	if( !fin )
		cout << "Error, file did not open" << endl;
	ofstream fout;
	fout.open("c5.tmp");
	if( !fout )
		cout << "Error, file did not open" << endl;

	if ( argc < 2 || argc > 3)
	{
		cout << "Must include program name, file name, and optional tempo." << endl;
		cout << "Examples below." << endl;
		cout << "prog4 scale.notes 1" << endl;
		cout << "prog4 scale.notes" << endl;

		return 1;
	}

	if ( argc == 2 )
	{

		strcpy( filename, argv[1] ); 
		tempo = 1.0;
	}

	if ( argc == 3 )
	{

		strcpy( filename, argv[1] ); 
		tempo = atof( argv[2] ); 
		tempo = tempo * 1.0;

		if ( tempo < .25 || tempo > 4.0 )
		{
			cout << "Tempo must be between .25 and 4.0 " << endl;
			return 2;
		}

	}

	tempo = 1/tempo;
	

	

	
	for (i = 0; i < NUM_NOTES; i++)
	{
			exp = (i - 69.0)/12.0;
			value = pow(2.0,exp);
			value = value * 440;
			array_size[i] = SAMPLE_RATE / value;
			//array_size[i]= SAMPLE_RATE / ((pow(2, exp)) * 440); 
	}
	curtt = 0;

	while(fin>>next_time>>next_note>>vol)
	{

		while( curtt < next_time * tempo)
		{


			for(i = 0; i < 441; i++)
			{
				sum = 0;
				for(j = 0; j < NUM_NOTES; j++)	
					sum += get_note(notes[j], position[j], array_size[j]);
				fout<<sum<<" ";
			}

			curtt++;
		}
		cout << "." << endl;
		if(next_note != -1)
			pluck_string(notes[next_note], array_size[next_note], vol);
	}
	
	fin.close();
	fout.close();

	convert_to_wave("c5.tmp", "c5.wav",44100, true);
	


    return 0;
}

/**************************************************************************//** 
 * @author Mackenzie Smith
 * 
 * @par Description: 
 * A detailed description of the function.  It can
 * extend to multiple lines
 * 
 * @param[in]      notes[] - The array for the ring buffer
 * @param[in]      position - position in time
   @param[in]	   array_size - size of the array
 *	

 * 
 *****************************************************************************/
double get_note(double notes[],int &position, int array_size)
{
	double sum=0;
	int next_position = (position+1)%array_size;
	
	sum = ((notes[position]+notes[next_position])/2.0) * .996;

	notes[position] = sum;
	position = next_position;

	return sum;
}

/**************************************************************************//** 
 * @author Mackenzie Smith
 * 
 * @par Description: 
 * A detailed description of the function.  It can
 * extend to multiple lines
 * 
 * @param[in]      notes - the array that stores some numbers
 * @param[in]      array_size - size of the array
 * @param[in]	   vol - the volume of the note to be played
 * 
 * 
 *****************************************************************************/

void pluck_string(double notes[],int array_size, float vol)
{
	float num = rand ();
	int i;

	for (i = 0; i < array_size; i ++)
	{
		num = rand();
		num /= 32767.0;
		num -= 0.5;
		num *= vol;
		notes[i] = num;
	}

 
}