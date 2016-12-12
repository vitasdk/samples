#include <psp2/kernel/processmgr.h>
#include <sstream>
#include <vector>

#include <cstdio>

int main(int argc, char *argv[]) {
	std::stringstream output;
	std::vector<std::string> hello = { "Hello" };
	hello.push_back(",");
	hello.push_back(" C++ ");
	hello.push_back("world!");
	for (auto &s : hello) {
		// std::cout does't work ATM :(
		output << s;
	}
	output << std::endl;
	printf("%s\n", output.str().c_str());
	sceKernelExitProcess(0);
    return 0;
}
