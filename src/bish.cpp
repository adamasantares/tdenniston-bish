#include <stdio.h>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <set>
#include <string>
#include <iostream>
#include <unistd.h>
#include "Compile.h"
#include "Parser.h"
#include "CodeGen.h"


/**
 * @brief Runs transcompiled code in a shell app
 * @param shellApp shell app name (bash)
 * @param stream transcompiled code stream
 * @param args
 * @return 
 */
int executeCode(const std::string &shellApp, std::istream &stream, const std::string &args) {
    // Must pass the -s parameter to bash to set the positional
    // parameters to 'args'.  The '--' disables any of the arguments
    // from being treated as arguments to the shell.
    std::string command = shellApp + " -s -- " + args;
    FILE *process = popen(command.c_str(), "w");
    char buffer[4096];
    do {
        stream.read(buffer, sizeof(buffer));
        fwrite(buffer, 1, stream.gcount(), process);
    } while (stream.gcount() > 0);
    fflush(process);
    // pclose returns the exit status of the process,
    // but shifted to the left by 8 bits.
    int e = pclose(process) >> 8;
    return e;
}


void usageInfo(char *argv0) {
    std::cerr << "USAGE: " << argv0 << " [-r] <INPUT> [<args>]\n";
    std::cerr << "  Compiles Bish file <INPUT> to bash. Specifying '-' for <INPUT>\n";
    std::cerr << "  reads from standard input.\n";
    std::cerr << "\nOPTIONS:\n";
    std::cerr << "  -h: Displays this help message.\n";
    std::cerr << "  -r: Compiles and runs the script.\n";
    std::cerr << "  <ARGS>: With -r, passes <ARGS> as arguments to script.\n";
    std::cerr << "  -l: list all code generators.\n";
    std::cerr << "  -u <NAME>: use code generator <NAME>.\n";
}


void showGeneratorsList() {
    const Bish::CodeGenerators::CodeGeneratorsMap &cg_map = Bish::CodeGenerators::all();
    for (Bish::CodeGenerators::CodeGeneratorsMap::const_iterator it = cg_map.begin();
         it != cg_map.end(); ++it) {
        std::cout << it->first << std::endl;
    }
}


int main(int argc, char **argv) {
    Bish::CodeGenerators::initialize();

    int c;
    bool runAfterCompile = false;
    std::string codeGeneratorName = "bash";

    while ((c = getopt(argc, argv, "hrlu:")) != -1) {
        switch (c) {
			case 'h':
				usageInfo(argv[0]);
				return 0;
			case 'r':
				runAfterCompile = true;
				break;
			case 'l':
				showGeneratorsList();
				return 1;
			case 'u':
				codeGeneratorName = std::string(optarg);
				break;
        }
    }

    if (optind >= argc) {
        usageInfo(argv[0]);
        return 1;
    }

    std::string path(argv[optind]);
    std::stringstream s;
    Bish::Parser p;
    Bish::Module *m = path.compare("-") == 0 ? p.parse(std::cin) : p.parse(path);

    std::string args;
    if (optind + 1 < argc) {
        if (!runAfterCompile) {
            std::cerr << "Can't pass arguments to script without -r.\n";
            return 1;
        }
        for (unsigned i = optind+1; i < argc; i++) {
            args += argv[i];
            args += " ";
        }
    }
    
    Bish::CodeGenerators::CodeGeneratorConstructor cg_constructor =
        Bish::CodeGenerators::get(codeGeneratorName);
    if (cg_constructor == NULL) {
        std::cerr << "No code generator " << codeGeneratorName << std::endl;
        return 1;
    }
	
    Bish::CodeGenerator *cg = cg_constructor(runAfterCompile ? s : std::cout);
    Bish::compile(m, cg);
	
    if (runAfterCompile) {
        exit( executeCode(codeGeneratorName, s, args) );
    }
    return 0;
}
