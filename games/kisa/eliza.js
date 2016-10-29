#!/usr/bin/env js
// Chat Bot by George Dunlop, www.peccavi.com
// Note - Eliza is a Classic Model of chat Bots.. but this implementation is mine :)
// May be used/modified if credit line is retained (c) 1997 All rights reserved

// OBJECT TYPE DEFINITIONS

// Keys

maxKey = 36;
keyNotFound = maxKey-1;
keyword = new Array(maxKey);

function key (key, idx, end)
{
	this.key = key;               			// phrase to match
	this.idx = idx;               			// first response to use
	this.end = end;               			// last response to use
	this.last = end;				// response used last time
}

maxrespnses = 116;
response = new Array (maxrespnses);

maxConj = 19;
max2ndConj = 7;

var conj1 = new Array (maxConj);
var conj2 = new Array (maxConj);
var conj3 = new Array (max2ndConj);
var conj4 = new Array (max2ndConj);


// Funtion to replaces all occurances of substring substr1 with substr2 within strng
// if type == 0 straight string replacement
// if type == 1 assumes padded strings and replaces whole words only
// if type == 2 non case sensitive assumes padded strings to compare whole word only
// if type == 3 non case sensitive straight string replacement

var RPstrg = "";

function replaceStr (strng, substr1, substr2, type)
{
	var pntr = -1;

	aString = strng;
	if( type == 0 ){
		if( strng.indexOf( substr1 ) >= 0 ){ pntr = strng.indexOf( substr1 );	}
	} else if( type == 1 ){
		if( strng.indexOf( " "+ substr1 +" " ) >= 0 ){ pntr = strng.indexOf( " " + substr1 + " " ) + 1; }
	} else if( type == 2 ){
		bstrng = strng.toUpperCase();
		bsubstr1 = substr1.toUpperCase();
		if( bstrng.indexOf( " "+ bsubstr1 +" " )>= 0 ){ pntr = bstrng.indexOf( " " + bsubstr1 + " " ) + 1; }
	} else {
		bstrng = strng.toUpperCase();
		bsubstr1 = substr1.toUpperCase();
		if( bstrng.indexOf( bsubstr1 ) >= 0 ){ pntr = bstrng.indexOf( bsubstr1 ); }
	}
	if( pntr >= 0 ){
		RPstrg += strng.substring( 0, pntr ) + substr2;
		aString = strng.substring(pntr + substr1.length, strng.length );
		if( aString.length > 0 ){ replaceStr( aString, substr1, substr2, type ); }
	}
	aString =  RPstrg + aString;
	RPstrg = "";
	return aString;
}

// Function to pad a string.. head, tail & punctuation

punct = new Array (".", ",", "!", "?", ":", ";", "&", '"', "@", "#", "(", ")" )

function padString (strng)
{
	aString = " " + strng + " ";
	for( i=0; i < punct.length; i++ ){
		aString = replaceStr( aString, punct[i], " " + punct[i] + " ", 0 );
	}
	return aString
}

// Function to strip padding

function unpadString (strng)
{
	aString = strng;
	aString = replaceStr( aString, "  ", " ", 0 ); 		// compress spaces
	if( strng.charAt( 0 ) == " " ){ aString = aString.substring(1, aString.length ); }
	if( strng.charAt( aString.length - 1 ) == " " ){ aString = aString.substring(0, aString.length - 1 ); }
	for( i=0; i < punct.length; i++ ){
		aString = replaceStr( aString, " " + punct[i], punct[i], 0 );
	}
	return aString
}

// Dress Input formatting i.e leading & trailing spaces and tail punctuation

var ht = 0;												// head tail stearing

function strTrim (strng)
{
	if(ht == 0){ loc = 0; }									// head clip
	else { loc = strng.length - 1; }						// tail clip  ht = 1
	if( strng.charAt( loc ) == " "){
		aString = strng.substring( - ( ht - 1 ), strng.length - ht);
		aString = strTrim(aString);
	} else {
		var flg = false;
		for(i=0; i<=5; i++ ){ flg = flg || ( strng.charAt( loc ) == punct[i]); }
		if(flg){
			aString = strng.substring( - ( ht - 1 ), strng.length - ht );
		} else { aString = strng; }
		if(aString != strng ){ strTrim(aString); }
	}
	if( ht ==0 ){ ht = 1; strTrim(aString); }
	else { ht = 0; }
	return aString;
}

