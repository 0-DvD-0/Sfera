#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <stdlib.h>

namespace fs = std::filesystem;

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}




int main( int argc, char* argv[] ) {

  if( argc != 2 ) {

    std::cout << "USAGE: ./asciiToTree [FileDirectory]" << std::endl;
    exit(1);

  }

  std::string Directory_s = argv[1];
  std::string Extension = ".dat";
  std::string OutDir = Directory_s+"RootFile";
  
  const fs::path Dir{Directory_s};
  fs::create_directories(OutDir);

  for (auto const& file : fs::directory_iterator{Dir}){

    if (Extension==file.path().extension())
    {
      std::string fileName(file.path());
      std::ifstream fs(Form("%s", fileName.c_str()));

      if( !fs.good() ) {
      std::cout << "-> No file called '" << fileName << "Exists." << std::endl;
         exit(1);
      }


      std::cout << "-> Opened ascii data file: " << file.path() << std::endl;

      fs::path ext = ".root";
      fs::path path = file.path();
      std::string Fn = path.replace_extension(ext).filename();
      std::string outfileName = OutDir+"/"+Fn;

      TFile* outfile = TFile::Open( outfileName.c_str(), "recreate" );
      TTree* tree = new TTree( "tree", "" );


      int ev=-1;
      int nch;
      int   ch       [128];
      float base     [128];
      float amp      [128];
      float charge   [128];
      float letime   [128];
      float tetime   [128];
      float ratecount[128];
      float pshape   [128][1024];

      tree->Branch( "ev"       , &ev      , "ev/I"            );
      tree->Branch( "nch"      , &nch     , "nch/I"           );
      tree->Branch( "ch"       , ch       , "ch[nch]/I"     );
      tree->Branch( "base"     , base     , "base[nch]/F"     );
      tree->Branch( "amp"      , amp      , "amp[nch]/F"      );
      tree->Branch( "charge"   , charge   , "charge[nch]/F"   );
      tree->Branch( "letime"   , letime   , "letime[nch]/F"   );
      tree->Branch( "tetime"   , tetime   , "tetime[nch]/F"   );
      tree->Branch( "ratecount", ratecount, "ratecount[nch]/F");
      tree->Branch( "pshape"   , pshape   , "pshape[nch][1024]/F");


      std::string line;
      bool wasReadingEvent = false;
      bool readyForPulseShape = false;



      if( fs.good() ) {

        std::cout << "-> Starting parsing file." << std::endl;
        nch=0;
        while( getline(fs,line) ) {

          std::string delimiter = " ";
          size_t pos = 0;
          std::vector<std::string> words;
          std::string word;

          while((pos = line.find("  ")) != std::string::npos) {
            replace(line,"  ", delimiter);
          }
          
          while ((pos = line.find(delimiter)) != std::string::npos) {
            word = line.substr(0, pos);
            line.erase(0, pos + delimiter.length());
            words.push_back(word);
          }

          if( words[0]=="===" && words[1]=="EVENT" && wasReadingEvent ) {

            if( ev % 100 == 0 ) std::cout << "   ... analyzing event: " << ev << std::endl;

            tree->Fill();
    
            nch = 0;
            wasReadingEvent = false;

          } else if( words[0]=="===" && words[1]=="CH:" ) {

            wasReadingEvent = true;
            readyForPulseShape = true;

            ch       [nch] = atoi(words[2].c_str());
            base     [nch] = atof(words[8].c_str());
            amp      [nch] = atof(words[11].c_str());
            charge   [nch] = atof(words[14].c_str());
            letime   [nch] = atof(words[17].c_str());
            tetime   [nch] = atof(words[20].c_str());
            ratecount[nch] = atof(words[23].c_str());

            nch += 1;


          } else if( readyForPulseShape ) {
          //} else if( readyForPulseShape && ch[nch]>=0 ) {
      
            for( unsigned i=0; i<words.size(); ++i ) 
              pshape[nch-1][i] = atof(words[i].c_str());

            readyForPulseShape = false;
      
          }

          if( words[0]=="===" && words[1]=="EVENT" && wasReadingEvent==false) {
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
  return 0;

}


