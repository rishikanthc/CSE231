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
struct cse231_cdi: public FunctionPass {
	static char ID;
	int temp;
	int name;
	cse231_cdi() : FunctionPass(ID) {}

	bool runOnFunction(Function &F) override {

		Module* m = F.getParent();
		LLVMContext &fctx = F.getContext();

		/* protoype for updateInstrInfo */
		std::vector<Type*> paramTypes = {Type::getInt32Ty(fctx), Type::getInt32PtrTy(fctx), Type::getInt32PtrTy(fctx)};
		Type *retType = Type::getVoidTy(fctx);
		FunctionType *TestFuncType = FunctionType::get(retType, paramTypes, false);
		Constant *updateStats = m->getOrInsertFunction("updateInstrInfo", TestFuncType);
		
		/* prototype for printOutInstrInfo */
		Type *pretType = Type::getVoidTy(fctx);
		FunctionType *pfunctype= FunctionType::get(pretType, false);
		Constant *printStats = m->getOrInsertFunction("printOutInstrInfo", pfunctype);

		/* Iterate over basic block */
		for(auto& B : F) {

			map<int, int> count;
			
			for(auto& I : B) {
				name = I.getOpcode();
				count[name]++;
			}

			/* Initialize IR Builder and insertpoint */
			IRBuilder<> builder(&B);
			builder.SetInsertPoint(B.getTerminator());

			vector<Value*>args;
			vector<Constant*> opcodes;
			vector<Constant*> opcounts;

			for (auto iter = count.begin(); iter != count.end(); iter++) {
				opcodes.push_back(ConstantInt::get(Type::getInt32Ty(fctx), iter->first));
				opcounts.push_back(ConstantInt::get(Type::getInt32Ty(fctx), iter->second));
			}

			ArrayType *arrayTy = ArrayType::get(Type::getInt32Ty(fctx), count.size());
			Value* ranges[2] = {ConstantInt::get(Type::getInt32Ty(fctx), 0), ConstantInt::get(Type::getInt32Ty(fctx), 0)};

			GlobalVariable *opcodes_global = new GlobalVariable(*m, arrayTy, true, 
					GlobalVariable::InternalLinkage, ConstantArray::get(arrayTy, opcodes), "opcodes_global");
                   	GlobalVariable *opcounts_global = new GlobalVariable(*m, ArrayType::get(Type::getInt32Ty(fctx), count.size()), true, 
					GlobalVariable::InternalLinkage, ConstantArray::get(arrayTy, opcounts), "opcounts_global");

			args.push_back(ConstantInt::get(Type::getInt32Ty(fctx), count.size()));
			args.push_back(builder.CreateInBoundsGEP(opcodes_global, ranges));
			args.push_back(builder.CreateInBoundsGEP(opcounts_global, ranges));

			builder.CreateCall(updateStats, args);		

			if ((string) B.getTerminator()->getOpcodeName() == "ret") {
				builder.SetInsertPoint(B.getTerminator());
				builder.CreateCall(printStats);
			}


		}

		return false;
	}
}; // end of struct cse231_cdi
}  // end of anonymous namespace

char cse231_cdi::ID = 0;
static RegisterPass<cse231_cdi> X("cse231-cdi", "Developed to test LLVM and docker",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
