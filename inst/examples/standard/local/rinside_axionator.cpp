#include <RInside.h>
 
const char* hello( std::string who ) {
  std::string result( "hello ") ;
  result += who ;
  return result.c_str() ;
} 

int main(int argc, char *argv[]) {
 
  RInside R(argc, argv);               
 
  R["hello"] = Rcpp::InternalFunction( &hello );
  std::string result = R.parseEval("hello('world')") ;
  std::cout << "hello( 'world') =  " << result << std::endl ; 
    
  return 0;
}
