#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "map"

using namespace llvm;
using namespace std;


namespace {
struct cse231_bb: public FunctionPass {
	static char ID;
	int temp;
	int name;
	cse231_bb() : FunctionPass(ID) {}

	bool runOnFunction(Function &F) override {

		Module* m = F.getParent();
		LLVMContext &fctx = F.getContext();

		/* protoype for updateInstrInfo */
		std::vector<Type*> paramTypes = {Type::getInt1Ty(fctx)};
		Type *retType = Type::getVoidTy(fctx);
		FunctionType *TestFuncType = FunctionType::get(retType, paramTypes, false);
		Constant *updateStats = m->getOrInsertFunction("updateBranchInfo", TestFuncType);
		
		/* prototype for printOutInstrInfo */
		Type *pretType = Type::getVoidTy(fctx);
		FunctionType *pfunctype= FunctionType::get(pretType, false);
		Constant *printStats = m->getOrInsertFunction("printOutBranchInfo", pfunctype);

		/* Iterate over basic block */
		for(auto& B : F) {

			map<int, int> count;
			IRBuilder<> builder(&B);
			BranchInst *bri = dyn_cast<BranchInst>(B.getTerminator());

			builder.SetInsertPoint(B.getTerminator());

			if (bri != NULL && bri->isConditional()) {
				vector<Value*> args;
				args.push_back(bri->getCondition());
				builder.CreateCall(updateStats, args);
			} else if ((string)B.getTerminator()->getOpcodeName() == "ret") {
				builder.SetInsertPoint(B.getTerminator());
				builder.CreateCall(printStats);
			}
		}

		return false;
	}
}; // end of struct cse231_bb
}  // end of anonymous namespace

char cse231_bb::ID = 0;
static RegisterPass<cse231_bb> X("cse231-bb", "Developed to test LLVM and docker",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