// adjust pronouns and verbs & such

function conjugate (sStrg)	// rephrases sString
{
	var sString = sStrg;
	for( i = 0; i < maxConj; i++ ){			// decompose
		sString = replaceStr( sString, conj1[i], "#@&" + i, 2 );
	}
	for( i = 0; i < maxConj; i++ ){			// recompose
		sString = replaceStr( sString, "#@&" + i, conj2[i], 2 );
	}
	// post process the resulting string
	for( i = 0; i < max2ndConj; i++ ){			// decompose
		sString = replaceStr( sString, conj3[i], "#@&" + i, 2 );
	}
	for( i = 0; i < max2ndConj; i++ ){			// recompose
		sString = replaceStr( sString, "#@&" + i, conj4[i], 2 );
	}
	return sString;
}

// Build our response string
// get a random choice of response based on the key
// Then structure the response

var pass = 0;
var thisstr = "";

function phrase( sString, keyidx )
{
	idxmin = keyword[keyidx].idx;
	idrange = keyword[keyidx].end - idxmin + 1;
	choice = keyword[keyidx].idx + Math.floor( Math.random() * idrange );
	if( choice == keyword[keyidx].last && pass < 5 ){
		pass++; phrase(sString, keyidx );
	}
	keyword[keyidx].last = choice;
	var rTemp = response[choice];
	var tempt = rTemp.charAt( rTemp.length - 1 );
	if(( tempt == "*" ) || ( tempt == "@" )){
		var sTemp = padString(sString);
		var wTemp = sTemp.toUpperCase();
		var strpstr = wTemp.indexOf( " " + keyword[keyidx].key + " " );
	strpstr += keyword[ keyidx ].key.length + 1;
		thisstr = conjugate( sTemp.substring( strpstr, sTemp.length ) );
		thisstr = strTrim( unpadString(thisstr) )
		if( tempt == "*" ){
			sTemp = replaceStr( rTemp, "<*", " " + thisstr + "?", 0 );
		} else { sTemp = replaceStr( rTemp, "<@", " " + thisstr + ".", 0 );
		}
	} else sTemp = rTemp;
	return sTemp;
}

// returns array index of first key found

var keyid = 0;

function testkey(wString)
{
	if( keyid < keyNotFound
		&& !( wString.indexOf( " " + keyword[keyid].key + " ") >= 0 )){
		keyid++; testkey(wString);
	}
}

function findkey(wString)
{
	keyid = 0;
	found = false;
	testkey(wString);
	if( keyid >= keyNotFound ){ keyid = keyNotFound; }
	return keyid;
}

// This is the entry point and the I/O of this code

var wTopic = "";											// Last worthy responce
var sTopic = "";											// Last worthy responce
var greet = false;
var wPrevious = "";					// so we can check for repeats
var started = false;

function listen(User)
{
	sInput = User;
	if (started) {
		clearTimeout(Rtimer);
	}
	Rtimer = setTimeout("wakeup()", 180000);	// wake up call
	started = true;										// needed for Rtimer
	sInput = strTrim(sInput);			// dress input formating
	if( sInput != "" ){
		wInput = padString(sInput.toUpperCase());	// Work copy
		var foundkey = maxKey;         		  		// assume it's a repeat input
		if (wInput != wPrevious){   					// check if user repeats himself
			foundkey = findkey(wInput);   			// look for a keyword.
		}
		if( foundkey == keyNotFound ){
			if( !greet ){ greet = true; return "Don't you ever say Hello?" }
			else {
				wPrevious = wInput;          			// save input to check repeats
				if(( sInput.length < 10 ) && ( wTopic != "" ) && ( wTopic != wPrevious )){
					lTopic = conjugate( sTopic ); sTopic = ""; wTopic = "";
					return 'OK... "' + lTopic + '". Tell me more.';
				} else {
					if( sInput.length < 15 ){
						return "Tell me more...";
					} else { return phrase( sInput, foundkey ); }
				}
			}
		} else {
			if( sInput.length > 12 ){ sTopic = sInput; wTopic = wInput; }
			greet = true; wPrevious = wInput;  			// save input to check repeats
			return phrase( sInput, foundkey );			// Get our response
		}
	} else {
		return "I can't help, if you will not chat with me!";
	}
}

