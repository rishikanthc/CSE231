#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include "map"

using namespace llvm;
using namespace std;



namespace {
struct cse231_cdi: public FunctionPass {
	static char ID;
	int temp;
	string name;
	cse231_cdi() : FunctionPass(ID) {}

	bool runOnFunction(Function &F) override {
		map<string, int> count;
		for(auto& B : F) {
			for(auto& I : B) {
				name = I.getOpcodeName();
				if(count.find(name) == count.end()) {
					count.insert(std::pair<string, int>(name,1));
				} else {
					temp = count[name];
					count[name] = temp + 1;
				}
			}
		}
    		for (auto iter = count.begin(); iter != count.end(); iter++)
			errs() << iter->first << '\t' << iter->second << "\n";

		return false;
	}
}; // end of struct cse231_cdi
}  // end of anonymous namespace

char cse231_cdi::ID = 0;
static RegisterPass<cse231_cdi> X("cse231-cdi", "Developed to test LLVM and docker",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
