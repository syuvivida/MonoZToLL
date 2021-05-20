#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <TH1D.h>
#include <TFile.h>
#include "untuplizer.h"
#include <TClonesArray.h>
#include <TLorentzVector.h>
using namespace std;

void leptonnuber()
{
    ifstream inputtxtFile("../ntuple_filelist/signal/DarkMatter_MonoZToLL_NLO_Vector_Mx2-150_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_Mx1-1_ctau-1_13TeV-madgraph.txt");
    string inputFile;
    Long64_t NNTotal=0;
    Int_t elenumbers;
    Int_t munumbers;
    Int_t taunumbers;
    TTree *T = new TTree("some_variable", "some_variable");
    T->Branch("elenumbers", &elenumbers, "elenumbers/I");
    T->Branch("munumbers", &munumbers, "munumbers/I");
    T->Branch("taunumbers", &taunumbers, "taunumbers/I");
    TH1D *elenumb = new TH1D("elenumb", "ee",5,0,5);
    TH1D *munumb = new TH1D("munumb", "mu",5,0,5);
    TH1D *taunumb = new TH1D("taunumb", "tau",5,0,5);
    while(getline(inputtxtFile,inputFile))
    {
        TreeReader data(inputFile.data());
        for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++)
        {
            NNTotal++;
            data.GetEntry(jEntry);
            // 0. check the generator-level information and make sure there is a Z->e+e-
            Int_t nGenPar        = data.GetInt("nGenPar");
            TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");
            Int_t* genParId      = data.GetPtrInt("genParId");
            Int_t* genParSt      = data.GetPtrInt("genParSt");
            Int_t* genMomParId   = data.GetPtrInt("genMomParId");
            bool findEle = false;
            bool findmu = false;
            bool findtau = false;
            vector<TLorentzVector*> myEles;
            vector<TLorentzVector*> myMu;
            vector<TLorentzVector*> myTau;
            for(int ig=0; ig < nGenPar; ig++)
            {
                TLorentzVector* thisGen = (TLorentzVector*)genParP4->At(ig);
                int pid=genParId[ig];
                int mompid = genMomParId[ig];
                int status = genParSt[ig];
                if(abs(pid)==11 && mompid==23)
                {
                    findEle=true;
                    myEles.push_back(thisGen);
                }
                else if(abs(pid)==13 && mompid==23)
                {
                    findmu=true; 
                    myMu.push_back(thisGen);
                }
                else if(abs(pid)==15 && mompid==23)
                {
                    findtau=true;
                    myTau.push_back(thisGen);
                }
            }
            elenumb->Fill(myEles.size());
            munumb->Fill(myMu.size());
            taunumb->Fill(myTau.size());
            elenumbers=myEles.size();
            munumbers=myMu.size();
            taunumbers=myTau.size();
            T->Fill();
            //cout<<"myEles size"<<myEles.size()<<endl;
            //cout<<"myMu size"<<myMu.size()<<endl;
            //cout<<"myTau size"<<myTau.size()<<endl;

        }
        cout<<"NNTotal ="<<NNTotal<<endl;
        cout<<"End of file"<<endl;
    }

    string outputfile = "../output/lepton_result.root";
    // out Tree branches
	TFile* fout = new TFile(outputfile.c_str(), "RECREATE" );
    fout->Write();
    elenumb->Write();
    munumb->Write();
    taunumb->Write();
    T->Write();
    fout->Close();
}