function wakeup()
{
	var strng1 = "    *** Are We going to Chat? ***";
	var strng2 = "  I can't help you without a dialog!";
	update (strng1, strng2);
}

// build our data base here

conj1[0]  = "are";   	conj2[0]  = "am";
conj1[1]  = "am";   	conj2[1]  = "are";
conj1[2]  = "were";  	conj2[2]  = "was";
conj1[3]  = "was";  	conj2[3]  = "were";
conj1[4]  = "I";    	conj2[4]  = "you";
conj1[5]  = "me";    	conj2[5]  = "you";
conj1[6]  = "you";   	conj2[6]  = "me";
conj1[7]  = "my";    	conj2[7]  = "your";
conj1[8]  = "your";  	conj2[8]  = "my";
conj1[9]  = "mine";  	conj2[9]  = "your's";
conj1[10] = "your's"; 	conj2[10] = "mine";
conj1[11] = "I'm";   	conj2[11] = "you're";
conj1[12] = "you're";  	conj2[12] = "I'm";
conj1[13] = "I've";  	conj2[13] = "you've";
conj1[14] = "you've"; 	conj2[14] = "I've";
conj1[15] = "I'll"; 	conj2[15] = "you'll";
conj1[16] = "you'll"; 	conj2[16] = "I'll";
conj1[17] = "myself"; 	conj2[17] = "yourself";
conj1[18] = "yourself"; conj2[18] = "myself";

// array to post process correct our tenses of pronouns such as "I/me"

conj3[0]  = "me am";   	conj4[0]  = "I am";
conj3[1]  = "am me";   	conj4[1]  = "am I";
conj3[2]  = "me can";   conj4[2]  = "I can";
conj3[3]  = "can me";   conj4[3]  = "can I";
conj3[4]  = "me have";  conj4[4]  = "I have";
conj3[5]  = "me will";  conj4[5]  = "I will";
conj3[6]  = "will me";  conj4[6]  = "will I";


// Keywords

keyword[ 0]=new key( "CAN YOU",  	1,  3);
keyword[ 1]=new key( "CAN I",    	4,  5);
keyword[ 2]=new key( "YOU ARE",  	6,  9);
keyword[ 3]=new key( "YOU'RE",   	6,  9);
keyword[ 4]=new key( "I DON'T",  	10, 13);
keyword[ 5]=new key( "I FEEL",   	14, 16);
keyword[ 6]=new key( "WHY DON'T YOU",	17, 19);
keyword[ 7]=new key( "WHY CAN'T I", 	20, 21);
keyword[ 8]=new key( "ARE YOU",  	22, 24);
keyword[ 9]=new key( "I CAN'T",  	25, 27);
keyword[10]=new key( "I AM",     	28, 31);
keyword[11]=new key( "I'M",      	28, 31);
keyword[12]=new key( "YOU",      	32, 34);
keyword[13]=new key( "I WANT",   	35, 39);
keyword[14]=new key( "WHAT",     	40, 48);
keyword[15]=new key( "HOW",      	40, 48);
keyword[16]=new key( "WHO",      	40, 48);
keyword[17]=new key( "WHERE",    	40, 48);
keyword[18]=new key( "WHEN",     	40, 48);
keyword[19]=new key( "WHY",      	40, 48);
keyword[20]=new key( "NAME",     	49, 50);
keyword[21]=new key( "CAUSE",    	51, 54);
keyword[22]=new key( "SORRY",    	55, 58);
keyword[23]=new key( "DREAM",    	59, 62);
keyword[24]=new key( "HELLO",    	63, 63);
keyword[25]=new key( "HI",       	63, 63);
keyword[26]=new key( "MAYBE",    	64, 68);
keyword[27]=new key( "NO",       	69, 73);
keyword[28]=new key( "YOUR",     	74, 75);
keyword[29]=new key( "ALWAYS",   	76, 79);
keyword[30]=new key( "THINK",    	80, 82);
keyword[31]=new key( "ALIKE",    	83, 89);
keyword[32]=new key( "YES",      	90, 92);
keyword[33]=new key( "FRIEND",   	93, 98);
keyword[34]=new key( "COMPUTER", 	99, 105);
keyword[35]=new key( "NO KEY FOUND", 	106, 112);
keyword[36]=new key( "REPEAT INPUT", 	113, 116);


