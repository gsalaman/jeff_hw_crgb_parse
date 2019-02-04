/* Let's define a CRGB "string" as starting with a C,  
 *  then having 3 comma separates values... 
 *  where the first value is the R value, second the G, third the B.   
 *  Those values can be 1, 2, or 3 digits.   
 *   
 *  Some examples: 
"C100,0,0" just red 
"C255,255,255" bright white 
"C1,0,1" really dim purple 

Write a function that takes a CRGB string,  
and returns a CRGB number based on the three numbers in that string.   
If the string is badly formed, return an all black CRGB (0,0,0).   
This includes all strings that *DON'T* start with a C...like: 
"G1,1,1" 

if there is stuff AFTER the CRGB string, ignore it.  So: 

"C10,20,30Hello" should still give you a CRGB with 10,20,30.  

Note:  you'll need to include FastLED.h in this one... 

Prototype: 

CRGB convertCRGBString(char *input ) 

*/ 

#include "SoftwareSerial.h" 
#include "FastLED.h" 
CRGB leds; 

typedef enum
{
  LOOK_FOR_COMMAND,
  BUILD_CRGB,
  PROCESS_STRING
} parse_state_type;

parse_state_type parse_state = LOOK_FOR_COMMAND;

#define BUFF_SIZE 12
char myBuffer[BUFF_SIZE];
int buffer_index=0;

void begin_crgb_build( void )
{
       int i;
       
       // This is (potentially) the start of a CRGB string.  Store that 'C' at the 
       // beginning of our buffer.
       myBuffer[0] = 'C';

       // erase all other characters (to prevent old CRGBS from being "insta-detected"
       for (i = 1; i < BUFF_SIZE; i++)
       {
         myBuffer[i] = ' ';
       }

       // get ready to collect the other characters.
       buffer_index = 1;
       
       Serial.println("** Found a C.  Starting to build");  
}

void begin_string_processing( void )
{
  Serial.println("String Command found:");
}

parse_state_type look_for_command( char c )
{
  
   if (c == 'C')
   {
       begin_crgb_build();
       
       return BUILD_CRGB;
     }
     else if (c == 'S')
     {
       // This is the start of a string.
       begin_string_processing();
       
       return PROCESS_STRING;
     }
     else
     {
       // not a valid command.  Keep looking.
       return LOOK_FOR_COMMAND;
     }
     
}  // end of look_for_command

parse_state_type process_string( char c )
{
   // enters and nulls terminate the string...we'll go back to looking for a command.
   if ( (c == '\n') || (c == NULL) )
   {
      // Since we're done with the string, lets print a line feed to make our output a little cleaner.
      Serial.println();

      // ...and we're back to looking for a command.
      return LOOK_FOR_COMMAND;
    }
    else
    {
      // just print the character.
      Serial.print(c);

      // ...and we're continuing to process the string.
      return PROCESS_STRING; 
    }
}     

parse_state_type build_crgb( char c )
{
   CRGB light_value;

    // If we're in the act of building a CRGB and we see either a C or an S, it's a new command.

    if (c == 'C')
    {
      // if we see a 'C', we need to start a new CRGB.
      begin_crgb_build();

      return BUILD_CRGB;
    }
    
    if (c == 'S')
    {
      // if we see an 'S', we need to start processing the string.
      begin_string_processing();

      return PROCESS_STRING;  
    }
    
    
    Serial.print("** Building CRGB, index ");
    Serial.print(buffer_index);
    Serial.print(" , char=");
    Serial.println(c);

    
    myBuffer[buffer_index] = c;

    // try what we've got so far...
    light_value = convertCRGBString(myBuffer);

    if (light_value)
    {
       Serial.println("FOUND VALID CRGB!!!");

       //go back to looking for a Command...
       buffer_index = 0;
       return LOOK_FOR_COMMAND;
    }
    else
    {
       // no valid CRGB (at least yet).  Did we run out of characters?
       buffer_index++;
       if (buffer_index >= BUFF_SIZE)
       {
           // We've seen too many characters.  This can't be a CRGB.
           buffer_index = 0;

           Serial.println("** Too many chars.  back to looking for command");
           return LOOK_FOR_COMMAND;
        }
        else
        {
          // Still got more characters to parse.  Keep building the CRGB.
          return BUILD_CRGB;
        }
         
     }  // end build CRGB 

}

