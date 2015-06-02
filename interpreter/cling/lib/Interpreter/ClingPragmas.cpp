//--------------------------------------------------------------------*- C++ -*-
// CLING - the C++ LLVM-based InterpreterG :)
// author:  Axel Naumann <axel@cern.ch>
//
// This file is dual-licensed: you can choose to license it under the University
// of Illinois Open Source License or the GNU Lesser General Public License. See
// LICENSE.TXT for details.
//------------------------------------------------------------------------------

#include "ClingPragmas.h"

#include "cling/Interpreter/Interpreter.h"

#include "clang/AST/ASTContext.h"
#include "clang/Basic/TokenKinds.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/Token.h"
#include "clang/Parse/Parser.h"

using namespace cling;
using namespace clang;

namespace {
    static void replaceEnvVars(std::string &Path){

      std::size_t bpos = Path.find("$");

      while (bpos != std::string::npos) {
        std::size_t spos = Path.find("/", bpos + 1);
        std::size_t length = Path.length();

      if (spos != std::string::npos) // if we found a "/"
        length = spos - bpos;

      std::string envVar = Path.substr(bpos + 1, length -1); //"HOME"
      const char* c_Path = getenv(envVar.c_str());
      std::string fullPath;
      if (c_Path != NULL){
        fullPath = std::string(c_Path);
      }
      Path.replace(bpos, length, fullPath);
      bpos = Path.find("$", bpos + 1); //search for next env variable
    }
  }

  static std::pair<bool, std::string> HandlePragmaHelper(Preprocessor &PP,
                                        Token &FirstToken,
                                        Interpreter &m_Interp,
                                        const std::string &pragmaInst){
    std::pair<bool, std::string> Diagnostics (true, "");
    struct SkipToEOD_t {
      Preprocessor& m_PP;
      SkipToEOD_t(Preprocessor& PP): m_PP(PP) {}
      ~SkipToEOD_t() { m_PP.DiscardUntilEndOfDirective(); }
    } SkipToEOD(PP);

    Token Tok;
    PP.Lex(Tok);
    if (Tok.isNot(tok::l_paren)) {
      Diagnostics.first = false;
      Diagnostics.second = std::string("cling:HandlePragmaHelper : expect '(' after #" + pragmaInst);
      return Diagnostics;
    }
    std::string Literal;
    if (!PP.LexStringLiteral(Tok, Literal, pragmaInst.c_str(),false /*allowMacroExpansion*/)) {
      // already diagnosed.
      Diagnostics.first = false;
      Diagnostics.second =  "LexStringLiteralError";
      return Diagnostics;
    }
    if((pragmaInst == "pragma cling add_include_path") || (pragmaInst == "pragma cling add_library_path")){
      { if (Literal.find("$") != std::string::npos)
          replaceEnvVars(Literal);
      }
    }
    clang::Parser& P = m_Interp.getParser();
    Parser::ParserCurTokRestoreRAII savedCurToken(P);
    // After we have saved the token reset the current one to something which
    // is safe (semi colon usually means empty decl)
    Token& CurTok = const_cast<Token&>(P.getCurToken());
    CurTok.setKind(tok::semi);

    if(!Literal.empty()){
      Diagnostics.first = true;
      Diagnostics.second = Literal;
    }
    else{
      Diagnostics.first = false;
      Diagnostics.second = "empty-literal";
    }
    return Diagnostics;
  }

  class PHLoad: public PragmaHandler {
    Interpreter& m_Interp;

  public:
    PHLoad(Interpreter& interp):
      PragmaHandler("load"), m_Interp(interp) {}

