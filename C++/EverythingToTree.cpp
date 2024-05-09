#include "TFile.h"
#include "TTree.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <sys/stat.h>
//include<windows.h>

namespace fs = std::filesystem;

bool isNumber(const std::string& s);
void Parse(fs::path path);


int main( int argc, char* argv[] ) {

  if( argc != 2 ) {

    std::cout << "USAGE: ./EverythingToTree [Folder]    -To convert every file inside the target folder\n
                  USAGE: ./EverythingToTree [File Name] -To convert a single file\n\n 
                  The output is stored in ../Dati" << std::endl;
    exit(1);

  }

  std::string Input = argv[1];

  if (Input.rfind(".dat")!=std::string::npos)
  { 
    const fs::path fpath{Input};
    Parse(fpath);
  }

  else if (std::filesystem::is_directory(Input))
  {
    const fs::path Dir{Input};

    for (auto const& file : fs::directory_iterator{Dir}){

      Parse(file.path());
    
    }
  }else{

    cout<<"Bad Input path :("<<endl;

  }
  
  return 0;

}



bool isNumber(const std::string& s) {

  std::string::const_iterator it = s.begin();
  while (it != s.end() && (std::isdigit(*it) || (*it)==std::string(".") || (*it)==std::string("-")) ) ++it;
  return !s.empty() && it == s.end();

}

void Parse(fs::path path){
    
    
    std::string Extension = ".dat";
    std::string OutDir = "../Dati";
    std::string fileName(path);
    std::string Myname = path.filename();
    std::size_t dPos = Myname.rfind(Extension);

    bool IsAscii = (fileName.rfind("Ascii.dat")!=std::string::npos)?true:false;
    
    if (dPos!=std::string::npos){
    
      std::ifstream fs(Form("%s", fileName.c_str()));
      if( !fs.good() ) {
        std::cout << "-> No file called '" << fileName << "found. Exiting." << std::endl;
        exit(1);
      }
      if(IsAscii){
        
        std::cout << "-> Opened ascii data file: " << file.path() << std::endl;
      
      }else{
        std::cout << "-> Opened measurements-only data file: " << fileName << std::endl;
      }

      std::string Fn;
      
      Fn = (dPos+Extension.size()==Myname.size())?Myname.erase(dPos,Extension.size())+"_0000.root": Myname.erase(dPos,Extension.size())+".root";
      std::string outfileName = OutDir+"/"+Fn;
      
      TFile* outfile = TFile::Open( outfileName.c_str(), "recreate" );
      TTree* tree = new TTree( "tree", "" );


      int ev;
      int nch;
      int   ch       [128];
      float base     [128];
      float amp      [128];
      float charge   [128];
      float letime   [128];
      float tetime   [128];
      float pshape   [128][1024];

      tree->Branch( "ev"       , &ev      , "ev/I"            );
      tree->Branch( "nch"      , &nch     , "nch/I"           );
      tree->Branch( "ch"       , ch       , "ch[nch]/I"       );
      tree->Branch( "base"     , base     , "base[nch]/F"     );
      tree->Branch( "amp"      , amp      , "amp[nch]/F"      );
      tree->Branch( "charge"   , charge   , "charge[nch]/F"   );
      tree->Branch( "letime"   , letime   , "letime[nch]/F"   );
      tree->Branch( "tetime"   , tetime   , "tetime[nch]/F"   );
      
      if(IsAscii){
        tree->Branch( "pshape"   , pshape   , "pshape[nch][1024]/F");
      }//tree->Branch( "ratecount", ratecount, "ratecount[nch]/F");


      std::string line;
      bool wasReadingEvent = false;
      bool readyForPulseShape = false;


      if( fs.good() ) {

        std::cout << "-> Starting parsing file." << std::endl;
        nch=0;

        while( getline(fs,line) ) {

          //std::cout << line << std::endl;
          line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

          std::string delimiter = " ";
          size_t pos = 0;
          std::vector<std::string> words;
          std::string word;
          while ((pos = line.find(delimiter)) != std::string::npos) {
            word = line.substr(0, pos);
            line.erase(0, pos + delimiter.length());
            words.push_back(word);
          }

          std::vector< std::string > words_cleaned;
          for( unsigned i=0; i<words.size(); ++i ) {
            if( isNumber(words[i]) ) words_cleaned.push_back( words[i] );
          }

          if (words.size()==0) continue; // protect from truncated data-taking 
          
          if( words[0]=="===" && words[1]=="Event" && wasReadingEvent ) {

            if( ev % 1000 == 0 ) std::cout << "   ... analyzing event: " << ev << std::endl;

            tree->Fill();
    
            nch = 0;
            wasReadingEvent = false;

          } else if( words[0]!="===" && words_cleaned.size()==7 ) {

            wasReadingEvent = true;
            readyForPulseShape = true;

            ch       [nch] = atoi(words_cleaned[0].c_str());
            base     [nch] = atof(words_cleaned[2].c_str());
            amp      [nch] = atof(words_cleaned[3].c_str());
            charge   [nch] = atof(words_cleaned[4].c_str());
            letime   [nch] = atof(words_cleaned[5].c_str());
            tetime   [nch] = atof(words_cleaned[6].c_str());
            //ratecount[ch] = atof(words_cleaned[15].c_str());

            nch += 1;

          } else if( readyForPulseShape && IsAscii) {
          //} else if( readyForPulseShape && ch[nch]>=0 ) {
      
            for( unsigned i=0; i<words.size(); ++i ) 
              pshape[nch-1][i] = atof(words[i].c_str());

            readyForPulseShape = false;
      
          }

          if( words[0]=="===" && words[1]=="Event" && wasReadingEvent==false ) {
            ev            = atoi(words[2].c_str());	
            //std::cout << ev << std::endl;
          }

        } // while get lines

      } // if file is good

      if( wasReadingEvent )
        {
          std::cout << "   ... analyzing event: " << ev << std::endl;
          tree->Fill();
        }

      fs.close();

      tree->Write();
      outfile->Close();

      std::cout << "-> Tree saved in: " << outfile->GetName() << std::endl;
      }

}
