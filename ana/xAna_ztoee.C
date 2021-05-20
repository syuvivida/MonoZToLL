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
void xAna_ztoee(string inputtxtFilename = "../ntuple_filelist/DYJets_HTBin/DYJetsToLL_M-50_HT-70to100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt", string outputname = "fullDYHT70_100")
{
    //./ntuple_filelist/signal/DarkMatter_MonoZToLL_NLO_Vector_Mx2-1_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_Mx1-0p1_ctau-1_13TeV-madgraph.txt
    TTree outTree("tree","out branches");
    //ifstream inputtxtFile("../ntuple_filelist/DYJets_HTBin/DYJetsToLL_M-50_HT-100to200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.txt");
    ifstream inputtxtFile(inputtxtFilename.data());
    string inputFile;
    //void some variable
    Int_t elenumbers;
    Int_t munumbers;
    Int_t taunumbers;
    Long64_t neeTotal=0;
    Long64_t nPass[20]={0};
    Long64_t nelePass[9]={0};
    vector<Int_t> SumeventWeight={0};
    int SSum =0; 
    //Create histrogram
    TH1D *Z_massee = new TH1D("Z_massee", "Z->ee", 150, 0, 150);
    Z_massee->Sumw2();
    Z_massee->GetXaxis()->SetTitle("mZprime");
    TH1D *h_event = new TH1D("h_event", "events", 10, 0, 10);
    TH1D* h_eventWeight = new TH1D("h_eventWeight", "eventWeight", 100, -1, 1);
    //TH1D* h_eventtotWeight = new TH1D("h_eventtotWeight", "eventWeight", 100, -1, 1);
    TH1D *elenumb = new TH1D("elenumb", "ee",5,0,5);
    TH1D *elePT = new TH1D("elePT", "PT",100,0,500);
    TH1D *elePTPT = new TH1D("elePTPT", "After PT",100,0,500);
    TH1D *elePTeta = new TH1D("elePTeta", "After eta",100,0,500);
    TH1D *taunumb = new TH1D("taunumb", "tau",10,0,10);
    TH1D *drtaue = new TH1D("drtaue", "dr",50,0,5);
    TH1D *Chi3D = new TH1D("Chi3D", "Chi3D",100,0,500);
    Chi3D->Sumw2();
    Chi3D->GetXaxis()->SetTitle("Chi3D");
    TH1D *Chi3Dlog = new TH1D("Chi3Dlog", "Chi3D",100,-10,10);
    Chi3Dlog->Sumw2();
    Chi3Dlog->GetXaxis()->SetTitle("Chi3D");
    TH1D *Chi3DPaperlog = new TH1D("Chi3DPaperlog", "Chi3DPaperlog",100,-10,10);
    Chi3DPaperlog->Sumw2();
    //Chi3DPaperlog->GetXaxis()->SetTitle("Chi3D(0.01)");
    while(getline(inputtxtFile,inputFile))
    {
        TreeReader data(inputFile.data());
        Long64_t nTotal=0;
        for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++)
        {
            if( jEntry % 2000 == 0 )
            {
                fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());
            }
            data.GetEntry(jEntry);
            Float_t mcWeight = data.GetFloat("mcWeight");
            Double_t eventWeight = mcWeight;
            //Double_t SumeventWeight;
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
            SumeventWeight.push_back(eventWeight);
            //SumeventWeight=eventWeight;
            SSum=SSum+eventWeight;
            h_event->Fill(1.0);
            h_eventWeight->Fill(0.,eventWeight);
            // 0. check the generator-level information and make sure there is a Z->e+e-
            Int_t nGenPar        = data.GetInt("nGenPar");
            TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");
            Int_t* genParId      = data.GetPtrInt("genParId");
            Int_t* genParSt      = data.GetPtrInt("genParSt");
            Int_t* genMomParId   = data.GetPtrInt("genMomParId");
            bool matchee = false;
            vector<TLorentzVector*> myEles;
            vector<TLorentzVector> dquark;
            dquark.clear();
            myEles.clear();
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
                //chi2Id 18
                if(abs(pid)==1 && abs(mompid)==18)
                {
                    dquark.push_back(*thisGen);
                }
            }
            //cout<<"dquark size"<<dquark.size()<<endl;
            if(matchee)neeTotal++;//count Z->ee events
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
                
                if(myEle->Pt()<20)continue;
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
                    if(vetoee.size()!=0)
                    { 
                       continue;
                    }
                }
            }
            //Sort electron by PT
            sort(goodElectrons.begin(), goodElectrons.end(), pt_greater);
            //4. veto tau
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
            }//end of tau loop
            bool tauee = false;
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
                }
                if(tauee)continue;
            }//End of veto tau
            //Start combination Z boson
            if(goodElectrons.size()>=2)
            {
                double PDGZmass=91.1876;
                double PT1 = goodElectrons[0].Pt();
                double PT2 = goodElectrons[1].Pt();
                if(PT1>25&&PT2>20)
                {
                    TLorentzVector Z_boson_ee;
                    Z_boson_ee=goodElectrons[0]+goodElectrons[1];
                    double deltaMass=abs(PDGZmass-Z_boson_ee.M());
                    if(deltaMass<15)
                    {
                        Z_massee->Fill(Z_boson_ee.M());
                    }
                }
            }
            //Save Thin Jet some variable
            int nTHINJets  = data.GetInt("THINnJet");
            TClonesArray* thinjetP4 = (TClonesArray*) data.GetPtrTObject("THINjetP4");
            Int_t* THINjetNTracks      = data.GetPtrInt("THINjetNTracks");
            Float_t* THINjetVtx3DSig = data.GetPtrFloat("THINjetVtx3DSig");
            vector<float>   *THINjetTrackImpdz = data.GetPtrVectorFloat("THINjetTrackImpdz", nTHINJets);
            vector<float>   *THINjetTrackImpdzError = data.GetPtrVectorFloat("THINjetTrackImpdzError", nTHINJets);
            vector<float>   *THINjetTrackImpdxy = data.GetPtrVectorFloat("THINjetTrackImpdxy", nTHINJets);
            vector<float>   *THINjetTrackImpdxyError = data.GetPtrVectorFloat("THINjetTrackImpdxyError", nTHINJets);
            vector<float>   *THINjetTrackPt =  data.GetPtrVectorFloat("THINjetTrackPt", nTHINJets);
            for(int i=0; i < nTHINJets; i++)
            {
                TLorentzVector* thisJet = (TLorentzVector*)thinjetP4->At(i);
                if(thisJet->Pt()<30)continue;
                if(fabs(thisJet->Eta())>2.5)continue;
                bool jetee=false;
                for(int j=0;j<goodElectrons.size();j++)
                {
                    if(thisJet->DeltaR(goodElectrons[j])<0.4)
                    {
                        //cout<<"00"<<endl;
                        jetee=true;
                        break;
                    }
                }
                if(jetee)continue;
                /*
                bool jetdquark=false;
                for(int j=0;j<dquark.size();j++)
                {
                    //cout<<"dr "<<thisJet->DeltaR(dquark[j])<<endl;
                    if(thisJet->DeltaR(dquark[j])<0.4)
                    {
                        jetdquark=true;
                        break;
                    }
                }
                if(!jetdquark)continue;
                */
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
                        Chi3Dlog->Fill(log10(chi),eventWeight);
                    }
                    if(THINjetTrackImpdxyError[i][j]>0 &&THINjetTrackImpdzError[i][j]>0 )
                    {
                        float dz = pow(THINjetTrackImpdz[i][j]/0.01,2);
                        float dxy = pow(THINjetTrackImpdxy[i][j]/THINjetTrackImpdxyError[i][j],2);
                        float chiP = sqrt(dz+dxy);
                        Chi3DPaperlog->Fill(log10(chiP),eventWeight);
                    }

                }
            }//End of thin jet 
        }//End of loop over entries
    }//End of loop all files
    cout<<"SumWeight = "<<SSum<<endl;
    
        string outputfile = outputname+".root";
        // out Tree branches
	    TFile* outFile = new TFile(outputfile.c_str(), "RECREATE" );
	    outFile->Write();
        Z_massee->Write();
        h_event->Write();
        Chi3Dlog->Write();
        Chi3DPaperlog->Write();
        h_eventWeight->Write();
        outFile->Close();
        //Z_massee->Delete();
        //delete *Chi3Dlog;
        //delete *Chi3DPaperlog;
        //delete *h_eventWeight;

}
void loop_all()
{
    ifstream inputtxtFilete("testsig.txt");
    string inputFile;
    string iInput, iOutput;
    stringstream ss;
    while(getline(inputtxtFilete,inputFile))
    {
        ss << inputFile;
        ss >> iInput >> iOutput;
        xAna_ztoee(iInput,iOutput);
    }
}