    void HandlePragma(Preprocessor &PP,
                      PragmaIntroducerKind Introducer,
                      Token &FirstToken) override {
      // TODO: use Diagnostics!
     // std::pair<bool, std::string> Diagnostics = HandlePragmaHelper(PP, FirstToken, m_Interp, "pragma cling load");

std::string pragmaInst = std::string("pragma cling load");
std::pair<bool, std::string> Diagnostics (true, "");





    struct SkipToEOD_t {
      Preprocessor& m_PP;
      SkipToEOD_t(Preprocessor& PP): m_PP(PP) {}
      ~SkipToEOD_t() { m_PP.DiscardUntilEndOfDirective(); }
    } SkipToEOD(PP);

    Token Tok;
    PP.Lex(Tok);
    if (Tok.isNot(tok::l_paren)) {
      Diagnostics.first = false;
      Diagnostics.second = std::string("cling:HandlePragmaHelper : expect '(' after #" + pragmaInst);
      //return Diagnostics;
    }
    std::string Literal;
    if (!PP.LexStringLiteral(Tok, Literal, pragmaInst.c_str(),false /*allowMacroExpansion*/)) {
      // already diagnosed.
      Diagnostics.first = false;
      Diagnostics.second =  "LexStringLiteralError";
      //return Diagnostics;
    }
    if((pragmaInst == "pragma cling add_include_path") || (pragmaInst == "pragma cling add_library_path")){
      { if (Literal.find("$") != std::string::npos)
          replaceEnvVars(Literal);
      }
    }
    clang::Parser& P = m_Interp.getParser();
    Parser::ParserCurTokRestoreRAII savedCurToken(P);
    // After we have saved the token reset the current one to something which
    // is safe (semi colon usually means empty decl)
    Token& CurTok = const_cast<Token&>(P.getCurToken());
    CurTok.setKind(tok::semi);

    if(!Literal.empty()){
      Diagnostics.first = true;
      Diagnostics.second = Literal;
    }
    else{
      Diagnostics.first = false;
      Diagnostics.second = "empty-literal";
    }





      if (Diagnostics.first == false){
        if (Diagnostics.second == "empty-literal"){
          llvm::errs() << "Cannot load unnamed files.\n" ;
          return;
        }else{
          llvm::errs() << Diagnostics.second;
        }
      }
      Preprocessor::CleanupAndRestoreCacheRAII cleanupRAII(PP);
      // We can't PushDeclContext, because we go up and the routine that pops
      // the DeclContext assumes that we drill down always.
      // We have to be on the global context. At that point we are in a
      // wrapper function so the parent context must be the global.
      TranslationUnitDecl* TU = m_Interp.getCI()->getASTContext().getTranslationUnitDecl();
      Sema::ContextAndScopeRAII pushedDCAndS(m_Interp.getSema(), TU, m_Interp.getSema().TUScope);
      Interpreter::PushTransactionRAII pushedT(&m_Interp);

      m_Interp.loadFile(Diagnostics.second, true /*allowSharedLib*/);
    }
  };

  class PHAddIncPath: public PragmaHandler {
    Interpreter& m_Interp;

  public:
    PHAddIncPath(Interpreter& interp):
      PragmaHandler("add_include_path"), m_Interp(interp) {}

    void HandlePragma(Preprocessor &PP,
                      PragmaIntroducerKind Introducer,
                      Token &FirstToken) override {
      // TODO: use Diagnostics!
      std::pair<bool, std::string> Diagnostics = HandlePragmaHelper(PP, FirstToken, m_Interp, "pragma cling add_include_path");
      //if the function HandlePragmaHelper returned false,
      if (Diagnostics.first == false){
        //but not because of an empty path,
        if (Diagnostics.second != "empty-literal"){
          //print the error message returned.
          llvm::errs() << Diagnostics.second;
        }
      }else{ // if HandlePragmaHelper returned success, this means that it also returned the path to be included
         m_Interp.AddIncludePath(Diagnostics.second);
      }
    }
  };

  class PHAddLibraryPath: public PragmaHandler {
    Interpreter& m_Interp;

  public:
    PHAddLibraryPath(Interpreter& interp):
      PragmaHandler("add_library_path"), m_Interp(interp) {}

    void HandlePragma(Preprocessor &PP,
                      PragmaIntroducerKind Introducer,
                      Token &FirstToken) override {
      // TODO: use Diagnostics!
      std::pair<bool, std::string> Diagnostics = HandlePragmaHelper(PP,FirstToken, m_Interp,"pragma cling add_library_path");
      //if the function HandlePragmaHelper returned false,
      if (Diagnostics.first == false){
        //but not because of an empty path,
        if (Diagnostics.second != "empty-literal"){
          //print the error message returned.
          llvm::errs() << Diagnostics.second;
        }
      }else{ // if HandlePragmaHelper returned success, this means that it also returned the path to be included
        InvocationOptions& Opts = m_Interp.getOptions();
        Opts.LibSearchPath.push_back(Diagnostics.second);
      }
    }
  };
}

void cling::addClingPragmas(Interpreter& interp) {
  Preprocessor& PP = interp.getCI()->getPreprocessor();
  // PragmaNamespace / PP takes ownership of sub-handlers.
  PP.AddPragmaHandler("cling", new PHLoad(interp));
  PP.AddPragmaHandler("cling", new PHAddIncPath(interp));
  PP.AddPragmaHandler("cling", new PHAddLibraryPath(interp));
}
