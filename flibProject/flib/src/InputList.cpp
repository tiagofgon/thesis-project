// Class InputList
// Inspired in Fortran's Namelist facility
//
// This utility uses:
// -) The string class
// -) The C++ I/O : classes iostream and ifstream (input from file)
// -) Careful error checking in the C++ I/O operations
// -) STL : the "list<T>" class that provides a linked list of objects
// -) STL : iterators to access the list of objects (objects are
//    descriptors of data items that must be read from file)
// ---------------------------------------------------------------

#include <InputList.hpp>
#include <StringTokenizer.hpp>
#include <list>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

// *******************************************************
// Auxiliary Function object needed to find InputObjects
// in the linked list with the find_if algorithm
// *******************************************************

class IOEqual
   {
   private:
   string tok;

   public:
   IOEqual(string S) : tok(S) {}

   bool operator() ( InputObj& IO )
      {
      if(IO.vName == tok) return true;
      else return false;
      }
   };

//****************************************************
// Auxiliary function used to report the status of the
// linked list with the for_each algorithm
// ***************************************************

void PrintStatus(InputObj& IO)
   {
   cout << "\nStatus of " << IO.vName << " is " << IO.vStatus;
   }
   
   
// **************
// Class InputObj
// **************

// Constructors for Input objects
// ------------------------------

InputObj::InputObj(string myname, void *addr, DataType type, int size)
    {
    vName = myname;
    vPtr  = addr;
    vType = type;
    vLen  = size;
    vStatus = 1;     // no data yet
    }
 
// ****************
// Class InputList
// ****************


void InputList::RegisterData(string str, void *addr, DataType type, int size)
   {
   InputObj node(str, addr, type, size);
   L.push_back(node);
   }

void InputList::UnregisterData(string str)
   {
   std::list<InputObj>::iterator pos;
   pos = find_if( L.begin(), L.end(), IOEqual(str) );
   if(pos != L.end()) L.erase(pos);
   }


void InputList::ReportStatus()
   {
   for_each( L.begin(), L.end(), PrintStatus);
   cout << endl;
   }

void InputList::UnregisterAllData()
   {
   InputObj IO;
   std::list<InputObj>::iterator pos = L.begin();
   while( pos != L.end() )
       {
       IO = *pos;
       UnregisterData(IO.vName);
       ++pos;
       }
   }

// Destructor
// ----------
InputList::~InputList()
   {
   //UnregisterAllData();
   }


// --------------------------------------------------------------
// This function assumes that a line from the input file has been
// copied to a string S, passed as argument
//
// This function modifies the linked list: an element is deleted
// and replaced by a new one pushed at the end of the list. This 
// means  that the order of elements is not preserved. But this 
// auxiliary function is only called by ReadData() which, at each 
// call, scans the whole list. Therefore, the fact that the order 
// is not preserved is irrelevant.
//
// Return values : IO.vStatus
// ------------------------------------------------------------

int InputList::InitializeObject(string& inputStr)
   {
   int n, ntk;
   string token;
   StringTokenizer ST;
   const string delims(" \t\n");
   InputObj IO;
   istringstream istr;

   // Get the first token of the string:
   // ---------------------------------
   ST.SetString(inputStr);
   token = ST.GetToken(delims);   

   // Get the InputObj that corresponds to "token"
   // --------------------------------------------
   std::list<InputObj>::iterator pos;
   pos = find_if( L.begin(), L.end(), IOEqual(token) ); 
   if( pos == L.end()) return 1;
   else
       {
       IO = *pos;    // remember, this is a COPY of what is stored in L
       IO.vStatus = 0;
       }
   
   // Here, we are OK, the string corresponds to a given IO. We go
   // ahead with the parsing. 
   // First, initialize the stringstream buffer with the remaining
   // content of the input strung (the data content) 
   // -----------------------------------------------------------
   string dataStr(inputStr, token.size()+1);
   istr.str(dataStr);

   // Next, check how many data tokens are available
   // ----------------------------------------------
   ntk = 0;
   token = ST.GetToken(delims);
   while( token != "")
       {
       ntk ++;
       token = ST.GetToken(delims);
       }
    if(ntk < IO.vLen) IO.vStatus = 2;   // not enough  data
    if(ntk > IO.vLen) 
        {
        IO.vStatus = 3;                // too much data
        ntk = IO.vLen;
        }
    
   // ------------------------------------------------------
   // the COPY IO has been changed. We need to replace the list 
   // object by erasing the element at the present position 
   // and pushing back the new one (the order in the linked 
   // list does not matter)
   // -------------------------------------------
   L.erase(pos);
   L.push_back(IO);

   // Now, we can perform the data write
   // ----------------------------------
   switch(IO.vType)
       {
       case EMPTY: break;
       case NI:  for(n=0; n<IO.vLen; n++) istr >> *( (int *)IO.vPtr + n );
		 break;
       case NL:  for(n=0; n<IO.vLen; n++) istr >> *( (long *)IO.vPtr + n );
		 break;
       case NUI: for(n=0; n<IO.vLen; n++) 
                     istr >> *( (unsigned int *)IO.vPtr + n );
		 break;
       case NUL: for(n=0; n<IO.vLen; n++) 
                     istr >> *( (unsigned long *)IO.vPtr + n );
		 break;
       case NF:  for(n=0; n<IO.vLen; n++) istr >> *( (float *)IO.vPtr + n );
                 break;
       case ND:  for(n=0; n<IO.vLen; n++) istr >> *( (double *)IO.vPtr + n );
		 break;
       }

   return IO.vStatus;
   }

