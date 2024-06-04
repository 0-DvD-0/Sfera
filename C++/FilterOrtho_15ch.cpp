// per compilare:
//export ROOTSYS=/snap/root-framework/931/usr/local 
//export LD_LIBRARY_PATH=$ROOTSYS/lib
//g++ -o filter FilterToTree.cpp `$ROOTSYS/bin/root-config --cflags --libs`

#include "TFile.h"
#include "TTree.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <sys/stat.h>
#include <sstream>
#include <vector>


//include<windows.h>

namespace fs = std::filesystem;
using csv = std::vector<std::vector<std::string>>; 

bool isNumber(const std::string& s);
bool IsValid(float charge[]);

csv read_csv_file(const std::string& file_path);

void printUsage();
void Parse(fs::path path);
void printCSVTable(const csv& csv_data);
void processInput(const std::string& inputPath, const std::string& filterFile);


const int  CHANNELS = 15;

float Q_1200_R[CHANNELS];
float Q_1200_L[CHANNELS]; 
float Q_511[CHANNELS]; //limite sinistro per gamma piccoli
//trigger Globale


int main(int argc, char* argv[]) {
    // Input check
    if (argc != 3) {
        printUsage();
        exit(1);
    }

    std::string inputPath = argv[1];
    std::string filterFile = argv[2];

    processInput(inputPath, filterFile);

    return 0;
}

void printUsage() {
    std::cout << "\n\tUSAGE: ./FilterToTree [Folder Path or File Path] [filter.csv]\n\n"
                 "\t-To convert every file inside the folder and filter for Orthopositronium.\n"
                 "\t-To convert a single file and filter for Orthopositronium.\n"
                 "\n\tThe output is stored in ../Dati" << std::endl;
}

void processInput(const std::string& inputPath, const std::string& filterFile) {
    csv csvData = read_csv_file(filterFile);
    printCSVTable(csvData);

    int skipRows = 1;
    for (int i = 0; i < CHANNELS; i++) {
        Q_1200_L[i] = std::stof(csvData[i + skipRows][1]);
        Q_1200_R[i] = std::stof(csvData[i + skipRows][2]);
        Q_511[i] = std::stof(csvData[i + skipRows][3]);

        //leggi il terzo array
    }

    if (inputPath.rfind(".dat") != std::string::npos) {
        const fs::path filePath{inputPath};
        Parse(filePath);
    } else if (std::filesystem::is_directory(inputPath)) {
        std::cout << "-> Opening folder: " << inputPath << std::endl;
        for (const auto& file : fs::directory_iterator{inputPath}) {
            Parse(file.path());
        }
    } else {
        std::cout << "\tBad Input path :(" << std::endl;
        exit(1);
    }
}

void printCSVTable(const csv& csv_data) {
  int cellWidth = 10;
  std::cout<<std::endl<<"   Printing Table:"<<std::endl<<std::endl;
    for (const auto& row : csv_data) {
        for (const auto& cell : row) {
            std::cout<< std::setw(cellWidth)<< cell; // Use tab as delimiter
        }
        std::cout << std::endl;
    }
    std::cout<<std::endl;
}

bool IsValid(float charge[]) {
    
    int Trigger = 0;
    int Ev = 0;

    for (int i = 0; i < CHANNELS; ++i) {
        Trigger += (charge[i] < Q_1200_R[i] && charge[i] > Q_1200_L[i]);
        Ev += (charge[i] < Q_511[i]);
    }

    return (Trigger == 1) && (Ev >= 3) && (Ev <= 5);
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
    
    bool IsAscii = (fileName.rfind("Ascii.dat")!=std::string::npos)?true:false;
    
    std::string Myname = path.filename();
    std::size_t dPos = Myname.rfind(Extension);
    std::string PEvent = (IsAscii)?"EVENT":"Event";

    int BaseIndex = (IsAscii)?3:2;

    
    if (dPos!=std::string::npos){
    
      std::ifstream fs(Form("%s", fileName.c_str()));
      if( !fs.good() ) {
        std::cout << "-> No file called '" << fileName << "found. Exiting." << std::endl;
        exit(1);
      }
      if(IsAscii){
        
        std::cout << "-> Opened ascii data file: " << fileName << std::endl;
      
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
      int   ch       [20];
      float base     [20];
      float amp      [20];
      float charge   [20];
      float letime   [20];
      float tetime   [20];
      float pshape   [20][1024];

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

        std::cout << "-->  Starting parsing file:" << std::endl;
        nch=0;
        std::istringstream iss;
        std::vector<std::string> words;
        std::vector< std::string > words_cleaned;
        std::string word;


        while( getline(fs,line) ) {

          //std::cout << line << std::endl;
          line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

          word.clear();
          words.clear();
          words_cleaned.clear();
          iss.clear();
          iss.str(line);
    
          while (std::getline(iss, word, ' ')) {

              
              if (isNumber(word)) {
                words_cleaned.push_back(word);
              }
              else {words.push_back(word);}
              
          }

          if (words.size()==0) continue; // protect from truncated data-taking 
          
          if( words[0]=="===" && (words[1]==PEvent) && wasReadingEvent ) {

            if( ev % 1000 == 0 ) std::cout << "     ... analyzing event: " << ev << std::endl;

            if(IsValid(charge)){
              tree->Fill();
            }
    
            nch = 0;
            wasReadingEvent = false;

          } else if( (words[0]!="===" && words_cleaned.size()==7) || (words[0]=="===" && words[1]=="CH:")) {
            wasReadingEvent = true;
            readyForPulseShape = true;

            ch[nch]     = std::stoi(words_cleaned[0]);
            base[nch]   = std::stof(words_cleaned[BaseIndex]);
            amp[nch]    = std::stof(words_cleaned[BaseIndex + 1]);
            charge[nch] = std::stof(words_cleaned[BaseIndex + 2]);
            letime[nch] = std::stof(words_cleaned[BaseIndex + 3]);
            tetime[nch] = std::stof(words_cleaned[BaseIndex + 4]);
            

            nch += 1;

          } else if( readyForPulseShape && IsAscii) {
          //} else if( readyForPulseShape && ch[nch]>=0 ) {
      
            for( unsigned i=0; i<words_cleaned.size(); ++i ) {
              pshape[nch-1][i] = std::stof(words_cleaned[i]);
            }
            readyForPulseShape = false;
      
          }

          if( words[0]=="===" && (words[1]==PEvent)&& wasReadingEvent==false ) {
            ev = std::stoi(words_cleaned[0]);
            //std::cout << ev << std::endl;
          }

        } // while get lines

      } // if file is good

      if( wasReadingEvent )
        {
          std::cout << "     ... analyzing event: " << ev << std::endl;
          tree->Fill();
        }

      fs.close();

      tree->Write();
      outfile->Close();

      std::cout << "-> Tree saved in: " << outfile->GetName() << std::endl;
    }

}

csv read_csv_file(const std::string& file_path) {
    std::vector<std::vector<std::string>> csv_data;
    std::ifstream file(file_path);
    if (!file) {
        std::cerr << "Error: Unable to open file " << file_path << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::istringstream iss(line);
        std::string cell;
        while (std::getline(iss, cell, ',')) {
            // Trim leading and trailing whitespaces from the cell
            cell.erase(0, cell.find_first_not_of(" \t\n\r\f\v"));
            cell.erase(cell.find_last_not_of(" \t\n\r\f\v") + 1);
            row.push_back(cell);
        }
        csv_data.push_back(row);
    }

    return csv_data;
}
