#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
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
bool pt_greater(const TLorentzVector a, const TLorentzVector b){
	double A = a.Pt();
    double B = b.Pt();
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
    Long64_t nelePass[8]={0};
    //Create histrogram
    TH1D *Z_massee = new TH1D("Z_massee", "Z->ee", 150, 0, 150);
    TH1D *Z_massmu = new TH1D("Z_massmu", "Z->mumu", 150, 0, 150);
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
            bool matchee = false;
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
                    matchee=true;
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
            if(matchee)
            {
                neeTotal++;
            }
            //0. has a good vertex
            int nVtx        = data.GetInt("nVtx");
            //if(nVtx<1)continue;
            //1. trigger
            //2.Reco  electron
            int nEle = data.GetInt("nEle");
            TClonesArray* eleP4 = (TClonesArray*) data.GetPtrTObject("eleP4");
            vector<bool>& eleIsPassLoose = *((vector<bool>*) data.GetPtr("eleIsPassLoose"));
            vector<bool>& eleIsPassMedium = *((vector<bool>*) data.GetPtr("eleIsPassMedium"));
            vector<bool>& eleIsPassVeto = *((vector<bool>*) data.GetPtr("eleIsPassVeto"));
            vector<TLorentzVector> goodElectrons;
            goodElectrons.clear();
            bool cutee[8]={false};
            vector<int> vetoee;
            if(nEle==0)
            {
                continue;
                cout<<"debug"<<endl;
            }
            for(int ie = 0; ie < nEle; ie++)
            {
                TLorentzVector* myEle = (TLorentzVector*)eleP4->At(ie);
                cutee[0]=true;
                if(myEle->Pt()<20 )
                {
                    continue;
                }
                cutee[1]=true;
                if(fabs(myEle->Eta())>2.5)
                {
                    continue;
                }
                cutee[2]=true;
                if(eleIsPassVeto[ie])
                {
                    vetoee.push_back(ie);   
                }
                if(!eleIsPassMedium[ie])
                {
                    continue;
                }
                vector<int>::iterator p=find(vetoee.begin(),vetoee.end(),ie);
                if(p !=vetoee.end())
                {
                    vetoee.erase(p);
                }
                cutee[3]=true;
                    goodElectrons.push_back(*myEle);
                }//End of loop nEle event
                if(goodElectrons.size()>=2)
                {
                    //cout<<"111111"<<endl;
                    if(vetoee.size()==0)
                    {
                        //cout<<"00000"<<endl;
                        cutee[4]=true;
                    }
                }
                sort(goodElectrons.begin(), goodElectrons.end(), pt_greater);
            //cout<<"size of myEles is ="<<myEles.size()<<endl;
            //3. muon
            int nMu = data.GetInt("nMu");
            TClonesArray* muP4 = (TClonesArray*) data.GetPtrTObject("muP4");
            vector<bool>& isTightMuon = *((vector<bool>*) data.GetPtr("isTightMuon"));
            vector<bool>& isSoftMuon = *((vector<bool>*) data.GetPtr("isSoftMuon"));
            vector<bool>& isGlobalMuon = *((vector<bool>*) data.GetPtr("isGlobalMuon"));
            vector<bool>& isTrackerMuon = *((vector<bool>*) data.GetPtr("isTrackerMuon"));
            float* muChHadIso =  data.GetPtrFloat("muChHadIso");
            float* muNeHadIso =  data.GetPtrFloat("muNeHadIso");
            float* muGamIso =  data.GetPtrFloat("muGamIso");
            float* muPUPt =  data.GetPtrFloat("muPUPt");
            int* muTrkLayers =  data.GetPtrInt("muTrkLayers");
            vector<TLorentzVector> goodmuons;
            goodmuons.clear();
            Double_t myMuIso;
            vector<int> vetomu;
            bool cutmu[8]={false};
                for(int im = 0; im < nMu; im++)
                {
                    TLorentzVector* myMu = (TLorentzVector*)muP4->At(im);
                    double mupT=myMu->Pt();
                    myMuIso=(muChHadIso[im] +max(0.,muNeHadIso[im]+muGamIso[im]-0.5*muPUPt[im]))/mupT;
                    if(myMu->Pt()<20) 
                    {
                        continue;
                    }
                    cutmu[0]=true;
                    if(fabs(myMu->Eta())>2.4) 
                    {
                        continue;
                    }
                    cutmu[1]=true;
                    if(myMuIso>0.15)
                    {
                        continue;
                    }
                    cutmu[2]=true;
                    if(muTrkLayers[im]<5)
                    {
                        continue;
                    }
                    cutmu[3]=true;
                    if(isSoftMuon[im])
                    {
                        vetomu.push_back(im);   
                    }
                    if(!isTightMuon[im])
                    {
                        continue;
                    }
                    vector<int>::iterator p=find(vetomu.begin(),vetomu.end(),im);
                    if(p !=vetomu.end())
                    {
                        vetomu.erase(p);
                    }
                    goodmuons.push_back(*myMu);
                }//end of mu loop
                if(goodmuons.size()>=2)
                {
                    //cout<<"111111"<<endl;
                    if(vetomu.size()==0)
                    {
                        cutmu[4]=true;
                        //cout<<"00000"<<endl;
                    }
                }
            sort(goodmuons.begin(), goodmuons.end(), pt_greater);       
            //4. veto tau (use to identified  and  rejected bg) 
            int nTau = data.GetInt("HPSTau_n");
            TClonesArray* tauP4 = (TClonesArray*) data.GetPtrTObject("HPSTau_4Momentum");
            //vector<bool>& disc_decayModeFinding = *((vector<bool>*) data.GetPtr("disc_decayModeFinding"));// DecayModeFinding metho?
            vector<bool>& disc_decayModeFinding = *((vector<bool>*) data.GetPtr("disc_decayModeFinding"));// DecayModeFinding metho?
            vector<bool>& disc_decayModeFinding = *((vector<bool>*) data.GetPtr("disc_decayModeFinding"));// DecayModeFinding metho?
            vector<TLorentzVector> goodtau;
            goodtau.clear();
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
                if(!disc_decayModeFinding[it])
                {
                    continue;
                }

                goodtau.push_back(*myTau);
            }//end of tau loop
            bool tauee = false;
            bool taumu = false;
            if(matchee)
            {
                if(goodtau.size()>0)
                {
                    for(int i=0;i<goodtau.size();i++)
                    {
                        for(int j=0;j<goodElectrons.size();j++)
                        {
                            if(goodtau[i].DeltaR(goodElectrons[j])>0.4)
                            {
                                tauee = true;
                                break;
                            }
                        }
                        if(tauee)
                        {
                            break;
                        }
                
                    }
                }
                if(tauee)
                {
                    continue;
                }
            }
            /*for(int k=0;k<goodmuons.size();k++)
            {
                if(goodtau[i].DeltaR(goodmuons[k])>0.4)
                {
                    cout<<"here"<<endl;
                    continue;
                }
            } 
            */   
            //cout<<"tau size ="<<goodtau.size()<<endl;
            //For lepton Selection
            bool Zee = false;
            bool Zmu = false;
            if (goodElectrons.size() >=2 && goodmuons.size()<2)
            {
                Zee=true;   
            }
            if (goodmuons.size() >=2 && goodElectrons.size()<2)
            {
                Zmu=true;
            }     
            double PDGZmass=91.1876;
            //5. Z -> ee
            if(Zee)
            {
                double PT1, PT2;
                double PT;
                double deltaMass;
                PT1=goodElectrons[0].Pt();
                PT2=goodElectrons[1].Pt();
                cutee[5]=true;
                if(PT1>25&&PT2>20)
                {
                    cutee[6]=true;
                    TLorentzVector Z_boson_ee;
                    Z_boson_ee=goodElectrons[0]+goodElectrons[1];
                    deltaMass=abs(PDGZmass-Z_boson_ee.M());
                    if(deltaMass>15)continue;
                    cutee[7]=true;
                    Z_massee->Fill(Z_boson_ee.M());
                }
            }
            if(Zmu)
            {
                double PT1, PT2;
                double PT;
                double deltaMass;
                PT1=goodmuons[0].Pt();
                PT2=goodmuons[1].Pt();
                cutmu[5]=true;
                if(PT1>25&&PT2>20)
                {
                    cutmu[6]=true;
                    TLorentzVector Z_boson_mu;
                    Z_boson_mu=goodmuons[0]+goodmuons[1];
                    deltaMass=abs(PDGZmass-Z_boson_mu.M());
                    if(deltaMass>15)continue;
                    cutmu[7]=true;
                    Z_massmu->Fill(Z_boson_mu.M());
                }
            }
            if(matchee)
            {
                for(int i=0;i<=7;i++)
                {
                   if(cutee[i])
                    {
                        nelePass[i]++;
                    }    
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
        cout<<"End of file"<<endl;
    }
    for(int i=0;i<8;i++)
    {
        cout << "nelePass[" << i << "]= " << nelePass[i] << endl;
        efferr(nelePass[i],neeTotal);
    }
    //cout<<"Totle true 2 electron"<<neeTotal<<endl;
    string outputfile = "../output/test.root";
    // out Tree branches
	TFile* fout = new TFile(outputfile.c_str(), "RECREATE" );
	fout->Write();
    Z_massee->Write();
    Z_massmu->Write();
    fout->Close();
}