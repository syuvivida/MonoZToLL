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
void ana_Ze(){
    TTree outTree("tree","out branches");
    //ifstream inputtxtFile("../ntuple_filelist/signal/DarkMatter_MonoZToLL_NLO_Vector_Mx2-150_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_Mx1-1_ctau-1_13TeV-madgraph.txt");
    ifstream inputtxtFile("testbkg.txt");
    string inputFile;
    //void some variable
    Int_t elenumbers;
    Int_t munumbers;
    Int_t taunumbers;
    Long64_t neeTotal=0;
    Long64_t nPass[20]={0};
    Long64_t nelePass[9]={0};
    //Create histrogram
    TH1D *Z_massee = new TH1D("Z_massee", "Z->ee", 150, 0, 150);
    Z_massee->Sumw2();
    Z_massee->GetXaxis()->SetTitle("mZprime");

    TH1D* h_eventWeight = new TH1D("h_eventWeight", "eventWeight", 100, -1, 1);
    TH1D *elenumb = new TH1D("elenumb", "ee",5,0,5);
    TH1D *elePT = new TH1D("elePT", "PT",100,0,500);
    TH1D *elePTPT = new TH1D("elePTPT", "After PT",100,0,500);
    TH1D *elePTeta = new TH1D("elePTeta", "After eta",100,0,500);
    TH1D *taunumb = new TH1D("taunumb", "tau",10,0,10);
    TH1D *drtaue = new TH1D("drtaue", "dr",50,0,5);
    TH1D *Chi3D = new TH1D("Chi3D", "Chi3D",100,0,500);
    Chi3D->Sumw2();
    Chi3D->GetXaxis()->SetTitle("Chi3D");
    TH1D *Chi3Dlog = new TH1D("Chi3Dlog", "Chi3D",50,-5,5);
    Chi3Dlog->Sumw2();
    Chi3Dlog->GetXaxis()->SetTitle("Chi3D");

    while(getline(inputtxtFile,inputFile))
    {
        TreeReader data(inputFile.data());
        Long64_t nTotal=0;
        for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++)
        {
            if( jEntry % 1000000 == 0 )
            {
                fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());
            }
            data.GetEntry(jEntry);
            
            nTotal++;
            Float_t mcWeight = data.GetFloat("mcWeight");
            Double_t eventWeight = mcWeight;
            if(eventWeight > 0)
            {
                eventWeight=1;
            }
            else if(eventWeight < 0)
            {
                eventWeight=-1;
            }
            else
            {
                eventWeight=1;
            }
            h_eventWeight->Fill(0.,eventWeight);
            // 0. check the generator-level information and make sure there is a Z->e+e-
            Int_t nGenPar        = data.GetInt("nGenPar");
            TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");
            Int_t* genParId      = data.GetPtrInt("genParId");
            Int_t* genParSt      = data.GetPtrInt("genParSt");
            Int_t* genMomParId   = data.GetPtrInt("genMomParId");
            bool matchee = false;
            bool find2Ele = false;
            vector<TLorentzVector*> myEles;
            myEles.clear();
            for(int ig=0; ig < nGenPar; ig++)
            if(matchee)
            {
                neeTotal++;//count Z->ee events
                //0. has a good vertex
                int nVtx        = data.GetInt("nVtx");
                if(nVtx<1)continue;
                 //2.Reco  electron
                int nEle = data.GetInt("nEle");
                TClonesArray* eleP4 = (TClonesArray*) data.GetPtrTObject("eleP4");
                vector<bool>& eleIsPassLoose = *((vector<bool>*) data.GetPtr("eleIsPassLoose"));
                vector<bool>& eleIsPassMedium = *((vector<bool>*) data.GetPtr("eleIsPassMedium"));
                vector<bool>& eleIsPassVeto = *((vector<bool>*) data.GetPtr("eleIsPassVeto"));
                vector<TLorentzVector> goodElectrons;
                goodElectrons.clear();
                vector<int> vetoee;
                vetoee.clear();
                for(int ie = 0; ie < nEle; ie++)
                {
                    TLorentzVector* myEle = (TLorentzVector*)eleP4->At(ie);
                    if(myEle->Pt()<20 )
                    {
                        continue;
                    }
                    elePTPT->Fill(myEle->Pt());
                    if(fabs(myEle->Eta())>2.5)
                    {
                        continue;
                    }
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
                    goodElectrons.push_back(*myEle);
                }//End of loop nEle event
                if(goodElectrons.size()>=2)
                {
                    if(vetoee.size()!=0)
                    { 
                       continue;
                    }
                }
                //Sort electron by PT
                sort(goodElectrons.begin(), goodElectrons.end(), pt_greater);
                //4. veto tau (use to identified  and  rejected bg)
                int nTau = data.GetInt("HPSTau_n");
                TClonesArray* tauP4 = (TClonesArray*) data.GetPtrTObject("HPSTau_4Momentum");
                vector<bool>& disc_decayModeFinding = *((vector<bool>*) data.GetPtr("disc_decayModeFinding"));// DecayModeFinding metho?
                vector<bool>& disc_decayModeFindingNewDMs = *((vector<bool>*) data.GetPtr("disc_decayModeFindingNewDMs"));// DecayModeFinding metho?
                vector<bool>& disc_byVVTightIsolationMVA3newDMwLT = *((vector<bool>*) data.GetPtr("disc_byVVTightIsolationMVA3newDMwLT"));// DecayModeFinding metho?
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
                    if(!disc_decayModeFindingNewDMs[it])
                    {
                        continue;
                    }
                    if(!disc_byVVTightIsolationMVA3newDMwLT[it])
                    {
                        continue;
                    }
                    goodtau.push_back(*myTau);
                }//end of tau loop
                taunumb->Fill(goodtau.size());
                bool tauee = false;
                //Check overlap with tau and veto true tau
                if(goodtau.size()>0)
                {
                    for(int i=0;i<goodtau.size();i++)
                    {
                        for(int j=0;j<goodElectrons.size();j++)
                        {
                            double dr = goodtau[i].DeltaR(goodElectrons[j]);
                            drtaue->Fill(dr);
                            if(goodtau[i].DeltaR(goodElectrons[j])>0.4)
                            {
                                tauee = true;
                                break;
                            }  
                        }
                    }
                }//End of veto tau
                if(tauee)
                {
                    continue;
                }
                //Start combination Z boson
                if(goodElectrons.size()>=2)
                {
                    double PDGZmass=91.1876;
                    double PT1, PT2;
                    double deltaMass;       
                    PT1=goodElectrons[0].Pt();
                    PT2=goodElectrons[1].Pt();
                    if(PT1>25&&PT2>20)
                    {
                        TLorentzVector Z_boson_ee;
                        Z_boson_ee=goodElectrons[0]+goodElectrons[1];
                        deltaMass=abs(PDGZmass-Z_boson_ee.M());
                        if(deltaMass<15)
                        {
                            Z_massee->Fill(Z_boson_ee.M(),eventWeight);
                        }
                    }
                }
                //Save Thin Jet some variable
                int nTHINJets  = data.GetInt("THINnJet");
                TClonesArray* thinjetP4 = (TClonesArray*) data.GetPtrTObject("THINjetP4");
                Int_t* THINjetNTracks      = data.GetPtrInt("THINjetNTracks");
                vector<float>   *THINjetTrackImpdz = data.GetPtrVectorFloat("THINjetTrackImpdz", nTHINJets);
                vector<float>   *THINjetTrackImpdzError = data.GetPtrVectorFloat("THINjetTrackImpdzError", nTHINJets);
                vector<float>   *THINjetTrackImpdxy = data.GetPtrVectorFloat("THINjetTrackImpdxy", nTHINJets);
                vector<float>   *THINjetTrackImpdxyError = data.GetPtrVectorFloat("THINjetTrackImpdxyError", nTHINJets);
                vector<float>   *THINjetTrackPt =  data.GetPtrVectorFloat("THINjetTrackPt", nTHINJets);
                vector<float> nTracks;
                for(int i=0; i < nTHINJets; i++)
                {
                    TLorentzVector* thisJet = (TLorentzVector*)thinjetP4->At(i);
                    if(thisJet->Pt()<30)
                    {
                        continue;
                    }
                    if(fabs(thisJet->Eta())>4.5)
                    {
                        continue;
                    }
                    for(int k=0;k<THINjetTrackImpdz[i].size();k++)
                    {
                        double PVTrackFraction;
                        PVTrackFraction=abs(THINjetTrackImpdz[i][k])/(float)THINjetNTracks[i];
                        //cout<<"PVTrackFraction ="<<PVTrackFraction<<endl;
                    }
                    for(int j=0; j < THINjetTrackPt[i].size(); j++)
                    {
                        if(THINjetTrackPt[i][j] < 1)
                        {
                            continue;
                        }
                        if(THINjetTrackImpdxyError[i][j]>0 &&THINjetTrackImpdzError[i][j]>0 )
                        {
                            float ddz = pow(THINjetTrackImpdz[i][j]/THINjetTrackImpdxyError[i][j],2);
                            float ddxy = pow(THINjetTrackImpdxy[i][j]/THINjetTrackImpdxyError[i][j],2);
                            float chi = sqrt(ddz+ddxy);
                            Chi3D->Fill(chi,eventWeight);
                            Chi3Dlog->Fill(log10(chi),eventWeight);
                            //X[i].push_back(xx);
                        }
                        //cout<<"dz ="<<THINjetTrackImpdz[i][j]<<endl;
                    }
                }
            }//End of Z->ee events 
        }//End of loop over entries
    }//End of loop all files  
    string outputfile = "../output/Zeebk.root";
    // out Tree branches
	TFile* outFile = new TFile(outputfile.c_str(), "RECREATE" );
	outFile->Write();
    h_eventWeight->Write();
    elePT->Write();
    elePTPT->Write();
    elePTeta->Write();
    Z_massee->Write();
    drtaue->Write();
    taunumb->Write();
    Chi3D->Write();
    Chi3Dlog->Write();
    outFile->Close();
}