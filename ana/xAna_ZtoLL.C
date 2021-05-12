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
void xAna_ZtoLL(){

    ifstream inputtxtFile("test.txt");
    string inputFile;
    //void some variable
    Int_t elenumbers;
    Int_t munumbers;
    Int_t taunumbers;
    Long64_t neeTotal=0;
    Long64_t nmuTotal=0;
    //Create histrogram
    TH1D *Z_massee = new TH1D("Z_massee", "Z->ee", 150, 0, 150);
    Z_massee->Sumw2();
    Z_massee->GetXaxis()->SetTitle("mZprime");
    TH1D *Z_massmu = new TH1D("Z_massmu", "Z->mu", 150, 0, 150);
    Z_massmu->Sumw2();
    Z_massmu->GetXaxis()->SetTitle("mZprime");

    while(getline(inputtxtFile,inputFile))
    {
        TreeReader data(inputFile.data());
        for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++)
        {
            if( jEntry % 100== 0 )
            {
                fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());
            }
            data.GetEntry(jEntry);
            // 0. check the generator-level information 
            Int_t nGenPar        = data.GetInt("nGenPar");
            TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");
            Int_t* genParId      = data.GetPtrInt("genParId");
            Int_t* genParSt      = data.GetPtrInt("genParSt");
            Int_t* genMomParId   = data.GetPtrInt("genMomParId");
            bool matchee = false;
            bool matchmu = false;
            for(int ig=0; ig < nGenPar; ig++)
            {
                TLorentzVector* thisGen = (TLorentzVector*)genParP4->At(ig);
                int pid=genParId[ig];
                int mompid = genMomParId[ig];
                int status = genParSt[ig];
                if(abs(pid)==11 && mompid==23)
                {
                    matchee=true;
                }
                else if(abs(pid)==13 && mompid==23)
                {
                    matchmu=true; 
                }
            }
            if(matchee)neeTotal++;
            if(matchmu)nmuTotal++;
            //0. has a good vertex
            int nVtx        = data.GetInt("nVtx");
            if(nVtx<1)continue;
            //2. electron
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
                if(myEle->Pt()<20 )continue;
                if(fabs(myEle->Eta())>2.5)continue;
                if(eleIsPassVeto[ie])
                {
                    vetoee.push_back(ie);
                }
                if(!eleIsPassMedium[ie])continue;
                vector<int>::iterator p=find(vetoee.begin(),vetoee.end(),ie);
                if(p !=vetoee.end())
                {
                    vetoee.erase(p);
                }
                goodElectrons.push_back(*myEle);
            }//End of loop nEle event
            if(matchee)
            {
                if(goodElectrons.size()>=2)
                {
                    if(vetoee.size()!=0)continue;
                }
            }
            sort(goodElectrons.begin(), goodElectrons.end(), pt_greater);
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
            vetomu.clear();
            for(int im = 0; im < nMu; im++)
            {
                TLorentzVector* myMu = (TLorentzVector*)muP4->At(im);
                double mupT=myMu->Pt();
                myMuIso=(muChHadIso[im] +max(0.,muNeHadIso[im]+muGamIso[im]-0.5*muPUPt[im]))/mupT;
                if(myMu->Pt()<20)continue;
                if(fabs(myMu->Eta())>2.4)continue;
                if(myMuIso>0.15)continue;
                if(muTrkLayers[im]<5)continue;
                if(isSoftMuon[im])
                {
                    vetomu.push_back(im);
                }
                if(!isTightMuon[im])continue;
                vector<int>::iterator p=find(vetomu.begin(),vetomu.end(),im);
                if(p !=vetomu.end())
                {
                    vetomu.erase(p);
                }
                goodmuons.push_back(*myMu);
            }//End of loop nMu event
            if(matchmu)
            {
                if(goodmuons.size()>=2)
                {
                    if(vetomu.size()!=0)continue;
                }
            }
            sort(goodmuons.begin(), goodmuons.end(), pt_greater);
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
                if( myTau->Pt() < 18 )continue;
                if( fabs(myTau->Eta())>2.3 )continue;
                if(!disc_decayModeFindingNewDMs[it])continue;
                if(!disc_byVVTightIsolationMVA3newDMwLT[it])continue;
                goodtau.push_back(*myTau);
            }//End of loop nTau event
            //Check overlap with e&mu and veto true tau
            bool tauee = false;
            bool taumu = false;
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
                        else if (goodtau[i].DeltaR(goodmuons[j])>0.4)
                        {
                            taumu = true;
                            break;
                        }
                    }
                }
            }
            if(matchee)
            {
                if(tauee)continue;
            }
            if(matchmu)
            {
                if(taumu)continue;
            }
            //Start combinate Z->ee
            double PDGZmass=91.1876;
            if(matchee)
            {
                if(goodElectrons.size()>=2)
                {
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
                            Z_massee->Fill(Z_boson_ee.M());
                        }
                    }
                }
            }
            //Start combinate Z->mu
            if(goodmuons.size()>=2)
            {
                double PT1, PT2;
                double deltaMass;       
                PT1=goodmuons[0].Pt();
                PT2=goodmuons[1].Pt();
                if(PT1>25&&PT2>20)
                {
                    TLorentzVector Z_boson_mu;
                    Z_boson_mu=goodmuons[0]+goodmuons[1];
                    deltaMass=abs(PDGZmass-Z_boson_mu.M());
                    if(deltaMass<15)
                    {
                        Z_massmu->Fill(Z_boson_mu.M());
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
            bool AK4jetee = false;
            bool AK4jetmu = false;
            for(int i=0; i < nTHINJets; i++)
            {
                TLorentzVector* thisJet = (TLorentzVector*)thinjetP4->At(i);
                if(thisJet->Pt()<30)continue;
                if(fabs(thisJet->Eta())>4.5)continue;
                if(matchee)
                {
                    for(int j=0;j<goodElectrons.size();j++)
                    {
                        if(thisJet[i].DeltaR(goodElectrons[j])<0.4)
                        {
                            AK4jetee=true;
                            break;
                        }
                    }
                    if(AK4jetee)continue;
                }
                if(matchmu)
                {
                    for(int j=0;j<goodmuons.size();j++)
                    {
                        if(thisJet[i].DeltaR(goodmuons[j])<0.4)
                        {
                            AK4jetmu=true;
                            break;
                        }
                    }
                    if(AK4jetmu)continue;
                }
                for(int j=0; j < THINjetTrackPt[i].size(); j++)
                {
                    if(THINjetTrackImpdxyError[i][j]>0 &&THINjetTrackImpdzError[i][j]>0 )
                    {
                        float ddz = pow(THINjetTrackImpdz[i][j]/THINjetTrackImpdxyError[i][j],2);
                        float ddxy = pow(THINjetTrackImpdxy[i][j]/THINjetTrackImpdxyError[i][j],2);
                        float chi = sqrt(ddz+ddxy);
                        Chi3D->Fill(chi,eventWeight);
                        Chi3Dlog->Fill(log10(chi),eventWeight);     
                    }
                }
                

            }//End of loop all ThinJet
        }//End of Run all Events

    }//End of Run all files
}//End of code