// ------------------------------------------------------
// This function returns 1 if everything was OK, and 0 if
// some error occurred. In itds present form, it strips the
// comment part of each line (comments start with "#")
// ------------------------------------------------------

int InputList::ReadData(const char *filename)
    {
    int retval, return_value, index, count;
    ifstream inFile(filename, ios::in);
    char buffer[1028];
    count = 0;     // used to count lines
   
    return_value = 1;
    do
       {
       count++;
       inFile.getline(buffer, 1028, '\n' );

       // check that there is no IO error other than EOF 
       if(!inFile.eof() && (inFile.fail() || inFile.bad()) )
           {
           cout << "\nInput error in reading line " << count << endl;
           inFile.close();
           return 0;
           }
       string S(buffer);
       
       // eliminate comment piece of line
       // -------------------------------
       index = S.find("#");
       if(index >= 0) S.resize(index);

       if( S!="" )
           {                 
           retval = InitializeObject(S);
           switch(retval)
              {
	      case 1: cout << " ** no data registered for line " 
                           << count  << endl; 
                      return_value = 0; 
                      break;
	      case 2: cout << " ** data missing in line " << count << endl; 
                      return_value = 0; 
                      break;
	      case 3: cout << " ** redundant data in line " << count << endl;
                      return_value=0; 
                      break;
	      case 4: cout << " **  Error in writing to data: CHECK input " 
                           << count << endl;
                      return_value=0; 
                      break;
              default: break;
	      }
           }

       } while(inFile.good());

    return return_value;
    }


// -----------------------------------------------------------
// This form of ReadData only reads the input file between two
// delimiter lines. Delimiter is passed as second argument
// Comments starting with "#" are accepted
// -----------------------------------------------------------

int InputList::ReadData(const char *filename, const char *delim)
    {
    int retval, return_value,count;
    ifstream inFile(filename, ios::in);
    string::size_type index;
    char buffer[1028];
    string ST, D(delim);
    count = 0;     // used to count lines
    return_value = 1;
 
    do
       {
       count++;
       inFile.getline(buffer, 1028, '\n' );

       // check that there is no IO error other than EOF 
       if(!inFile.eof() && (inFile.fail() || inFile.bad()) )
           {
           cout << "\nInput error in reading line " << count << endl;
           inFile.close();
           return 0;
           }
       string S(buffer);
       
       // eliminate comment piece of line
       // -------------------------------
       index = S.find("#");
       if(index != string::npos) S.resize(index);
       ST = S;
       } while(inFile.good() && (ST != D));

    if( !inFile.good() )
       {
       cout << "\nError: delimiter not found in file" << endl;
       return 0;
       }

    // Start reading relevant part of file
    // -----------------------------------
    do
       {
       count++;
       inFile.getline(buffer, 1028, '\n' );

       // check that there is no IO error other than EOF 
       if(!inFile.eof() && (inFile.fail() || inFile.bad()) )
           {
           cout << "\nInput error in reading line " << count << endl;
           inFile.close();
           return 0;
           }
       string S(buffer);
       
       // eliminate comment piece of line
       // -------------------------------
       index = S.find("#");
       if(index != string::npos) S.resize(index);

       if( S!="" )
           {                 
           retval = InitializeObject(S);
           switch(retval)
              {
	      case 1: cout << " ** no data registered for line " 
                           << count  << endl; 
                      return_value = 0; 
                      break;
	      case 2: cout << " ** data missing in line " << count << endl; 
                      return_value = 0; 
                      break;
	      case 3: cout << " ** redundant data in line " << count << endl;
                      return_value=0; 
                      break;
	      case 4: cout << " **  Error in writing to data: CHECK input " 
                           << count << endl;
                      return_value=0; 
                      break;
              default: break;
	      }
           }
       ST = S;
       } while(inFile.good() && (ST != D));

    return return_value;
    }

//////////////////////////////////////////////////////
// This function prints the iList object IL to the
// standard output
//////////////////////////////////////////////////////

void InputList::PrintData()
    {
    int k;
    InputObj IO;
    
    cout << "\nData values :\n-----------\n";
   
   // Identify the InputObj that corresponds to "token"
   // -------------------------------------------------
   std::list<InputObj>::const_iterator pos = L.begin();
   while( pos != L.end() )
      {
      IO = *pos;
      cout << IO.vName << "\t";
      if( IO.vName.size() <8 ) cout << "\t";
      if(IO.vStatus != 1 )
	   {
	   for(k=0; k < IO.vLen; k++)
	     {
	     switch(IO.vType)
		{
	        case EMPTY: break;
	        case NI:
		   cout << *( (int *)IO.vPtr + k ) << "  ";
		   break;
	        case NL:
		   cout << *( (long *)IO.vPtr + k ) << "  ";
		   break;
	        case NUI:
		   cout << *( (unsigned int *)IO.vPtr + k ) << "  ";
		   break;
	        case NUL:
		   cout << *( (unsigned long *)IO.vPtr + k ) << "  ";
		   break;
	        case NF:
		   cout << *( (float *)IO.vPtr + k ) << "  ";
		   break;
	        case ND:
		   cout << *( (double *)IO.vPtr + k ) << "  ";
		   break;
		}
	     }
	   }

      switch(IO.vStatus)
         {
         case 0: break;
	 case 1: cout << " ** no input data "; break;
	 case 2: cout << " ** some data was missing "; break;
	 case 3: cout << " ** redundant data in input"; break;
	 }
      cout << endl;
      pos++;
      }
   }

///////////////////////////////////////////////////////////////////
	   

     
