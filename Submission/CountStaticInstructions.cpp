#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include "map"

using namespace llvm;
using namespace std;



namespace {
struct cse231_csi: public FunctionPass {
  static char ID;
  cse231_csi() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    map<string, int> count;

    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
	    //int temp;
	    string name = I->getOpcodeName();
        count[name]++;
	    /*if(count.find(name) == count.end()) {
		    count.insert(std::pair<string, int>(name, 1));
	    } else {
		    temp = count[name];
		    count[name] = temp + 1;
	    }*/

    }
    for (auto iter = count.begin(); iter != count.end(); iter++)
	    errs() << iter->first << '\t' << iter->second << "\n";

    return false;
  }
}; // end of struct cse231_csi
}  // end of anonymous namespace

char cse231_csi::ID = 0;
static RegisterPass<cse231_csi> X("cse231-csi", "Developed to test LLVM and docker",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
