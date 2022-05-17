#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>

std::string exec(const char* cmd);
int NewPID();

int main(){
	
	int killerPID =	NewPID();
	if(killerPID > 0){
		std::cout << "killerPID is: " << killerPID << std::endl;
	}
	return 0;
}

std::string exec(const char* cmd){
	std::array<char, 128> buffer;
	std::string result;
	std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
	while(!feof(pipe.get())){
		if(fgets(buffer.data(), 128, pipe.get()) != nullptr)
			result += buffer.data();
	}
	return result;
}

int NewPID(){
	std::string read = exec("ps aux");
	std::cout << read << std::endl;

	std::stringstream check1(read);
	std::string tmp;
	std::vector <std::string> tokens;
	
	while(getline(check1,tmp, '\n'))
	{
		std::stringstream check2(tmp);
		std::string tmp2;
		copy(std::istream_iterator<std::string>(check2),
		     std::istream_iterator<std::string>(),
		     std::back_inserter(tokens));
		
		for(int i = 0; i < tokens.size(); i++){
			std::cout << i << tokens[i] << std::endl;
			if(tokens[i] == "gnuplot"){
				std::cout << "Winner: " << tokens[1] << std::endl;
				int T = std::stoi(tokens[1]);
				std::cout << "Token 1 as int is: " << T << std::endl;
				return T;
			}
		}
		tokens.clear();
		/*while(getline(check2, tmp2, ' '))
				{	
					if(tmp2 == " "){}
					else{
					tokens.push_back(tmp2);
					}
					std::cout << tmp2 << std::endl;
					if(tmp2 == "gnuplot"){
						std::cout << "winner" << std::endl;
						std::cout << "token is: " << tokens[1] << std::endl;
					//
						//return tokens[1];
					}
				}
				tokens.clear();
		
		//if tmp = "gnu" get pid and exit this.*/
	}
	return 0;
}