void setup( ) 
{ 
  Serial.begin(9600); 

  buffer_index = 0;
  parse_state = LOOK_FOR_COMMAND;
} 

void loop ( ) 
{ 
   char c;
   parse_state_type next_state;

   while (Serial.available())
   {
     c = Serial.read();

     switch (parse_state)
     {
       case LOOK_FOR_COMMAND:
         next_state = look_for_command(c);
       break;

       case PROCESS_STRING:
         next_state = process_string(c);
       break;

       case BUILD_CRGB:
         next_state = build_crgb(c);
       break;

       default:
         Serial.print("UNKNOWN STATE: ");
         Serial.println(parse_state);
     }  // end of switch on parse state

     #ifdef DEBUG_STATE_MACHINE
     Serial.print("Current State:  ");
     Serial.print(parse_state);
     Serial.print(", input: ");
     Serial.print(c);
     Serial.print(", Next State: ");
     Serial.println(next_state);
     #endif

     parse_state = next_state;
               
   }  // end serial available
   
} // end of loop

CRGB convertCRGBString(char *input ) 
{ 
  int Rvalue = 0; 
  int Gvalue = 0; 
  int Bvalue = 0; 
  int CommaCount = 0; 
  bool Cfound = false; 
  bool RvalueGood = false; 
  bool GvalueGood = false; 
  bool BvalueGood = false; 
  while (*input) // while anything is available to read from the passed string 
  { 
    if (*input == 'C') // if the current char in the string is 'C' 
    {  
      Cfound = true; 
      Serial.println("C found!"); 
      input++; 
    }  
    if (*input == ',') 
    { 
       CommaCount++; 
       Serial.println("Comma found"); 
       input++; 
    }   
    if ((makeCharAnInt(*input) >= 0) &&  
              (makeCharAnInt(*input) <= 9) && (Cfound == true))    
    { 
        if (CommaCount == 0) 
          {   
            Rvalue = Rvalue * 10; 
            Rvalue = Rvalue + makeCharAnInt(*input);   
          }  
                     
        else if (CommaCount == 1) 
          {     
            Gvalue = Gvalue * 10; 
            Gvalue = Gvalue + makeCharAnInt(*input);   
          }  

        else if (CommaCount == 2) 
          { 
            Bvalue = Bvalue * 10; 
            Bvalue = Bvalue + makeCharAnInt(*input);  
          }  
        input++;    
     } 
     else 
        input++; 
   } 
   if (Rvalue <= 255) 
      RvalueGood = true; 
   if (Bvalue <= 255) 
      BvalueGood = true; 
   if (Gvalue <= 255) 
      GvalueGood = true; 
                   
  Serial.println(); 
  Serial.print ("CommaCount = "); 
  Serial.println(CommaCount); 
  Serial.print ("Rvalue = "); 
  Serial.println(Rvalue); 
  Serial.print ("Gvalue = "); 
  Serial.println(Gvalue); 
  Serial.print ("Bvalue = "); 
  Serial.println(Bvalue); 
  Serial.print("Cfound? = "); 
  Serial.println(Cfound); 
       
  if (Cfound && (CommaCount >= 2) && RvalueGood && BvalueGood && GvalueGood )  
    return CRGB (Rvalue, Bvalue, Gvalue); 
  else  
    return CRGB (0,0,0); 
} 

int getRValue( char *input ) 
{ 
  int Rvalue = 0; 
  bool Rfound = false; 
  while ((*input) && (!Rfound)) // while anything is available to read from the passed string 
  { 
    if (*input == 'R') // if the current char in the string is 'R' 
    {  

      input++; 
      while ((makeCharAnInt(*input) >= 0) && //current char is digit between 0-9 
              (makeCharAnInt(*input) <= 9)) 
      { 
        Rfound = true;  // R has been found in the string 
        Rvalue = Rvalue * 10; 
        Rvalue = Rvalue + makeCharAnInt(*input); 
        input++; 
      }    
    } 
     input++; 
  } 
   
  if (Rfound) // return Rvalue if there is one, otherwise error code -1 
    return Rvalue; 
  else  
    return -1; 
} 

int makeCharAnInt( char c )  
{ 
   int newInteger; 
   if ((c >= 48) && (c <= 57))  
      { 
         newInteger = c - 48; 
         return newInteger; 
      } 
   else  
      { 
          return -1; 
      }     
}   

\
