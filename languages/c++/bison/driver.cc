#include <cctype>
#include <fstream>
#include <cassert>

#include "driver.hh"

Demo::Driver::~Driver()
{
   delete(scanner);
   scanner = nullptr;
   delete(parser);
   parser = nullptr;
}

void
Demo::Driver::parse( const char * const filename )
{
   assert( filename != nullptr );
   std::ifstream in_file( filename );
   if( ! in_file.good() )
   {
       exit( EXIT_FAILURE );
   }
   parse_helper( in_file );
   return;
}

void
Demo::Driver::parse( std::istream &stream )
{
   if( ! stream.good()  && stream.eof() )
   {
       return;
   }
   //else
   parse_helper( stream );
   return;
}


void
Demo::Driver::parse_helper( std::istream &stream )
{

   delete(scanner);
   try
   {
      scanner = new Demo::Scanner( &stream );
   }
   catch( std::bad_alloc &ba )
   {
      std::cerr << "Failed to allocate scanner: (" <<
         ba.what() << "), exiting!!\n";
      exit( EXIT_FAILURE );
   }

   delete(parser);
   try
   {
      parser = new Demo::Parser( (*scanner) /* scanner */,
                                  (*this) /* driver */ );
   }
   catch( std::bad_alloc &ba )
   {
      std::cerr << "Failed to allocate parser: (" <<
         ba.what() << "), exiting!!\n";
      exit( EXIT_FAILURE );
   }
   if( parser->parse() != 0 )
   {
      std::cerr << "Parse failed!!\n";
   }
   return;
}

void
Demo::Driver::add_upper()
{
   uppercase++;
   chars++;
   words++;
}

void
Demo::Driver::add_lower()
{
   lowercase++;
   chars++;
   words++;
}

void
Demo::Driver::add_word( const std::string &word )
{
   words++;
   chars += word.length();
   for(const char &c : word ){
      if( islower( c ) )
      {
         lowercase++;
      }
      else if ( isupper( c ) )
      {
         uppercase++;
      }
   }
}

void
Demo::Driver::add_newline()
{
   lines++;
   chars++;
}

void
Demo::Driver::add_char()
{
   chars++;
}


std::ostream&
Demo::Driver::print( std::ostream &stream )
{
   stream << red  << "Results: " << norm << "\n";
   stream << blue << "Uppercase: " << norm << uppercase << "\n";
   stream << blue << "Lowercase: " << norm << lowercase << "\n";
   stream << blue << "Lines: " << norm << lines << "\n";
   stream << blue << "Words: " << norm << words << "\n";
   stream << blue << "Characters: " << norm << chars << "\n";
   return(stream);
}
