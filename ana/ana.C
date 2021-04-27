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
void efferr(float nsig,float ntotal,float factor=1)
{
  float eff = nsig/ntotal;
  float err = sqrt( (1-eff)*eff/ntotal);
  cout << "efficiency = " << eff*factor << " +- " << err*factor << endl;
}
bool pt_greater(const TLorentzVector* a, const TLorentzVector* b){
	double A = a->Pt();
    double B = b->Pt();
    return (A > B);
}
void ana(){
    TTree outTree("tree","out branches");
    //ifstream inputtxtFile("../ntuple_filelist/signal/DarkMatter_MonoZToLL_NLO_Vector_Mx2-150_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_Mx1-1_ctau-1_13TeV-madgraph.txt");
    ifstream inputtxtFile("test.txt");
    string inputFile;
    //void some variable
    Int_t elenumbers;
    Int_t munumbers;
    Int_t taunumbers;
    Long64_t NNTotal=0;
    Long64_t nPass[20]={0};
    Long64_t neeTotal=0;
    Long64_t neeeTotal=0;
    Long64_t nelePass[6]={0};
    //Create histrogram
    TH1D *Z_massee = new TH1D("Z_massee", "Z->ee", 150, 0, 150);
    TH1D *elenumb = new TH1D("elenumb", "ee",5,0,5);
    TH1D *munumb = new TH1D("munumb", "mu",5,0,5);
    TH1D *taunumb = new TH1D("taunumb", "tau",5,0,5);
    while(getline(inputtxtFile,inputFile))
    {
        //std::string inputFile= "NCUGlobalTuples_1.root"
        TreeReader data(inputFile.data());
        Long64_t nTotal=0;
        for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++)
        {
            NNTotal++;
            data.GetEntry(jEntry);
            nTotal++;
            // 0. check the generator-level information and make sure there is a Z->e+e-
            Int_t nGenPar        = data.GetInt("nGenPar");
            TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");
            Int_t* genParId      = data.GetPtrInt("genParId");
            Int_t* genParSt      = data.GetPtrInt("genParSt");
            Int_t* genMomParId   = data.GetPtrInt("genMomParId");
            bool find1Ele = false;
            bool find2Ele = false;
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
                    find1Ele=true;
                    myEles.push_back(thisGen);
                    //if(myEles.size()==2)
                    //{    
                    //    find2Ele=true;       
                    //}
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
            //if(find2Ele)
            //{
            //    neeTotal++;
            //}
            if(find1Ele)
            {
                neeTotal++;
                neeeTotal++;
            }
            //0. has a good vertex
            int nVtx        = data.GetInt("nVtx");
            if(nVtx<1)continue;
            //1. trigger
            //2.Reco  electron
            int nEle = data.GetInt("nEle");
            TClonesArray* eleP4 = (TClonesArray*) data.GetPtrTObject("eleP4");
            vector<bool>& eleIsPassLoose = *((vector<bool>*) data.GetPtr("eleIsPassLoose"));
            vector<bool>& eleIsPassMedium = *((vector<bool>*) data.GetPtr("eleIsPassMedium"));
            vector<bool>& eleIsPassVeto = *((vector<bool>*) data.GetPtr("eleIsPassVeto"));
            vector<TLorentzVector*> goodElectrons;
            goodElectrons.clear();
            bool cut0=false;
            bool cut1=false;
            bool cut2=false;
            bool cut3=false;
            bool cut4=false;
            if(find1Ele)
            {
                for(int ie = 0; ie < nEle; ie++)
                {
                    TLorentzVector* myEle = (TLorentzVector*)eleP4->At(ie);
                    cut0=true;
                    if(fabs(myEle->Eta())>2.5)
                    {
                        continue;
                    }
                    cut1=true;
                    if(!eleIsPassVeto[ie])
                    {

                        continue;
                    }
                    cut2=true;
                    //nelePass[3]++;
                    if(!eleIsPassMedium[ie])
                    {
                        continue;
                    }
                    cut3=true;
                    //nelePass[4]++;
                    if(myEle->Pt()<20 )
                    {
                        continue;
                    }
                    cut4=true;
                    //nelePass[5]++;
                    goodElectrons.push_back(myEle);
                }//End of loop nEle event
                sort(goodElectrons.begin(), goodElectrons.end(), pt_greater);
                if(cut1)
                {
                    nelePass[1]++;
                }
                if(cut2)
                {
                    nelePass[2]++;
                }
                if(cut3)
                {
                    nelePass[3]++;
                }
                if(cut4)
                {
                    nelePass[4]++;
                }
            }
            if(cut0)
            {
                nelePass[0]++;
            }
            //cout<<"size of myEles is ="<<myEles.size()<<endl;
            //3. muon
            int nMu = data.GetInt("nMu");
            TClonesArray* muP4 = (TClonesArray*) data.GetPtrTObject("muP4");
            vector<bool>& isTightMuon = *((vector<bool>*) data.GetPtr("isTightMuon"));
            vector<bool>& isSoftMuon = *((vector<bool>*) data.GetPtr("isSoftMuon"));
            vector<bool>& isGlobalMuon = *((vector<bool>*) data.GetPtr("isGlobalMuon"));
            vector<bool>& isTrackerMuon = *((vector<bool>*) data.GetPtr("isTrackerMuon"));
            float* mudz     = data.GetPtrFloat("mudz");
            float* mudxy     = data.GetPtrFloat("mudxy");
            float* muChi2NDF     = data.GetPtrFloat("muChi2NDF");
            int* muPixelHits   = data.GetPtrInt("muPixelHits");
            int* muTrkLayers   = data.GetPtrInt("muTrkLayers");
            int* muPixelLayers = data.GetPtrInt("muMatches");
            float* muChHadIso     = data.GetPtrFloat("muChHadIso");
            float* muNeHadIso     = data.GetPtrFloat("muNeHadIso");
            float* muGamIso     = data.GetPtrFloat("muGamIso");
            float* muPUPt     = data.GetPtrFloat("muPUPt");
            int* muHits = data.GetPtrInt("muHits");//vaild muon hits?
            int* muMatches = data.GetPtrInt("muMatches");//Matched Muon Stations?
            vector<int> myTMuos;
            vector<int> mySMuos;
            vector<int> myMuos;
            Double_t myMuIso;
            myTMuos.clear();
            mySMuos.clear();
            myMuos.clear();
            for(int im = 0; im < nMu; im++)
            {
                TLorentzVector* myMu = (TLorentzVector*)muP4->At(im);
                double mupT=myMu->Pt();
                myMuIso=(muChHadIso[im] +max(0.,muNeHadIso[im]+muGamIso[im]-0.5*muPUPt[im]))/mupT;
                if( fabs(myMu->Eta())>2.4 ) 
                {
                    continue;
                }
                if(muTrkLayers[im]<5)
                {
                    continue;
                }
                if(isTightMuon[im])
                {
                    if(isGlobalMuon[im])
                    {
                        if(myMu->Pt()>20 && muChi2NDF[im]<10 && muPixelHits[im]>0&&muHits[im]>0 &&muMatches[im]>1&&mudxy[im]<0.2&&mudz[im]<0.5&&myMuIso<=0.15)
                        {
                           myMuos.push_back(im);
                           continue;
                           //cout<<"0"<<endl;
                        }
                    }
                }
                if(isSoftMuon[im])
                {
                    if(isGlobalMuon[im]||isTrackerMuon[im])
                    {
                        if(myMu->Pt()>10&&muPixelLayers[im]>0&&mudxy[im]<0.3&&mudz[im]<20&&myMuIso<=0.25)
                        {
                            myMuos.push_back(im);
                        }
                    }
                }
            }
            //For lepton Selection
            bool Zee = false;
            if(goodElectrons.size()>=2 && myMuos.size()>=2)
            {
                continue;
            }
            nPass[0]++;
            if (goodElectrons.size()<2 && myMuos.size()<2)
            {
                continue;
            }
            nPass[1]++;
            if (goodElectrons.size()>myMuos.size())
            {
                Zee=true;
                nPass[1]++;
            }
            /*
            //4. veto tau (use to identified  and  rejected bg) 
            int nTau = data.GetInt("HPSTau_n");
            TClonesArray* tauP4 = (TClonesArray*) data.GetPtrTObject("HPSTau_4Momentum");
            vector<bool>& disc_decayModeFinding = *((vector<bool>*) data.GetPtr("disc_decayModeFinding"));// DecayModeFinding metho?
            vector<int> myTaus;
            myTaus.clear();
            for(int it=0; it < nTau; it++)
            {
                TLorentzVector* myTau = (TLorentzVector*)tauP4->At(it);
                if( myTau->Pt() < 18 )
                {
                    continue;
                }
                if( fabs(myTau->Eta())>2.3 )
                {
                    continue;
                }
                if( !disc_decayModeFinding[it] )
                {
                    continue;
                }
                myTaus.push_back(it);
            }
            if(myTaus.size()>0)
            {
                continue;
            }
            */
            //5. Z -> ee
            if(Zee)
            {
                double PT1, PT2;
                double PDGZmass=91.1876;
                double PT;
                double deltaMass;
                PT1=goodElectrons[0]->Pt();
                PT2=goodElectrons[1]->Pt();
                if(PT1>25&&PT2>20)
                {
                    TLorentzVector Z_boson_ee;
                    Z_boson_ee=*goodElectrons[0]+*goodElectrons[1];
                    //cout<<"total Mass ="<<MassLL<<endl;
                    deltaMass=abs(PDGZmass-Z_boson_ee.M());
                    //cout<<"deltaMass ="<<deltaMass<<endl;
                    if(deltaMass>15)continue;
                    Z_massee->Fill(Z_boson_ee.M());
                }
            }
            //AK4 Jet
            const int nTHINJets = data.GetInt("THINnJet");
            TClonesArray* thinjetP4 = (TClonesArray*) data.GetPtrTObject("THINjetP4");
            float* thinJetCSV =  data.GetPtrFloat("THINjetCISVV2");
            unsigned int nGoodTHINJets=0;
            std::vector<int> indexForDPhi;
            indexForDPhi.clear();
            for(int ij=0; ij < nTHINJets; ij++)
            {
                TLorentzVector* thisJet = (TLorentzVector*)thinjetP4->At(ij);
                if(thisJet->Pt()<30)
                {
                    continue;
                }
                if(fabs(thisJet->Eta())>4.5)
                {
                    continue;
                }
                // for b-jet (medium ID)
                if(thinJetCSV[ij]<0.6328)//need check
                {
                    continue;
                }
                for(int jj=0; jj<ij; jj++)
                {
                    TLorentzVector* thatJet = (TLorentzVector*)thinjetP4->At(jj);
                    if(thatJet->Pt()<20)
                    {
                        continue;
                    }
                    if(fabs(thatJet->Eta())>2.4)
                    {
                        continue;
                    }
                }// end of inner loop jet
            } // end of outer loop let

        }// end of loop over entries
        //Search eletron cut eff
        cout<<"End of file"<<endl;
    }
    for(int i=0;i<6;i++)
    {
        cout << "nelePass[" << i << "]= " << nelePass[i] << endl;
        efferr(nelePass[i],neeTotal);
    }
     cout<<"Totle event"<<NNTotal<<endl;
    cout<<"Totle true 2 electron"<<neeTotal<<endl;
    cout<<"Totle true electron"<<neeeTotal<<endl;
    string outputfile = "../output/test.root";
    // out Tree branches
	TFile* fout = new TFile(outputfile.c_str(), "RECREATE" );
	fout->Write();
    Z_massee->Write();
    fout->Close();
}