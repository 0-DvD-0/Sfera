#include<iostream>
#include<filesystem>
#include<typeinfo>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"

namespace fs = std::filesystem;

int main() 
{
    std::string pth = "data/Pi_Day";
    for (const auto & entry : fs::directory_iterator(pth))
    {
        TFile* f1= TFile::Open(entry.path().c_str(), "READ" ); 

        TTree* tree = (TTree*)f1->Get("tree");
        TH1D* h1_charge = new TH1D("charge", "", 100, -1000., 0.);

        float charge;
        tree->SetBranchAddress("charge", &charge);
        int nentries = tree->GetEntries();

        for(unsigned iEntry=0; iEntry<nentries; ++iEntry ) {
            tree->GetEntry(iEntry);
            h1_charge->Fill(charge);
        }

        TCanvas* c1 = new TCanvas("c1", "", 600, 600);
        c1->cd();
        h1_charge->Draw();

        std::string hst = "ist_";

        c1->SaveAs((hst + entry.path().filename().string() + ".pdf").c_str());

        tree->Delete();
        f1->Close();

    }


    


    return 0;
}