response[  0]="ELIZA - Javascript Version by George Dunlop ( george@peccavi.com )"
response[  1]="Don't you believe that I can<*";
response[  2]="Perhaps you would like to be able to<*";
response[  3]="You want me to be able to<*";
response[  4]="Perhaps you don't want to<*";
response[  5]="Do you want to be able to<*";
response[  6]="What makes you think I am<*";
response[  7]="Does it please you to believe I am<*";
response[  8]="Perhaps you would like to be<*";
response[  9]="Do you sometimes wish you were<*";
response[ 10]="Don't you really<*";
response[ 11]="Why don't you<*";
response[ 12]="Do you wish to be able to<*";
response[ 13]="Does that trouble you?";
response[ 14]="Tell me more about such feelings.";
response[ 15]="Do you often feel<*";
response[ 16]="Do you enjoy feeling<*";
response[ 17]="Do you really believe I don't<*";
response[ 18]="Perhaps in good time I will<@";
response[ 19]="Do you want me to<*";
response[ 20]="Do you think you should be able to<*";
response[ 21]="Why can't you<*";
response[ 22]="Why are you interested in whether or not I am<*";
response[ 23]="Would you prefer if I were not<*";
response[ 24]="Perhaps in your fantasies I am<*";
response[ 25]="How do you know you can't<*";
response[ 26]="Have you tried?";
response[ 27]="Perhaps you can now<*";
response[ 28]="Did you come to me because you are<*";
response[ 29]="How long have you been<*";
response[ 30]="Do you believe it is normal to be<*";
response[ 31]="Do you enjoy being<*";
response[ 32]="We were discussing you, not me.";
response[ 33]="Oh... <*";
response[ 34]="You're not really talking about me, are you?";
response[ 35]="What would it mean to you if you got<*";
response[ 36]="Why do you want<*";
response[ 37]="Suppose you got<*";
response[ 38]="What if you never got<*";
response[ 39]="I sometimes also want<@";
response[ 40]="Why do you ask?";
response[ 41]="Does that question interest you?";
response[ 42]="What answer would please you the most?";
response[ 43]="What do you think?";
response[ 44]="Are such questions on your mind often?";
response[ 45]="What is it that you really want to know?";
response[ 46]="Have you asked anyone else?";
response[ 47]="Have you asked such questions before?";
response[ 48]="What else comes to mind when you ask that?";
response[ 49]="Names don't interest me.";
response[ 50]="I don't care about names, please go on.";
response[ 51]="Is that the real reason?";
response[ 52]="Don't any other reasons come to mind?";
response[ 53]="Does that reason explain anything else?";
response[ 54]="What other reasons might there be?";
response[ 55]="Please don't apologise!";
response[ 56]="Apologies are not necessary.";
response[ 57]="What feelings do you have when you apologise?";
response[ 58]="Don't be so defensive!";
response[ 59]="What does that dream suggest to you?";
response[ 60]="Do you dream often?";
response[ 61]="What persons appear in your dreams?";
response[ 62]="Are you disturbed by your dreams?";
response[ 63]="How are you today.. What would you like to discuss?";
response[ 64]="You don't seem quite certain.";
response[ 65]="Why the uncertain tone?";
response[ 66]="Can't you be more positive?";
response[ 67]="You aren't sure?";
response[ 68]="Don't you know?";
response[ 69]="Are you saying no just to be negative?";
response[ 70]="You are being a bit negative.";
response[ 71]="Why not?";
response[ 72]="Are you sure?";
response[ 73]="Why no?";
response[ 74]="Why are you concerned about my<*";
response[ 75]="What about your own<*";
response[ 76]="Can you think of a specific example?";
response[ 77]="When?";
response[ 78]="What are you thinking of?";
response[ 79]="Really, always?";
response[ 80]="Do you really think so?";
response[ 81]="But you are not sure you<*";
response[ 82]="Do you doubt you<*";
response[ 83]="In what way?";
response[ 84]="What resemblence do you see?";
response[ 85]="What does the similarity suggest to you?";
response[ 86]="What other connections do you see?";
response[ 87]="Could there really be some connection?";
response[ 88]="How?";
response[ 89]="You seem quite positive.";
response[ 90]="Are you Sure?";
response[ 91]="I see.";
response[ 92]="I understand.";
response[ 93]="Why do you bring up the topic of friends?";
response[ 94]="Do your friends worry you?";
response[ 95]="Do your friends pick on you?";
response[ 96]="Are you sure you have any friends?";
response[ 97]="Do you impose on your friends?";
response[ 98]="Perhaps your love for friends worries you.";
response[ 99]="Do computers worry you?";
response[100]="Are you talking about me in particular?";
response[101]="Are you frightened by machines?";
response[102]="Why do you mention computers?";
response[103]="What do you think machines have to do with your problems?";
response[104]="Don't you think computers can help people?";
response[105]="What is it about machines that worries you?";
response[106]="Say, do you have any psychological problems?";
response[107]="What does that suggest to you?";
response[108]="I see.";
response[109]="I'm not sure I understand you fully.";
response[110]="Come, come, elucidate your thoughts.";
response[111]="Can you elaborate on that?";
response[112]="That is quite interesting.";
response[113]="Why did you repeat yourself?";
response[114]="Do you expect a different answer by repeating yourself?";
response[115]="Come, come, elucidate your thoughts.";

