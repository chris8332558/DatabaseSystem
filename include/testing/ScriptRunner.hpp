//
//  ScriptRunner.hpp
//  Database
//
//  Created by rick gessner on 3/30/23.
//  Copyright © 2018-2023 rick gessner. All rights reserved.
//


#ifndef ScriptRunner_h
#define ScriptRunner_h

#include <fstream>
#include <sstream>
#include <string>
#include "controller/AppController.hpp"
#include "misc/Remoting.hpp"

namespace ECE141 {

  class ScriptRunner {
  public:
      char semicolon = ';';
    ScriptRunner(AppController &anApp): app(anApp) {}
      
    std::string readCommand(std::istream &anInput) {
      std::string theResult;
      char theChar{0};
      while(semicolon!=theChar && !anInput.eof()) {
        anInput >> theChar;
        if(0==theChar) break;
        theResult+=theChar;
      }
      
      static const char* typeOfWhitespaces = " \t\n\r\f\v";
      theResult.erase(theResult.find_last_not_of(typeOfWhitespaces) + 1);
      theResult.erase(0,theResult.find_first_not_of(typeOfWhitespaces));

      return theResult;
    }
        
    StatusResult run(std::istream &anInput,std::ostream &anOutput,
                     size_t aMaxErrors=1) {
      StatusResult theResult;
      
      auto theTimer=Config::getTimer();
      auto theStart=theTimer.started();

      DBConnector connector{Config::getConnectionString()};

      while(theResult || aMaxErrors) {
        std::string theCommand(readCommand(anInput));

        if(theCommand.length()) {
          std::stringstream theStream(theCommand);
          anOutput << theCommand << "\n";
          theResult=connector.send(theStream.str(), [&](View &aView) {
              aView.show(anOutput);
              anOutput << std::endl;
          });
//          theResult=app.handleInput(theStream, [&](View &aView) {
//            aView.show(anOutput);
//            anOutput << std::endl;
//          });
          if(!theResult) aMaxErrors--;
        }
        else break;
      }
      
      anOutput << "Script Elapsed: " << std::fixed << std::setprecision(5)
        << theTimer.elapsed(theStart) << " secs\n";

      return theResult;
    }
    
  protected:
    AppController &app;
  };

}

#endif /* ScriptRunner_h */
