#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"
#include<iostream>
#include<filesystem>

namespace fs = std::filesystem;


int main() 
{
    std::string FName;
    std::string path = "data/Pi_Day";
    for (const auto & entry : fs::directory_iterator(path))
    {
        FName = entry.path();
        std::string tmp = FName;
        TFile* f1= TFile::Open(tmp.c_str(), "open" ); 
    
        TTree* tree = (TTree*)f1->Get("tree");
        TH1D* h1_charge = new TH1D("charge", "", 100, -1000., 0.);

        float charge;
        tree->SetBranchAddress( "charge", &charge );
        int nentries = tree->GetEntries();

        for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {
            tree->GetEntry(iEntry);
            h1_charge->Fill(charge);
        }
        std::cout<<"Hello";

        TCanvas* c1 = new TCanvas("c1", "", 600, 600);
        h1_charge->Draw();
/*
        std::string HName = "Hist_";

        c1->SaveAs(HName.append(FName).c_str());
        */

        f1->Close();
    }
    return 0;
}