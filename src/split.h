//split.h

#include <string>
#include <vector>

class Splitter {
  std::vector<std::string> _tokens;
public:
  typedef std::vector<std::string>::size_type size_type;
public:
  Splitter ( const std::string& src, const std::string& delim ){
    reset ( src, delim );
  }
  std::string& operator[] ( size_type i ){
    return _tokens.at ( i );
  }
  size_type size() const{
    return _tokens.size();
  }
  void reset ( const std::string& src, const std::string& delim ){
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end;
    for ( ; ; ) {
      end = src.find ( delim, start );
      tokens.push_back ( src.substr ( start, end - start ) );
      if ( end == std::string::npos )
        break;
      start = end + delim.size();
    }
    _tokens.swap ( tokens );
  }
};
