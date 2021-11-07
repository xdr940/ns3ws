#include <vector>
#include <set>
#include <string>
#include <map>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <cinttypes>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>

int main(){

    std::cout<<"at my0"<<std::endl;

    std::map<std::string,int64_t> time_dims;
    time_dims["s"]=1e9;
    time_dims["ms"]=1e6;
    time_dims["us"]=1e3;
    time_dims["ns"]=1;

    std::map<std::string,int64_t>::iterator it;


    for ( it = time_dims.begin(); it != time_dims.end(); it++ ) {
        // if( dim.compare(it->c_str())==0){
            
        // }
        std::cout <<"tt ";
    }

  return 0;
}