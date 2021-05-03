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
void ana_ee(){
    TTree outTree("tree","out branches");
    //ifstream inputtxtFile("../ntuple_filelist/signal/DarkMatter_MonoZToLL_NLO_Vector_Mx2-150_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_Mx1-1_ctau-1_13TeV-madgraph.txt");
    ifstream inputtxtFile("test.txt");
    string inputFile;
    //void some variable
    Int_t elenumbers;
    Int_t munumbers;
    Int_t taunumbers;
    Long64_t neeTotal=0;
    Long64_t nPass[20]={0};
    Long64_t nelePass[8]={0};
    //Create histrogram
    TH1D *Z_massee = new TH1D("Z_massee", "Z->ee", 150, 0, 150);
    TH1D *elenumb = new TH1D("elenumb", "ee",5,0,5);
    TH1D *elePT = new TH1D("elePT", "PT",100,0,500);
    TH1D *elePTPT = new TH1D("elePTPT", "After PT",100,0,500);
    TH1D *elePTeta = new TH1D("elePTeta", "After eta",100,0,500);
    TH1D *taunumb = new TH1D("taunumb", "tau",10,0,10);
    TH1D *drtaue = new TH1D("drtaue", "dr",50,0,5);
    while(getline(inputtxtFile,inputFile))
    {
        //std::string inputFile= "NCUGlobalTuples_1.root"
        TreeReader data(inputFile.data());
        Long64_t nTotal=0;
        for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++)
        {
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
            vector<TLorentzVector*> myEles;
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
            }
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
                    elePT->Fill(myEle->Pt());
                    //cout<<"PT ="<<myEle->Pt()<<endl;
                    if(myEle->Pt()<20 )
                    {
                        continue;
                    }
                    elePTPT->Fill(myEle->Pt());
                    cutee[1]=true;
                    if(fabs(myEle->Eta())>2.5)
                    {
                        continue;
                    }
                    elePTeta->Fill(myEle->Pt());
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
                    if(vetoee.size()==0)
                    {
                        cutee[4]=true;
                    }
                }
                if(goodElectrons.size()==0)
                {
                    continue;
                }
                //Sort electron by PT
                sort(goodElectrons.begin(), goodElectrons.end(), pt_greater);
                //4. veto tau (use to identified  and  rejected bg)
                int nTau = data.GetInt("HPSTau_n");
                TClonesArray* tauP4 = (TClonesArray*) data.GetPtrTObject("HPSTau_4Momentum");
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
                taunumb->Fill(goodtau.size());
                bool tauee = false;
            
                if(goodtau.size()>0)
                {
                    for(int i=0;i<goodtau.size();i++)
                    {
                        for(int j=0;j<goodElectrons.size();j++)
                        {
                            double dr = goodtau[i].DeltaR(goodElectrons[j]);
                            drtaue->Fill(dr);
                            //cout<<"dr ="<<dr<<endl;
                            /*
                            if(goodtau[i].DeltaR(goodElectrons[j])>0.4)
                            {
                                //tauee = true;
                                //break;
                            }
                            */
                        }
                        //if(tauee)
                        //{
                        //    break;
                        //}
                
                    }
                    //if(tauee)
                    //{
                    //    continue;
                    //}
                }//End of veto tau
                //Start combination Z boson
                double PDGZmass=91.1876;
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
                for(int i=0;i<=7;i++)
                {
                   if(cutee[i])
                    {
                        nelePass[i]++;
                    }    
                }

            }//End of Z->ee events 
            
        }//End of loop over entries
        cout<<"nTotal = "<<nTotal<<endl;
        cout<<"neeTotal = "<<neeTotal<<endl;
        
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
    elePT->Write();
    elePTPT->Write();
    elePTeta->Write();
    Z_massee->Write();
    drtaue->Write();
    taunumb->Write();
    fout->Close();

}