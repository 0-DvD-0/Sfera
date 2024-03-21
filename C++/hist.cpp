#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

// ROOT header files
#include "TH1D.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TStyle.h"
#include "THStack.h" 

int main(){
   
    
   
    
    TString fileName= "../data/Run_ch3_Data_3_14_2024_Ascii.root";
    
    TFile* file = TFile::Open(fileName);
    TTree* tree = (TTree*)file->Get("tree");
    if( !file->IsOpen() ) {
        std::cout << "problems opening root file: exiting... " << std::endl;
        exit(-1);
    }
    std::cout << "Reading data from root file " << fileName << std::endl;
    
    int nentries = tree->GetEntries();
    float base[5000], amp[5000], charge[5000], ratecount[5000], tetime[5000], letime[5000];
    float pshape[5000];//[1024];
    int nch;
    tree->SetBranchAddress("base", base);      // [3]
    tree->SetBranchAddress("nch", &nch);     // [3]
    tree->SetBranchAddress("charge", charge); // [3]
    tree->SetBranchAddress("amp", amp); // [3]
    tree->SetBranchAddress("ratecount", ratecount);      // [3]
    tree->SetBranchAddress("tetime", tetime);     // [3]
    tree->SetBranchAddress("letime", letime);
    tree->Scan("base");
    tree->Print();
    TH1D h_charge ("h_charge", "h_charge", 1000, -10, 1000 );
    TH1D h_amp ("amp", "amp", 1000, -2.0, 2.0);
    TH1D h_rate ("ratecount", "ratecount", 1000, 0, 2000);
    TH1D h_te ("tetime", "tetime", 1000, 0, 1024);
    TH1D h_le ("letime", "letime", 1000, 0, 1024);
    TH1D h_base ("base", "base", 1000, -0.3, 0.3);
   
    for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {
        tree->GetEntry(iEntry);
        h_charge.Fill(fabs(charge[0]));
        h_amp.Fill(amp[0]);
        h_rate.Fill(ratecount[0]);
        h_te.Fill(tetime[0]);
        h_le.Fill(letime[0]);
        h_base.Fill(base[0]);
        
    } // for entries
       
        
     
    file->Close();
        
   
    gStyle->SetOptStat(111111);

    TCanvas canv("canv", "canvas for plotting", 2000, 2000);
    canv.Divide(3,3);
    canv.cd(1);
    h_charge.GetXaxis()->SetTitle("charge");
    h_charge.GetYaxis()->SetTitle("events");
    h_charge.Draw("");
    cout << "here" << endl;
    canv.cd(2);
    h_amp.GetXaxis()->SetTitle("amp");
    h_amp.GetYaxis()->SetTitle("events");
    h_amp.Draw("");
    canv.cd(3);
    h_rate.GetXaxis()->SetTitle("rate");
    h_rate.GetYaxis()->SetTitle("events");
    h_rate.Draw("");
    canv.cd(4);
    h_te.GetXaxis()->SetTitle("te");
    h_te.GetYaxis()->SetTitle("events");
    h_te.Draw("");
    canv.cd(5);
    h_le.GetXaxis()->SetTitle("le");
    h_le.GetYaxis()->SetTitle("events");
    h_le.Draw("");
    canv.cd(6);
    h_base.GetXaxis()->SetTitle("base");
    h_base.GetYaxis()->SetTitle("events");
    h_base.Draw("");

  
    canv.SaveAs("./charge_ch3.pdf");
    
    

    return 0;
}