/*
  TwitterCritterYun

  Sends a tweet via a Twitter account using Temboo from an Arduino Yun.

  See my Instructable at: http://www.instructables.com/id/E9EOCASGOTSYROS/ for details

  This example assumes basic familiarity with Arduino sketches, and that your Yun is connected
  to the Internet.
  
  When Arduino is powered on, a single tweet will be sent. If tweet can't be sent, it will retry over and over
  until successful.  Once it is successful once, it will stop sending tweets.
  
  The message is randomized to prevent twitter from blocking your tweets (they don't like duplicates)
  
  Please read more about this and see my other projects at http://manganlabs.com

*/

#include <Bridge.h>
#include <Temboo.h>
#include <String.h>
#include "TwitterCritterAcctInfo.h" // contains Twitter and Temboo account information 

/*** Random Sayings For Twitter ***/
prog_char string_0[] PROGMEM = "Take it like a man, shorty."; 
prog_char string_1[] PROGMEM = "Last one alive, lock the door!";
prog_char string_2[] PROGMEM = "Another successful procedure.";
prog_char string_3[] PROGMEM = "This is my world. You are not welcome in my world!";
prog_char string_4[] PROGMEM = "Whoops... that was not medicine.";
prog_char string_5[] PROGMEM = "Now is good time to run, cowards!";
prog_char string_6[] PROGMEM = "I had me good eye on you the whole time.";
prog_char string_7[] PROGMEM = "I told you not to touch that darned thing.";
prog_char string_8[] PROGMEM = "KABOOOOM!!!";
prog_char string_9[] PROGMEM = "Time to inform your next of kin!";
prog_char string_10[] PROGMEM = "Another satisfied customer!";  

// Then set up a table to refer to your strings.
PROGMEM const char *string_table[] = {
  string_0,
  string_1,
  string_2,
  string_3,
  string_4,
  string_5,
  string_6,
  string_7,
  string_8,
  string_9,
  string_10          
};

//The following two values are critical. If they are too large, they will cause memory issues
//with the Arduino and will be very difficult to troubleshoot.
char tweetbuffer[120]; // Default: 100, 120 - Used to construct the tweet from multiple strings
char tweetmsg[80]; // Default: 60, 80 - The main twitter message

int numRuns = 0;   // execution count, so this sketch doesn't run forever
int maxRuns = 1;  // the max number of times the Twitter Update should run
  
void setup() {

  Bridge.begin();
  Console.begin();
  delay(4000); // for debugging, wait until a serial console is connected
  
  /*** Generate Random Seed to pick random phrase ***/
  // if analog input pin 0 is disconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
  
}

void loop()
{
  
  // Only try to send the tweet if we haven't exceeded the defined maxRuns
  if (numRuns < maxRuns) {
    String tweet;
     
    strcpy(tweetmsg, "Critter caught! "); //main tweet message
    strcpy_P(tweetbuffer, (char*)pgm_read_word(&(string_table[random(10)])));
    strcat(tweetmsg, tweetbuffer); // append random phrase to main tweet msg
        
    // Convert char to string
    for(int i = 0; tweetmsg[i] != 0; i++)
    
    tweet += tweetmsg[i];
    String tweetText(tweet);    
   
    TembooChoreo StatusesUpdateChoreo;

    // invoke the Temboo client
    // NOTE that the client must be reinvoked, and repopulated with
    // appropriate arguments, each time its run() method is called.
    StatusesUpdateChoreo.begin();

    // set Temboo account credentials
    StatusesUpdateChoreo.setAccountName(TEMBOO_ACCOUNT);
    StatusesUpdateChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    StatusesUpdateChoreo.setAppKey(TEMBOO_APP_KEY);

    // identify the Temboo Library choreo to run (Twitter > Tweets > StatusesUpdate)
    StatusesUpdateChoreo.setChoreo("/Library/Twitter/Tweets/StatusesUpdate");

    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Twitter/Tweets/StatusesUpdate/
    // for complete details about the inputs for this Choreo

    // add the Twitter account information
    StatusesUpdateChoreo.addInput("AccessToken", TWITTER_ACCESS_TOKEN);
    StatusesUpdateChoreo.addInput("AccessTokenSecret", TWITTER_ACCESS_TOKEN_SECRET);
    StatusesUpdateChoreo.addInput("ConsumerKey", TWITTER_CONSUMER_KEY);
    StatusesUpdateChoreo.addInput("ConsumerSecret", TWITTER_CONSUMER_SECRET);

    // and the tweet we want to send
    StatusesUpdateChoreo.addInput("StatusUpdate", tweetText);

    // tell the Process to run and wait for the results. The
    // return code (returnCode) will tell us whether the Temboo client
    // was able to send our request to the Temboo servers
    unsigned int returnCode = StatusesUpdateChoreo.run();

    // a return code of zero (0) means everything worked
    if (returnCode == 0) {
      Console.println("Success! Tweet sent!");
      numRuns++;
    } else {
      // a non-zero return code means there was an error
      // read and print the error message
      while (StatusesUpdateChoreo.available()) {
        char c = StatusesUpdateChoreo.read();
        Console.print(c);
      }
    }
    StatusesUpdateChoreo.close();

    // do nothing for the next 90 seconds
    delay(90000);
  }
}