response[116]="Please don't repeat yourself!";

///////////////////////////////////////////////////////////////
//***********************************************************//
//* everything below here was originally in dia_1.html      *//
//***********************************************************//
///////////////////////////////////////////////////////////////

// Chat Bot by George Dunlop, www.peccavi.com
// May be used/modified if credit line is retained (c) 1997 All rights reserved

// Put together an array for the dialog

chatmax = 5;						// number of lines / 2
chatpoint = 0;
chatter = new Array(chatmax);

// Wait function to allow our pieces to get here prior to starting

function hello()
{
	chatter[chatpoint] = "> Hello, I am Eliza.";
	chatpoint = 1;
	return write();
}

function start()
{
	for (i = 0; i < chatmax; i++) {
		chatter[i] = "";
	}
	chatter[chatpoint] = "  Loading...";
	write();
	hello();
}

// Fake time thinking to allow for user self reflection
// And to give the illusion that some thinking is going on

var elizaresponse = "";

function think()
{
	document.Eliza.input.value = "";
	if (elizaresponse != "") {
		respond();
	} else {
		setTimeout("think()", 250);
	}
}

function dialog()
{
	var Input = document.Eliza.input.value;	  // capture input and copy to log
	document.Eliza.input.value = "";
	chatter[chatpoint] = " \n* " + Input;
	elizaresponse = listen(Input);
	setTimeout("think()", 1000 + Math.random()*3000);
	chatpoint ++ ;
	if( chatpoint >= chatmax ){ chatpoint = 0; }
	return write();
}

function respond()
{
	chatpoint -- ;
	if( chatpoint < 0 ){ chatpoint = chatmax-1; }
	chatter[chatpoint] += "\n> " + elizaresponse;
	chatpoint ++ ;
	if( chatpoint >= chatmax ){ chatpoint = 0; }
	return write();
}

// Allow for unprompted response from the engine

function update (str1, str2)
{
	chatter[chatpoint] = " \n> " + str1;
	chatter[chatpoint] += "\n> " + str2;
	chatpoint ++ ;
	if( chatpoint >= chatmax ){ chatpoint = 0; }
	return write();
}

function write()
{
	document.Eliza.log.value = "";
	for(i = 0; i < chatmax; i++){
		n = chatpoint + i;
		if (n < chatmax) {
			print (chatter [n]);
		} else {
			print (chatter [n - chatmax]);
		}
	}
	refresh();
	return false;				// don't redraw the ELIZA's form!
}

// Correct user overwrites

function refresh()
{
	setTimeout ("write()", 10000);
}

print ("Not implemented yet");
